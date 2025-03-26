use std::io;
use std::{ffi::OsStr, os::windows::ffi::OsStrExt, ptr};

#[derive(serde::Serialize)]
pub struct PartitionInfo {
    pub drive_letter: String,
    pub label: String,
    pub fs_type: String,
    pub total_size: u64,
    pub free_space: u64,
}

type DWORD = u32;
type LPCWSTR = *const u16;
type LPWSTR = *mut u16;
type BOOL = i32;
type UlargeInteger = u64;

#[link(name = "kernel32")]
extern "system" {
    fn GetVolumeInformationW(
        lpRootPathName: LPCWSTR,
        lpVolumeNameBuffer: LPWSTR,
        nVolumeNameSize: DWORD,
        lpVolumeSerialNumber: *mut DWORD,
        lpMaximumComponentLength: *mut DWORD,
        lpFileSystemFlags: *mut DWORD,
        lpFileSystemNameBuffer: LPWSTR,
        nFileSystemNameSize: DWORD,
    ) -> BOOL;

    fn GetDiskFreeSpaceExW(
        lpDirectoryName: LPCWSTR,
        lpFreeBytesAvailable: *mut UlargeInteger,
        lpTotalNumberOfBytes: *mut UlargeInteger,
        lpTotalNumberOfFreeBytes: *mut UlargeInteger,
    ) -> BOOL;
}

fn to_wstring(s: &str) -> Vec<u16> {
    OsStr::new(s).encode_wide().chain(Some(0)).collect()
}

fn get_filesystem_type(root_path: &str) -> io::Result<String> {
    let mut fs_type_buffer = [0u16; 32];
    let root_path_w = to_wstring(root_path);

    let success = unsafe {
        GetVolumeInformationW(
            root_path_w.as_ptr(),
            ptr::null_mut(),
            0,
            ptr::null_mut(),
            ptr::null_mut(),
            ptr::null_mut(),
            fs_type_buffer.as_mut_ptr(),
            fs_type_buffer.len() as DWORD,
        )
    };

    if success != 0 {
        let fs_type = String::from_utf16_lossy(&fs_type_buffer)
            .trim_end_matches('\0')
            .to_string();
        Ok(fs_type)
    } else {
        Err(io::Error::last_os_error())
    }
}

fn get_disk_space(root_path: &str) -> io::Result<(u64, u64)> {
    let mut total_bytes = 0u64;
    let mut free_bytes = 0u64;
    let root_path_w = to_wstring(root_path);

    let success = unsafe {
        GetDiskFreeSpaceExW(
            root_path_w.as_ptr(),
            ptr::null_mut(),
            &mut total_bytes as *mut _,
            &mut free_bytes as *mut _,
        )
    };

    if success != 0 {
        Ok((total_bytes, free_bytes))
    } else {
        Err(io::Error::last_os_error())
    }
}

fn get_disk_label(root_path: &str) -> io::Result<String> {
    let mut label_buffer = [0u16; 32];
    let root_path_w = to_wstring(root_path);

    let success = unsafe {
        GetVolumeInformationW(
            root_path_w.as_ptr(),
            label_buffer.as_mut_ptr(),
            label_buffer.len() as DWORD,
            ptr::null_mut(),
            ptr::null_mut(),
            ptr::null_mut(),
            ptr::null_mut(),
            0,
        )
    };

    if success != 0 {
        let label = String::from_utf16_lossy(&label_buffer)
            .trim_end_matches('\0')
            .to_string();
        Ok(label)
    } else {
        Err(io::Error::last_os_error())
    }
}

pub fn detect_partitions() -> Vec<PartitionInfo> {
    use rayon::prelude::*;

    (b'A'..=b'Z')
        .into_par_iter()
        .filter_map(|drive| {
            let drive_letter = format!("{}:", drive as char);
            let root_path = format!("{}:\\", drive as char);

            match (
                get_filesystem_type(&root_path),
                get_disk_space(&root_path),
                get_disk_label(&root_path),
            ) {
                (Ok(fs_type), Ok((total, free)), Ok(label)) => Some(PartitionInfo {
                    drive_letter,
                    label,
                    fs_type,
                    total_size: total,
                    free_space: free,
                }),
                _ => None,
            }
        })
        .collect()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_partition_detection() {
        let partitions = detect_partitions();
        assert!(!partitions.is_empty());
        for part in partitions {
            println!(
                "Drive {} ({}) - Total: {} GB",
                part.drive_letter,
                part.fs_type,
                part.total_size / 1_000_000_000
            );
        }
    }

    #[test]
    fn test_drive_c() {
        let info = get_filesystem_type("C:\\").unwrap();
        assert!(matches!(info.as_str(), "NTFS" | "FAT32"));

        let (total, _) = get_disk_space("C:\\").unwrap();
        assert!(total > 0);
    }
}
