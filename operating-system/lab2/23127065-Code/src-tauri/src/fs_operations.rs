use std::{cmp::Ordering, fs, os::windows::fs::MetadataExt, path::Path};

use crate::{
    models::fs::{DirectoryNode, FileMetadata},
    utils::base::{format_attributes, system_time_to_local_strings},
};

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
    // Ignore system or hidden files
    if let Ok(metadata) = fs::metadata(path) {
        let attrs = metadata.file_attributes();
        !(attrs & 0x2 != 0 || attrs & 0x4 != 0) // Hidden or System
    } else {
        false
    }
}
