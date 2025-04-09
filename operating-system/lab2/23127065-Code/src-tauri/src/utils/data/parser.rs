use std::time::SystemTime;

use crate::models::fat_fs::FAT32BootSector;

use super::{
    convert_fat_time,
    reader::{read_u16_le, read_u24_le, read_u32_le},
};

pub fn parse_sized(data: &[u8], size: usize) -> u64 {
    if data.len() < size || size < 1 {
        return 0;
    }

    let mut value = 0u64;
    for (i, &byte) in data.iter().take(size).enumerate() {
        value |= (byte as u64).wrapping_shl(i as u32 * 8);
    }
    value
}

pub fn parse_sized_signed(data: &[u8], size: usize) -> i64 {
    let unsigned = parse_sized(data, size);
    let sign_bit = 1u64.wrapping_shl(size as u32 * 8 - 1);
    if unsigned & sign_bit != 0 {
        let adjustment = 1u64.wrapping_shl(size as u32 * 8);
        if adjustment == 1 {
            i64::MIN
        } else {
            unsigned as i64 - adjustment as i64
        }
    } else {
        unsigned as i64
    }
}

pub fn parse_boot_sector(data: &[u8]) -> FAT32BootSector {
    let bytes_per_sector = read_u16_le(&data[0xB..0xD]);
    let sectors_per_cluster = data[0xD];
    let reserved_sectors = read_u16_le(&data[0x0E..0x10]);
    let volume_size = read_u32_le(&data[0x20..0x24]);
    let fat_count = data[0x10];
    let sectors_per_fat = read_u32_le(&data[0x24..0x28]);
    let root_dir_cluster = read_u32_le(&data[0x2C..0x30]);
    let fat_type = String::from_utf8_lossy(&data[0x52..0x52 + 8])
        .trim()
        .to_string();

    let fat_size = sectors_per_fat as u64 * bytes_per_sector as u64;
    let data_start = (reserved_sectors as u64 + fat_count as u64 * sectors_per_fat as u64)
        * bytes_per_sector as u64;

    FAT32BootSector {
        bytes_per_sector,
        sectors_per_cluster,
        reserved_sectors,
        volume_size,
        fat_count,
        sectors_per_fat,
        root_dir_cluster,
        fat_size,
        fat_type,
        data_start,
    }
}

pub fn parse_partition_type(ptype: u8) -> &'static str {
    // See more at: https://datarecovery.com/rd/hexadecimal-flags-for-partition-type/
    match ptype {
        0x00 => "Empty",
        0x01 => "FAT12",
        0x04 => "FAT16 <32MB",
        0x05 => "Extended",
        0x06 => "FAT16 (large)",
        0x07 => "NTFS/exFAT",
        0x0B => "FAT32 (CHS)",
        0x0C => "FAT32 (LBA)",
        0x0E => "FAT16 (LBA)",
        0x0F => "Extended (LBA)",
        0x82 => "Linux swap",
        0x83 => "Linux native",
        0x8E => "Linux LVM",
        0xA5 => "FreeBSD",
        0xA6 => "OpenBSD",
        0xAF => "Mac OS X HFS+",
        0xEE => "GPT Protective",
        0xEF => "EFI System Partition",
        _ => "Unknown",
    }
}

pub fn parse_path(path: &str) -> Vec<String> {
    let mut path = path.to_string();
    if let Some(colon_index) = path.find(':') {
        if colon_index > 0 {
            path = path[(colon_index + 1)..].to_string();
        }
    }

    let path = path.replace("\\\\", "\\").replace('/', "\\");
    path.trim_start_matches('\\')
        .split('\\')
        .map(|p| String::from(p.trim()))
        .filter(|s| !s.is_empty())
        .collect()
}

pub fn parse_short_name(entry: &[u8]) -> (String, String, String) {
    let base = String::from_utf8_lossy(&entry[0..8]).trim().to_string();
    let ext = String::from_utf8_lossy(&entry[8..11]).trim().to_string();

    if ext.is_empty() {
        (format!("{}", base), base, ext)
    } else {
        (format!("{}.{}", base, ext), base, ext)
    }
}

pub fn parse_times(entry: &[u8]) -> (SystemTime, SystemTime, SystemTime) {
    let time_created = read_u24_le(&entry[0xD..0xD + 3]);
    let date_created = read_u16_le(&entry[0x10..0x10 + 2]);
    let time_modified = read_u16_le(&entry[0x16..0x16 + 2]);
    let date_modified = read_u16_le(&entry[0x18..0x18 + 2]);
    let date_accessed = read_u16_le(&entry[0x12..0x12 + 2]);

    (
        convert_fat_time(date_created, time_created, true),
        convert_fat_time(date_modified, time_modified as u32, false),
        convert_fat_time(date_accessed, 0, false),
    )
}

pub fn parse_file_content(entry: &[u8]) -> String {
    String::from_utf8(entry.to_vec()).unwrap_or_default()

    // let mut content = String::new();
    // for i in 0..entry.len() {
    //     if entry[i] == 0x00 {
    //         break;
    //     }
    //     content.push(entry[i] as char);
    // }
    // content
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_sized() {
        // 4-byte normal case
        let data = [0x01, 0x02, 0x03, 0x04];
        assert_eq!(parse_sized(&data, 4), 0x04030201);

        // 1-byte minimal case
        assert_eq!(parse_sized(&[0xAB], 1), 0xAB);

        // 8-byte maximum size
        let data = [0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88];
        assert_eq!(parse_sized(&data, 8), 0x8877665544332211);

        // Partial data usage
        let data = [0x99, 0xAA, 0xBB, 0xCC];
        assert_eq!(parse_sized(&data, 2), 0xAA99);
    }

    #[test]
    fn test_parse_sized_with_less_data() {
        // Test where the passed size is less than the length of data slice.
        let data = [0xAA, 0xBB, 0xCC, 0xDD];
        // Using only the first two bytes in little endian: 0xBBAA.
        let expected = 0xBBAA;
        assert_eq!(parse_sized(&data, 2), expected);
    }

    #[test]
    fn test_parse_sized_max_values() {
        // Test maximum values for different sizes
        assert_eq!(parse_sized(&[0xFF], 1), 0xFF);
        assert_eq!(parse_sized(&[0xFF, 0xFF], 2), 0xFFFF);
        assert_eq!(parse_sized(&[0xFF; 4], 4), 0xFFFFFFFF);
        assert_eq!(parse_sized(&[0xFF; 8], 8), 0xFFFFFFFFFFFFFFFF);
    }

    #[test]
    fn test_parse_sized_zero() {
        // Zero value tests
        assert_eq!(parse_sized(&[0x00], 1), 0);
        assert_eq!(parse_sized(&[0x00, 0x00], 2), 0);
        assert_eq!(parse_sized(&[0x00; 8], 8), 0);
    }

    #[test]
    fn test_parse_sized_signed_positive() {
        // 2-byte positive
        assert_eq!(parse_sized_signed(&[0xFF, 0x7F], 2), 32767);

        // 4-byte positive
        let data = [0x00, 0x00, 0x00, 0x7F];
        assert_eq!(parse_sized_signed(&data, 4), 0x7F000000);
    }

    #[test]
    fn test_parse_sized_signed_negative() {
        // 1-byte negative
        assert_eq!(parse_sized_signed(&[0xFF], 1), -1);

        // 2-byte negative
        assert_eq!(parse_sized_signed(&[0x01, 0x80], 2), -32767);

        let data = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80];

        // 4-byte negative (i32::MIN)
        assert_eq!(parse_sized_signed(&data[4..], 4), -2147483648);

        // 8-byte negative (i64::MIN)
        assert_eq!(parse_sized_signed(&data, 8), i64::MIN);
    }

    #[test]
    fn test_parse_sized_signed_sign_extension() {
        // 3-byte negative with different padding
        let data = [0xCD, 0xAB, 0x80]; // 0x80ABCD in LE
        assert_eq!(parse_sized_signed(&data, 3), -0x7F5433);
    }

    #[test]
    fn test_parse_partition_type() {
        // Test known partition types
        assert_eq!(parse_partition_type(0x00), "Empty");
        assert_eq!(parse_partition_type(0x07), "NTFS/exFAT");
        assert_eq!(parse_partition_type(0x0C), "FAT32 (LBA)");
        assert_eq!(parse_partition_type(0x83), "Linux native");
        assert_eq!(parse_partition_type(0xEF), "EFI System Partition");

        // Test unknown partition type
        assert_eq!(parse_partition_type(0x42), "Unknown");
        assert_eq!(parse_partition_type(0xFF), "Unknown");
    }

    #[test]
    fn test_parse_path() {
        let path = "\\\\path\\to\\file.txt";
        let expected = vec!["path".to_string(), "to".to_string(), "file.txt".to_string()];
        let result = parse_path(path);
        assert_eq!(result, expected);

        // Additional path tests
        let path2 = "/another/path/to/file.txt";
        let expected2 = vec![
            "another".to_string(),
            "path".to_string(),
            "to".to_string(),
            "file.txt".to_string(),
        ];
        let result2 = parse_path(path2);
        assert_eq!(result2, expected2);

        let path3 = "C:\\Windows\\System32";
        let expected3 = vec!["Windows".to_string(), "System32".to_string()];
        let result3 = parse_path(path3);
        assert_eq!(result3, expected3);

        // Test with empty segments
        let path4 = "\\\\server\\share\\\\folder\\\\";
        let expected4 = vec![
            "server".to_string(),
            "share".to_string(),
            "folder".to_string(),
        ];
        let result4 = parse_path(path4);
        assert_eq!(result4, expected4);

        // Test with whitespace between segments
        let path5 = "  \\  path  \\  to  \\file name.txt  ";
        let expected5 = vec![
            "path".to_string(),
            "to".to_string(),
            "file name.txt".to_string(),
        ];
        let result5 = parse_path(path5);
        assert_eq!(result5, expected5);
    }
}
