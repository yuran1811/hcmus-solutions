use std::{
    fs::File,
    io::{Read, Result, Seek, SeekFrom},
    str,
};

use crate::models::{fat_fs::FATTable, partition::PartitionRawInfo};

use super::parser::parse_partition_type;

pub fn read_u16_le(bytes: &[u8]) -> u16 {
    u16::from_le_bytes(bytes[..2].try_into().unwrap_or_else(|_| [0; 2]))
}

pub fn read_u24_le(bytes: &[u8]) -> u32 {
    bytes[..3]
        .iter()
        .enumerate()
        .fold(0, |acc, (i, &b)| acc | ((b as u32) << (8 * (i))))
}

pub fn read_u32_le(bytes: &[u8]) -> u32 {
    u32::from_le_bytes(bytes[..4].try_into().unwrap_or_else(|_| [0; 4]))
}

pub fn read_u48_le(bytes: &[u8]) -> u64 {
    bytes[..6]
        .iter()
        .enumerate()
        .fold(0, |acc, (i, &b)| acc | ((b as u64) << (1 << (3 + i))))
}

pub fn read_u64_le(bytes: &[u8]) -> u64 {
    u64::from_le_bytes(bytes[..8].try_into().unwrap_or_else(|_| [0; 8]))
}

pub fn read_fat32_raw_info(file: &mut File, start_lba: u32) -> std::io::Result<(String, String)> {
    file.seek(SeekFrom::Start(start_lba as u64 * 512))?;

    let mut vbr = [0u8; 512];
    file.read_exact(&mut vbr)?;

    Ok((
        String::from_utf8_lossy(&vbr[0x47..0x47 + 11])
            .trim()
            .to_string(),
        String::from_utf8_lossy(&vbr[0x52..0x52 + 8])
            .trim()
            .to_string(),
    ))
}

pub fn read_fat_table(
    file: &mut File,
    fat_count: u8,
    fat_size: u64,
    fat_start: u64,
) -> std::io::Result<Vec<FATTable>> {
    let mut raw_fats = vec![0u8; fat_size as usize];
    let mut fats = vec![];

    file.seek(SeekFrom::Start(fat_start))?;
    for _ in 0..fat_count {
        file.read_exact(&mut raw_fats)?;
        fats.push(FATTable::new(raw_fats.clone()));
    }

    Ok(fats)
}

pub fn read_mbr_disk(path: &str, partition_idx: Option<usize>) -> Result<Vec<PartitionRawInfo>> {
    if path.is_empty() || !path.starts_with(r"\\.\") {
        return Ok(vec![]);
    }

    if File::open(path).is_err() {
        return Ok(vec![]);
    }

    let mut file = File::open(path)?;
    let mut buffer = [0u8; 512];
    file.read_exact(&mut buffer)?;

    // Check boot signature
    if &buffer[0x1FE..] != [0x55, 0xAA] {
        return Err(std::io::Error::new(
            std::io::ErrorKind::InvalidData,
            "Invalid MBR boot sector",
        ));
    }

    let mut partitions = Vec::new();
    for i in 0..4 {
        // MBR disk has four partitions (16-byte entries)
        let offset = 0x1BE + i * 16;
        let entry = &buffer[offset..offset + 16];

        let partition_type = entry[4];
        let total_sectors = read_u32_le(&entry[12..16]);
        if partition_type == 0x00 || total_sectors == 0 {
            break; // Skip empty partitions or partitions with zero sectors
        }

        let is_bootable = entry[0] == 0x80;
        let start_lba = u32::from_le_bytes([entry[8], entry[9], entry[10], entry[11]]);

        let (raw_label, raw_type) = read_fat32_raw_info(&mut file, start_lba)?;

        partitions.push(PartitionRawInfo {
            is_bootable,
            partition_type,
            start_lba,
            total_sectors,

            raw_label: raw_label.clone(),
            raw_type: raw_type.clone(),

            size: (total_sectors as u64) * 512, // Assuming 512 bytes per sector
            part_label: raw_label,
            part_type: parse_partition_type(partition_type).to_string(),
        });

        if let Some(idx) = partition_idx {
            if idx == i {
                return Ok(vec![partitions[idx].clone()]);
            }
        }
    }

    Ok(partitions)
}
