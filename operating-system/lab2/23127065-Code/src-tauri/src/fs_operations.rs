use chrono::{DateTime, Local};
use std::{
    cmp::Ordering,
    fs,
    os::windows::fs::MetadataExt,
    path::{Path, PathBuf},
    time::SystemTime,
};

#[derive(serde::Serialize, Debug)]
pub struct DirectoryNode {
    pub name: String,
    pub path: PathBuf,
    pub is_file: bool,
    pub children: Vec<DirectoryNode>,
    pub metadata: FileMetadata,
    pub has_children: bool,
    pub loaded: bool,
}

#[derive(serde::Serialize, Debug)]
pub struct FileMetadata {
    pub name: String,
    pub attributes: String,
    pub created_date: String,
    pub created_time: String,
    pub size: u64,
}

fn format_attributes(attrs: u32) -> String {
    let mut attributes = Vec::new();

    if attrs & 0x1 != 0 {
        attributes.push("Read-only");
    }
    if attrs & 0x2 != 0 {
        attributes.push("Hidden");
    }
    if attrs & 0x4 != 0 {
        attributes.push("System");
    }
    if attrs & 0x10 != 0 {
        attributes.push("Directory");
    }
    if attrs & 0x20 != 0 {
        attributes.push("Archive");
    }
    if attrs & 0x40 != 0 {
        attributes.push("Device");
    }
    if attrs & 0x80 != 0 {
        attributes.push("Normal");
    }
    if attrs & 0x100 != 0 {
        attributes.push("Temporary");
    }
    if attrs & 0x400 != 0 {
        attributes.push("Offline");
    }

    attributes.join(", ")
}

fn system_time_to_local_strings(time: SystemTime) -> (String, String) {
    let datetime: DateTime<Local> = time.into();
    (
        datetime.format("%Y-%m-%d").to_string(),
        datetime.format("%H:%M:%S").to_string(),
    )
}

pub fn get_file_metadata(path: &str) -> Result<FileMetadata, String> {
    let path = Path::new(path);
    let metadata = fs::metadata(path).map_err(|e| format!("Metadata error: {}", e))?;

    let (created_date, created_time) = system_time_to_local_strings(
        metadata
            .created()
            .map_err(|e| format!("Creation time error: {}", e))?,
    );

    Ok(FileMetadata {
        name: path
            .file_name()
            .and_then(|n| n.to_str())
            .unwrap_or("Unknown")
            .to_string(),
        attributes: format_attributes(metadata.file_attributes()),
        created_date,
        created_time,
        size: if metadata.is_file() {
            metadata.len()
        } else {
            0
        },
    })
}

pub fn read_text_file(path: &str) -> Result<String, String> {
    let content = fs::read_to_string(path).map_err(|e| format!("Read error: {}", e))?;

    if content.is_empty() {
        return Err("File is empty".into());
    }

    Ok(content)
}

fn extract_metadata(path: &Path) -> Result<FileMetadata, String> {
    get_file_metadata(path.to_str().unwrap_or_default())
}

pub fn read_immediate_children(path: &str) -> Result<DirectoryNode, String> {
    let path = Path::new(path);
    let mut node = build_shallow_node(path)?;

    match fs::read_dir(path) {
        Ok(entries) => {
            for entry in entries {
                match entry {
                    Ok(entry) => {
                        let child_path = entry.path();
                        if should_include(&child_path) {
                            node.children.push(build_shallow_node(&child_path)?);
                        }
                    }
                    Err(_) => {
                        // Skip entries that can't be read
                        // Could add logging here if needed
                    }
                }
            }
        }
        Err(_) => {
            // Directory exists but content can't be read (likely permission denied)
            // We continue with an empty children list
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

fn build_shallow_node(path: &Path) -> Result<DirectoryNode, String> {
    let metadata = fs::metadata(path).map_err(|e| e.to_string())?;

    Ok(DirectoryNode {
        name: if path.parent().is_none() {
            path.to_str()
                .and_then(|s| s.get(0..2))
                .unwrap_or("root")
                .trim_end_matches("\\")
                .to_string()
        } else {
            path.file_name()
                .and_then(|n| n.to_str())
                .unwrap_or("unknown")
                .to_string()
        },
        path: path.to_path_buf(),
        is_file: metadata.is_file(),
        children: Vec::new(),
        metadata: extract_metadata(path)?,
        has_children: metadata.is_dir() && has_any_child(path),
        loaded: false,
    })
}

fn has_any_child(path: &Path) -> bool {
    fs::read_dir(path)
        .map(|mut entries| entries.next().is_some())
        .unwrap_or(false)
}

fn should_include(path: &Path) -> bool {
    // Bỏ qua các thư mục ẩn và hệ thống
    if let Ok(metadata) = fs::metadata(path) {
        let attrs = metadata.file_attributes();
        !(attrs & 0x2 != 0 || attrs & 0x4 != 0) // Hidden or System
    } else {
        false
    }
}
