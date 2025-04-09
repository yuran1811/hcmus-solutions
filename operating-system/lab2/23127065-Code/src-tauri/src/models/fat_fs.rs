use std::{
    cmp::Ordering,
    collections::HashMap,
    fs::{File, OpenOptions},
    io::{Read, Seek, SeekFrom},
    time::SystemTime,
};

use crate::utils::{
    base::{calc_fat32_used_space, format_attributes, system_time_to_local_strings},
    data::{
        parser::{
            parse_boot_sector, parse_file_content, parse_path, parse_short_name, parse_times,
        },
        reader::{read_fat_table, read_u32_le},
    },
};

use super::{
    fs::{DirectoryNode, FileMetadata, FileSystem, FsError, Result},
    partition::PartitionRawInfo,
};

#[derive(Debug, Clone, Copy)]
pub struct FATAttributes(pub u8);

impl FATAttributes {
    pub const READ_ONLY: u8 = 0x01;
    pub const HIDDEN: u8 = 0x02;
    pub const SYSTEM: u8 = 0x04;
    pub const VOLUME_ID: u8 = 0x08;
    pub const DIRECTORY: u8 = 0x10;
    pub const ARCHIVE: u8 = 0x20;

    pub fn contains(self, flag: u8) -> bool {
        (self.0 & flag) != 0
    }
}

#[derive(Debug, Clone)]
pub struct FATEntry {
    pub path: String,
    pub name: String,
    pub short_name: String,
    pub extension: String,
    pub attributes: FATAttributes,
    pub created: SystemTime,
    pub modified: SystemTime,
    pub accessed: SystemTime,
    pub cluster: u32,
    pub size: u32,
    pub is_deleted: bool,
    pub is_directory: bool,
}

impl std::fmt::Display for FATEntry {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(
            f,
            "{}\"{}\" (size: {}) (created: {}, modified: {})",
            if self.is_directory { "[DIR]" } else { "" },
            self.name,
            self.size,
            self.created
                .duration_since(SystemTime::UNIX_EPOCH)
                .unwrap_or_default()
                .as_secs(),
            self.modified
                .duration_since(SystemTime::UNIX_EPOCH)
                .unwrap_or_default()
                .as_secs(),
        )
    }
}

impl FATEntry {
    pub fn is_active(&self) -> bool {
        self.attributes.contains(
            FATAttributes::DIRECTORY
                | FATAttributes::ARCHIVE
                | FATAttributes::READ_ONLY
                | FATAttributes::SYSTEM,
        ) && !self.is_deleted
    }

    pub fn is_valid(&self, name: &str) -> bool {
        self.is_active()
            && !self.name.is_empty()
            && self.name.to_uppercase().trim() == name.to_uppercase().trim()
    }
}

#[derive(Debug)]
pub struct FATTable {
    pub fat: Vec<u32>,
    pub len: u32,
}

impl FATTable {
    pub fn new(data: Vec<u8>) -> Self {
        let fat = data
            .chunks_exact(4)
            .map(|chunk| read_u32_le(chunk))
            .collect::<Vec<_>>();
        let len = fat.len() as u32;

        Self { fat, len }
    }

    pub fn is_in_bounds(&self, cluster: u32) -> bool {
        2 <= cluster && cluster <= 0x0FFF_FFEF && cluster < self.len
    }

    pub fn get_cluster_chain(&self, start: u32) -> Vec<u32> {
        if !self.is_in_bounds(start) {
            return vec![];
        }

        let mut chain = Vec::new();
        let mut current = start;
        loop {
            chain.push(current);

            current = self.fat[current as usize];
            if !self.is_in_bounds(current) {
                break;
            }
        }

        chain
    }
}

#[derive(Debug, Clone)]
pub struct DET {
    pub entries: Vec<FATEntry>,
    pub len: u32,
}

impl DET {
    pub fn new(data: Vec<u8>, path: &str) -> Self {
        let mut entries = Vec::new();
        let mut long_name = String::new();

        for entry in data.chunks(32) {
            if entry[0] == 0x00 {
                break; // End of directory entries
            }

            if entry[0xB] == 0x0F {
                // Long file name entry
                let mut name = String::from_utf8_lossy(&entry[1..1 + 10]).to_string();
                name.push_str(&String::from_utf8_lossy(&entry[0x0E..0x0E + 12]));
                name.push_str(&String::from_utf8_lossy(&entry[0x1C..0x1C + 4]));
                let mut x = name.as_bytes().to_vec();

                if let Some(pos) = x.iter().rposition(|&value| value == 0) {
                    x.truncate(pos);
                }

                long_name.insert_str(0, &String::from_utf8(x).unwrap_or_else(|_| String::new()));
                continue;
            }

            let attr = FATAttributes(entry[0xB]);
            let (created, modified, accessed) = parse_times(entry);
            let short_name = parse_short_name(entry);
            let mut name = if long_name.len() > 0 {
                long_name.to_string()
            } else {
                short_name.0
            };
            long_name.clear();

            name.retain(|c| c != '\0');

            entries.push(FATEntry {
                path: format!("{}\\{}", path, name),
                name,
                short_name: short_name.1,
                extension: short_name.2,
                attributes: attr,
                created,
                modified,
                accessed,
                cluster: read_u32_le(&[entry[26], entry[27], entry[20], entry[21]]),
                size: read_u32_le(&entry[0x1C..0x1C + 4]),
                is_deleted: entry[0] == 0xE5,
                is_directory: attr.contains(FATAttributes::DIRECTORY),
            });
        }

        let len = entries.len() as u32;

        Self { entries, len }
    }

    pub fn find_entry(&self, name: &str) -> Option<FATEntry> {
        self.entries.iter().find(|e| e.is_valid(name)).cloned()
    }

    pub fn get_active_entries(&self) -> Vec<FATEntry> {
        self.entries
            .iter()
            .filter(|e| e.is_active())
            .cloned()
            .collect()
    }
}

#[derive(Debug)]
pub struct FATClusterManager {
    pub data_start: u64,
    pub sectors_per_cluster: u64,
    pub bytes_per_sector: u64,
}

impl FATClusterManager {
    pub fn get_cluster_size(&self) -> u64 {
        self.sectors_per_cluster * self.bytes_per_sector
    }

    pub fn get_cluster_offset(&self, cluster: u64) -> u64 {
        self.data_start + (cluster - 2) * self.get_cluster_size()
    }

    pub fn read_cluster_data(&mut self, file: &mut File, cluster: u32) -> Result<Vec<u8>> {
        let mut buffer = vec![0u8; self.get_cluster_size() as usize];
        file.seek(SeekFrom::Start(self.get_cluster_offset(cluster as u64)))?;
        file.read_exact(&mut buffer)?;
        Ok(buffer)
    }

    pub fn read_all_cluster_data(&mut self, file: &mut File, chain: Vec<u32>) -> Result<Vec<u8>> {
        if chain.is_empty() {
            return Ok(vec![]);
        }

        let mut data = Vec::new();
        for cluster in chain {
            data.extend(self.read_cluster_data(file, cluster)?);
        }

        Ok(data)
    }
}

#[derive(Debug)]
pub struct FAT32PartitionStructure {
    pub boot: Vec<u8>,
    pub fats: Vec<FATTable>,
    pub rdet: DET,
    pub all_det: HashMap<u32, DET>,
    pub cluster_manager: FATClusterManager,
}

pub struct FAT32BootSector {
    pub bytes_per_sector: u16,
    pub sectors_per_cluster: u8,
    pub reserved_sectors: u16,
    pub volume_size: u32,
    pub fat_count: u8,
    pub sectors_per_fat: u32,
    pub root_dir_cluster: u32,
    pub fat_size: u64,
    pub fat_type: String,
    pub data_start: u64,
}

pub struct FAT32 {
    pub raw_part_info: Option<PartitionRawInfo>,
    pub file: File,
    pub bytes_per_sector: u32,
    pub sectors_per_cluster: u32,
    pub reserved_sectors: u32,
    pub used_space: u64,
    pub volume_size: u64,
    pub fat_size: u64,
    pub fat_count: u32,
    pub fat_type: String,
    pub root_dir_cluster: u32,
    pub data_start: u64,
    pub path_stack: Vec<String>,
    pub structure: FAT32PartitionStructure,
}

impl FAT32 {
    pub fn open(path: &str, raw_part_info: Option<PartitionRawInfo>) -> Result<Self> {
        let mut file = OpenOptions::new().read(true).open(path)?;
        let mut boot = vec![0u8; 512];

        file.read_exact(&mut boot)?;
        if &boot[0x1FE..] != [0x55, 0xAA] {
            // Boot Record Signature check
            return Err(FsError::InvalidStructure);
        }

        let FAT32BootSector {
            bytes_per_sector,
            sectors_per_cluster,
            reserved_sectors,
            volume_size,
            fat_count,
            sectors_per_fat: _,
            root_dir_cluster,
            fat_size,
            fat_type,
            data_start,
        } = parse_boot_sector(&boot);

        if !fat_type.starts_with("FAT32") {
            return Err(FsError::InvalidStructure);
        }

        let mut cluster_manager = FATClusterManager {
            data_start,
            sectors_per_cluster: sectors_per_cluster as u64,
            bytes_per_sector: bytes_per_sector as u64,
        };

        // Read the FAT tables
        let fats = read_fat_table(
            &mut file,
            fat_count,
            fat_size,
            reserved_sectors as u64 * bytes_per_sector as u64,
        )?;

        // Read the root directory entry table (RDET)
        let mut all_det = HashMap::new();
        all_det.insert(
            root_dir_cluster,
            DET::new(
                cluster_manager.read_all_cluster_data(
                    &mut file,
                    fats[0].get_cluster_chain(root_dir_cluster),
                )?,
                &path,
            ),
        );

        Ok(Self {
            raw_part_info: raw_part_info.clone(),
            file,
            bytes_per_sector: bytes_per_sector as u32,
            sectors_per_cluster: sectors_per_cluster as u32,
            reserved_sectors: reserved_sectors as u32,
            used_space: calc_fat32_used_space(
                &fats,
                sectors_per_cluster as u64,
                bytes_per_sector as u64,
            ),
            volume_size: volume_size as u64,
            fat_size,
            fat_count: fat_count as u32,
            fat_type,
            root_dir_cluster,
            data_start,
            path_stack: vec![path.to_string()],
            structure: FAT32PartitionStructure {
                boot,
                fats,
                rdet: all_det[&root_dir_cluster].clone(),
                all_det,
                cluster_manager,
            },
        })
    }

    pub fn print_info(&self) {
        println!(
            "FAT32 Info:\n  Bytes per sector: {}\n  Sectors per cluster: {}\n  Reserved sectors: {}\n  Volume size (sectors): {}\n  FAT size (sectors): {}\n  FAT count: {}\n  FAT n entries: {}\n  FAT Type: {}\n  RDET at cluster: {}\n  Data start: {}",
            self.bytes_per_sector,
            self.sectors_per_cluster,
            self.reserved_sectors,
            self.volume_size,
            self.fat_size,
            self.fat_count,
            self.structure.fats[0].len,
            self.fat_type,
            self.root_dir_cluster,
            self.data_start
        );
    }

    pub fn get_label(&self) -> String {
        let rdet = self.structure.rdet.clone();
        let label_entry = rdet
            .entries
            .iter()
            .find(|e| e.attributes.contains(FATAttributes::VOLUME_ID));

        if label_entry.is_none() {
            return String::new();
        }
        let label_entry = label_entry.unwrap();

        label_entry.name.clone()
    }
}

impl FileSystem for FAT32 {
    fn current_path(&self) -> String {
        if self.path_stack.is_empty() {
            "\\".to_string()
        } else {
            format!("{}", self.path_stack.join("\\"))
        }
    }
}

pub trait FAT32FileSystem: FileSystem {
    fn cd(&mut self, path: &str) -> Result<()>;

    fn list_rdir(&mut self) -> Result<Vec<FATEntry>>;
    fn list_dir(&mut self, path: &str) -> Result<Vec<FATEntry>>;

    fn fetch_file(&mut self, path: &str) -> Result<FATEntry>;
    fn fetch_metadata(&mut self, path: &str) -> Result<(FileMetadata, Option<FATEntry>)>;

    fn read_file(&mut self, path: &str, parse_text: Option<bool>) -> Result<(Vec<u8>, String)>;

    fn read_immediate_children(&mut self, path: &str) -> Result<DirectoryNode>;
    fn build_shallow_node(&mut self, path: &str) -> Result<DirectoryNode>;
}

impl FAT32FileSystem for FAT32 {
    fn cd(&mut self, path: &str) -> Result<()> {
        let dirs = parse_path(path);
        if dirs.is_empty() {
            return Ok(());
        }

        if dirs.len() == 1 {
            if path == ".." {
                self.path_stack.pop();
                return Ok(());
            } else if path == "." {
                return Ok(());
            }
        }

        let tmp_path = format!("{}\\{}", self.current_path(), path);
        if self.list_dir(&tmp_path)?.is_empty() {
            return Err(FsError::NotFound);
        }

        self.path_stack.push(dirs.join("\\"));
        Ok(())
    }

    fn list_rdir(&mut self) -> Result<Vec<FATEntry>> {
        let entries = self.structure.all_det[&self.root_dir_cluster].get_active_entries();

        if entries.is_empty() {
            Ok(vec![])
        } else {
            Ok(entries)
        }
    }

    fn list_dir(&mut self, path: &str) -> Result<Vec<FATEntry>> {
        let dirs = parse_path(&path);
        if dirs.is_empty() {
            return self.list_rdir();
        }

        let mut cur_det = self.structure.rdet.clone();
        for dir in dirs {
            let entry = cur_det.find_entry(&dir);
            if entry.is_none() {
                return Err(FsError::NotFound);
            }

            let entry = entry.unwrap();
            if entry.is_directory {
                if let Some(det) = self.structure.all_det.get(&entry.cluster) {
                    cur_det = det.clone();
                } else {
                    let chain = self.structure.fats[0].get_cluster_chain(entry.cluster);
                    let data = self
                        .structure
                        .cluster_manager
                        .read_all_cluster_data(&mut self.file, chain)?;

                    let det = DET::new(data, path);

                    self.structure.all_det.insert(entry.cluster, det.clone());
                    cur_det = det;
                }
            }
        }

        Ok(cur_det
            .entries
            .iter()
            .filter(|e| e.is_active() && e.name != "." && e.name != "..")
            .map(|e| e.clone())
            .collect())
    }

    fn fetch_file(&mut self, path: &str) -> Result<FATEntry> {
        let mut dirs = parse_path(&path);
        let entry = if dirs.len() > 0 {
            let name = dirs.pop().unwrap_or_default();
            let dir_path = dirs.join("\\");

            self.list_dir(&dir_path)?
                .into_iter()
                .find(|e| e.is_valid(&name))
        } else {
            None
        };

        if entry.is_none() {
            Err(FsError::NotFound)
        } else {
            Ok(entry.unwrap())
        }
    }

    fn fetch_metadata(&mut self, path: &str) -> Result<(FileMetadata, Option<FATEntry>)> {
        let mut dirs = parse_path(path);
        if dirs.is_empty() {
            return Ok((
                FileMetadata {
                    attributes: format_attributes(0),
                    created_date: String::new(),
                    created_time: String::new(),
                    name: self.get_label(),
                    size: self.volume_size,
                },
                None,
            ));
        }

        let dir = dirs.pop().unwrap_or_default();
        let dir_path = dirs.join("\\");
        let file_metadata = self.fetch_file(path);
        let entry = if file_metadata.is_ok() {
            Some(file_metadata.unwrap())
        } else {
            match self.list_dir(&dir_path) {
                Ok(entries) => entries.into_iter().find(|e| e.is_valid(&dir)),
                Err(_) => return Err(FsError::NotFound),
            }
        };
        if entry.is_none() || !entry.clone().unwrap().is_active() {
            return Err(FsError::NotFound);
        }

        let entry = entry.unwrap();
        let (created_date, created_time) = system_time_to_local_strings(entry.created);

        Ok((
            FileMetadata {
                attributes: format_attributes(entry.attributes.0 as u32),
                created_date,
                created_time,
                name: entry.name.clone(),
                size: entry.size as u64,
            },
            Some(entry.clone()),
        ))
    }

    fn read_file(&mut self, path: &str, parse_text: Option<bool>) -> Result<(Vec<u8>, String)> {
        let entry = self.fetch_file(path);
        if entry.is_err() {
            return Err(entry.unwrap_err());
        }

        let entry = entry.unwrap();
        if !entry.is_active() {
            return Ok((vec![], String::new()));
        }

        let parse_text = parse_text.unwrap_or(false);

        let chain = self.structure.fats[0].get_cluster_chain(entry.cluster);
        let mut content = Vec::with_capacity(entry.size as usize);
        for cluster in chain {
            let data = self
                .structure
                .cluster_manager
                .read_cluster_data(&mut self.file, cluster)?;

            let limit = entry.size.min(data.len() as u32) as usize;
            content.extend_from_slice(&data[0..limit]);
        }

        let parsed_content = parse_file_content(&content);

        Ok((
            content,
            if parse_text {
                parsed_content
            } else {
                String::new()
            },
        ))
    }

    fn read_immediate_children(&mut self, path: &str) -> Result<DirectoryNode> {
        let mut node = self.build_shallow_node(path)?;
        let entries = self.list_dir(path)?;
        for entry in entries {
            if entry.is_active() {
                let child_node = self.build_shallow_node(&entry.path)?;
                node.children.push(child_node);
            }
        }

        node.children.sort_by(|a, b| {
            match (!a.is_file, !b.is_file) {
                (true, false) => Ordering::Less, // a is a folder, b is not → a comes first
                (false, true) => Ordering::Greater, // b is a folder, a is not → b comes first
                _ => a.name.cmp(&b.name),        // both are same type: sort by name
            }
        });

        Ok(node)
    }

    fn build_shallow_node(&mut self, path: &str) -> Result<DirectoryNode> {
        let (metadata, entry) = self.fetch_metadata(path)?;
        let is_dir = if entry.is_none() {
            true
        } else {
            entry.clone().unwrap().is_directory
        };

        Ok(DirectoryNode {
            name: metadata.name.clone(),
            path: path.into(),
            is_file: !is_dir,
            children: Vec::new(),
            metadata,
            has_children: is_dir && self.list_dir(path)?.len() > 0,
            loaded: false,
        })
    }
}
