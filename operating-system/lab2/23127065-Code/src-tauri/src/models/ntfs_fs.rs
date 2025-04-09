use std::collections::HashMap;
use std::fs::{File, OpenOptions};
use std::io::{Read, Seek, SeekFrom};

use crate::utils::data::{parser, reader};
use parser::{parse_sized, parse_sized_signed};
use reader::{read_u16_le, read_u32_le, read_u48_le, read_u64_le};

use super::fs::{FileSystem, FsError, Result};

#[derive(Debug, Clone)]
pub struct NTFSAttribute {
    pub type_code: u32,
    pub resident: bool,
    pub content: Vec<u8>,
    pub runs: Vec<(i64, u64)>, // (LCN offset, length in clusters)
}

#[derive(Debug, Clone)]
pub struct MFTEntry {
    pub file_id: u64,
    pub parent_id: u64,
    pub name: String,
    pub size: u64,
    pub is_directory: bool,
    pub attributes: HashMap<u32, NTFSAttribute>,
}

impl MFTEntry {
    pub fn parse(data: &[u8]) -> Result<Self> {
        if &data[0..4] != b"FILE" {
            return Err(FsError::InvalidStructure);
        }

        let fixup_offset = read_u16_le(&data[4..6]) as usize;
        let fixup_count = read_u16_le(&data[6..8]) as usize;
        let sector_size = 512;
        let mut fixed_data = data.to_vec();

        // Apply fixup array
        let fixups = &data[fixup_offset..fixup_offset + fixup_count * 2];
        for (i, sector) in fixed_data.chunks_mut(sector_size).enumerate() {
            if i >= fixup_count {
                break;
            }
            let fixup = &fixups[i * 2..i * 2 + 2];
            sector[sector_size - 2..sector_size].copy_from_slice(fixup);
        }

        let mut entry = MFTEntry {
            file_id: read_u48_le(&fixed_data[44..50]),
            parent_id: 0,
            name: String::new(),
            size: 0,
            is_directory: false,
            attributes: HashMap::new(),
        };

        let mut offset = read_u16_le(&fixed_data[20..22]) as usize;
        while offset + 4 <= fixed_data.len() {
            let attr_type = read_u32_le(&fixed_data[offset..offset + 4]);
            let attr_len = read_u32_le(&fixed_data[offset + 4..offset + 8]) as usize;

            if attr_type == 0xFFFFFFFF || attr_len == 0 {
                break;
            }
            if offset + attr_len > fixed_data.len() {
                break;
            }

            let resident = fixed_data[offset + 8] == 0;
            let content_size = read_u32_le(&fixed_data[offset + 16..offset + 20]) as usize;
            let content_offset =
                offset + read_u16_le(&fixed_data[offset + 20..offset + 22]) as usize;

            let mut attribute = NTFSAttribute {
                type_code: attr_type,
                resident,
                content: Vec::new(),
                runs: Vec::new(),
            };

            if resident {
                attribute.content =
                    fixed_data[content_offset..content_offset + content_size].to_vec();
            } else {
                let runlist_offset =
                    offset + read_u16_le(&fixed_data[offset + 32..offset + 34]) as usize;
                Self::parse_runlist(&fixed_data[runlist_offset..], &mut attribute.runs)?;
            }

            match attr_type {
                0x10 => Self::parse_standard_info(&attribute, &mut entry),
                0x30 => Self::parse_file_name(&attribute, &mut entry)?,
                0x80 => Self::parse_data(&attribute, &mut entry),
                _ => {}
            }

            entry.attributes.insert(attr_type, attribute);
            offset += attr_len;
        }

        Ok(entry)
    }

    pub fn parse_runlist(data: &[u8], runs: &mut Vec<(i64, u64)>) -> Result<()> {
        let mut offset = 0;
        let mut current_lcn: i64 = 0;

        while offset < data.len() {
            let header = data[offset];
            if header == 0 {
                break;
            }
            offset += 1;

            let len_size = (header & 0x0F) as usize;
            let lcn_size = (header >> 4) as usize;

            if offset + len_size + lcn_size > data.len() {
                return Err(FsError::InvalidStructure);
            }

            let len = parse_sized(&data[offset..offset + len_size], len_size);
            offset += len_size;

            let lcn_delta = if lcn_size > 0 {
                parse_sized_signed(&data[offset..offset + lcn_size], lcn_size)
            } else {
                0
            };
            offset += lcn_size;

            current_lcn += lcn_delta;
            runs.push((current_lcn, len));
        }

        Ok(())
    }

    pub fn parse_standard_info(attr: &NTFSAttribute, entry: &mut MFTEntry) {
        if attr.content.len() >= 72 {
            entry.is_directory = read_u32_le(&attr.content[56..60]) & 0x10000000 != 0;
        }
    }

    pub fn parse_file_name(attr: &NTFSAttribute, entry: &mut MFTEntry) -> Result<()> {
        if attr.content.len() >= 74 {
            entry.parent_id = read_u48_le(&attr.content[0..6]);
            let name_len = attr.content[64] as usize;
            let name_data = &attr.content[66..66 + name_len * 2];

            entry.name = String::from_utf16(
                &name_data
                    .chunks_exact(2)
                    .map(|c| read_u16_le(c))
                    .collect::<Vec<u16>>(),
            )?;
        }
        Ok(())
    }

    fn parse_data(attr: &NTFSAttribute, entry: &mut MFTEntry) {
        if attr.resident {
            entry.size = attr.content.len() as u64;
        } else if attr.content.len() >= 48 {
            entry.size = read_u64_le(&attr.content[48..56]);
        }
    }
}

pub struct NTFS {
    pub file: File,
    pub bytes_per_cluster: u64,
    pub mft: HashMap<u64, MFTEntry>,
    pub current_dir: u64,
    pub cluster_size: u64,
    pub mft_start: u64,
}

impl NTFS {
    pub fn find_entry(&self, name: &str) -> Result<&MFTEntry> {
        self.list_directory()?
            .iter()
            .find(|e| e.name == name)
            .map(|e| *e) // Dereference double reference
            .ok_or(FsError::NotFound)
    }

    pub fn open(path: &str) -> Result<Self> {
        let mut file = OpenOptions::new().read(true).open(path)?;
        let mut boot = [0u8; 512];
        file.read_exact(&mut boot)?;

        if &boot[3..11] != b"NTFS    " {
            return Err(FsError::InvalidStructure);
        }

        let bytes_per_sector = 2u32.pow(boot[11] as u32);
        let sectors_per_cluster = boot[13] as u32;
        let bytes_per_cluster = bytes_per_sector * sectors_per_cluster;
        let mft_cluster = read_u64_le(&boot[48..56]);
        let mft_start = mft_cluster * bytes_per_cluster as u64;
        let mft_record_size = 2u32.pow((boot[64] as i8).abs() as u32);

        let mut fs = NTFS {
            file,
            bytes_per_cluster: bytes_per_cluster as u64,
            mft: HashMap::new(),
            current_dir: 5, // Root directory ID
            cluster_size: bytes_per_cluster as u64,
            mft_start,
        };

        fs.load_mft(mft_record_size)?;
        Ok(fs)
    }

    pub fn load_mft(&mut self, record_size: u32) -> Result<()> {
        let mut offset = self.mft_start;
        let mut buffer = vec![0u8; record_size as usize];

        for _ in 0..1024 {
            // Load first 1024 entries
            self.file.seek(SeekFrom::Start(offset))?;
            self.file.read_exact(&mut buffer)?;

            match MFTEntry::parse(&buffer) {
                Ok(mut entry) => {
                    if entry.file_id == 5 {
                        // Root entry
                        entry.parent_id = 5;
                    }
                    self.mft.insert(entry.file_id, entry);
                }
                Err(_) => break,
            }

            offset += record_size as u64;
        }

        Ok(())
    }

    pub fn list_directory(&self) -> Result<Vec<&MFTEntry>> {
        let current = self.mft.get(&self.current_dir).ok_or(FsError::NotFound)?;

        Ok(self
            .mft
            .values()
            .filter(|e| e.parent_id == current.file_id && e.name != "." && e.name != "..")
            .collect())
    }

    pub fn read_file_data(&mut self, entry: &MFTEntry) -> Result<Vec<u8>> {
        let data_attr = entry.attributes.get(&0x80).ok_or(FsError::NotFound)?;

        if data_attr.resident {
            return Ok(data_attr.content.clone());
        }

        let mut content = Vec::with_capacity(entry.size as usize);
        for (lcn, len) in &data_attr.runs {
            if *lcn < 0 {
                continue;
            }

            let offset = *lcn as u64 * self.cluster_size;
            let length = *len as u64 * self.cluster_size;

            self.file.seek(SeekFrom::Start(offset))?;
            let mut chunk = vec![0u8; length as usize];
            self.file.read_exact(&mut chunk)?;

            content.extend_from_slice(&chunk);
        }

        content.truncate(entry.size as usize);
        Ok(content)
    }

    pub fn change_dir(&mut self, name: &str) -> Result<()> {
        let entries = self.list_directory()?;
        let entry = entries
            .iter()
            .find(|e| e.name.eq_ignore_ascii_case(name) && e.is_directory)
            .ok_or(FsError::NotFound)?;

        self.current_dir = entry.file_id;
        Ok(())
    }

    pub fn print_info(&self) {
        println!(
            "NTFS Info:\n  Bytes per cluster: {}\n  MFT start: {}",
            self.bytes_per_cluster, self.mft_start
        );
    }
}

impl FileSystem for NTFS {
    fn current_path(&self) -> String {
        let mut path = Vec::new();
        let mut current_id = self.current_dir;

        while current_id != 5 {
            // Until root
            if let Some(entry) = self.mft.get(&current_id) {
                path.push(entry.name.clone());
                current_id = entry.parent_id;
            } else {
                break;
            }
        }

        format!(
            "\\{}",
            path.into_iter().rev().collect::<Vec<_>>().join("\\")
        )
    }
}

pub trait NTFSFileSystem: FileSystem {
    fn list_rdir(&mut self) -> Result<Vec<&MFTEntry>>;
    fn list_dir(&mut self, path: &str) -> Result<Vec<MFTEntry>>;
    fn read_file(&mut self, name: &str) -> Result<Vec<u8>>;
}

impl NTFSFileSystem for NTFS {
    fn list_rdir(&mut self) -> Result<Vec<&MFTEntry>> {
        let entries = self.list_directory()?;

        Ok(entries)
    }

    fn list_dir(&mut self, path: &str) -> Result<Vec<MFTEntry>> {
        let entries = self.list_directory()?;
        let mut result = Vec::new();

        for entry in entries {
            if entry.name.eq_ignore_ascii_case(path) {
                result.push(entry.clone());
            }
        }

        Ok(result)
    }

    fn read_file(&mut self, name: &str) -> Result<Vec<u8>> {
        // Immutable phase: find entry and extract needed data
        let (cluster_size, runs, size) = {
            let entries = self.list_directory()?;
            let entry = entries
                .iter()
                .find(|e| e.name == name && !e.is_directory)
                .ok_or(FsError::NotFound)?;

            let data_attr = entry.attributes.get(&0x80).ok_or(FsError::NotFound)?;

            if data_attr.resident {
                return Ok(data_attr.content.clone());
            }

            // Clone needed values from immutable context
            (self.cluster_size, data_attr.runs.clone(), entry.size)
        };

        // Mutable phase: perform I/O operations
        let mut content = Vec::with_capacity(size as usize);
        for (lcn, len) in &runs {
            if *lcn < 0 {
                continue;
            }

            let offset = *lcn as u64 * cluster_size;
            let length = *len as u64 * cluster_size;

            self.file.seek(SeekFrom::Start(offset))?;
            let mut chunk = vec![0u8; length as usize];
            self.file.read_exact(&mut chunk)?;

            content.extend_from_slice(&chunk);
        }

        content.truncate(size as usize);
        Ok(content)
    }
}
