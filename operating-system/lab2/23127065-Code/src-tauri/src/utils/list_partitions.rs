use std::process::Command;

use crate::models::{
    fat_fs::FAT32,
    partition::{PartitionInfo, PartitionRawInfo},
};

use super::data::reader::read_mbr_disk;

pub fn list_linux_partitions() {
    match Command::new("lsblk")
        .args(&["-o", "NAME,SIZE,TYPE,MOUNTPOINT"])
        .output()
    {
        Ok(output) => {
            println!("{}", String::from_utf8_lossy(&output.stdout));
        }
        Err(e) => {
            println!("Error running lsblk: {}", e);
        }
    }
}

pub fn list_windows_disks() {
    match Command::new("wmic")
        .args(&["diskdrive", "get", "Caption,DeviceID,Model"])
        .output()
    {
        Ok(output) => {
            println!("{}", String::from_utf8_lossy(&output.stdout));
        }
        Err(e) => {
            println!("Error running wmic: {}", e);
        }
    }
}

pub fn list_mac_partitions() {
    match Command::new("diskutil").arg("list").output() {
        Ok(output) => {
            println!("{}", String::from_utf8_lossy(&output.stdout));
        }
        Err(e) => {
            println!("Error running diskutil: {}", e);
        }
    }
}

pub fn list_fat32_paritions() -> Vec<(PartitionRawInfo, String)> {
    let mut part_list = Vec::new();

    for i in 0..32 {
        let path = format!(r"\\.\PhysicalDrive{}", i);
        match read_mbr_disk(&path, None) {
            Ok(partitions) => {
                if partitions.is_empty() && i > 0 {
                    break;
                }

                for partition in partitions.iter() {
                    if partition.raw_type.starts_with("FAT32") {
                        part_list.push((partition.clone(), path.clone()));
                    }
                }
            }
            Err(_) => {
                break;
            }
        }
    }

    part_list
}

pub fn list_fat32_paritions_by_letter() -> Vec<PartitionInfo> {
    (b'A'..=b'Z')
        .into_iter()
        .filter_map(|drive| {
            let drive_letter = format!("{}:", drive as char);
            match FAT32::open(&format!("\\\\.\\{}", drive_letter), None) {
                Ok(fat) => Some(PartitionInfo {
                    drive_letter,
                    label: fat.get_label(),
                    fs_type: fat.raw_part_info.clone().unwrap_or_default().raw_type,
                    total_size: fat.volume_size * fat.bytes_per_sector as u64,
                    free_space: fat.volume_size * fat.bytes_per_sector as u64 - fat.used_space,
                }),
                _ => None,
            }
        })
        .collect()
}
