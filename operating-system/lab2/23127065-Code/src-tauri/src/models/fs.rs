use std::{io::Error as IOError, path::PathBuf, string::FromUtf16Error, time::SystemTime};

#[derive(Debug)]
pub struct FileInfo {
    pub name: String,
    pub attributes: u8,
    pub created: SystemTime,
    pub modified: SystemTime,
    pub size: u64,
    pub is_directory: bool,
}

impl std::fmt::Display for FileInfo {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(
            f,
            "{}'{}' (size: {}) (created:{}, modified:{})",
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

#[derive(Debug)]
pub enum FsError {
    Io(IOError),
    ParseError(&'static str),
    Utf16Error,
    NotFound,
    InvalidStructure,
}

impl From<IOError> for FsError {
    fn from(e: IOError) -> Self {
        Self::Io(e)
    }
}

impl From<&'static str> for FsError {
    fn from(e: &'static str) -> Self {
        Self::ParseError(e)
    }
}

impl From<FromUtf16Error> for FsError {
    fn from(_: FromUtf16Error) -> Self {
        Self::Utf16Error
    }
}

impl From<()> for FsError {
    fn from(_: ()) -> Self {
        Self::NotFound
    }
}

impl std::fmt::Display for FsError {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        match self {
            FsError::Io(e) => write!(f, "IO Error: {}", e),
            FsError::ParseError(s) => write!(f, "Parse Error: {}", s),
            FsError::Utf16Error => write!(f, "UTF-16 Decoding Error"),
            FsError::NotFound => write!(f, "File/Directory not found"),
            FsError::InvalidStructure => write!(f, "Invalid filesystem structure"),
        }
    }
}

pub type Result<T> = std::result::Result<T, FsError>;

pub trait FileSystem {
    fn current_path(&self) -> String;
}

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
