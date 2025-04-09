use std::{
    fs::File,
    io::{self, Read, Write},
    process::Command,
    time::SystemTime,
};

use chrono::{DateTime, Local};

use crate::models::{
    fat_fs::{FATEntry, FATTable},
    fs::FileMetadata,
};

pub fn enable_raw_mode() {
    let _ = Command::new("stty").arg("raw").arg("-echo").output();
}

pub fn disable_raw_mode() {
    let _ = Command::new("stty").arg("sane").output();
}

pub fn clear_current_line() {
    print!("\r\x1B[K");
    std::io::stdout().flush().unwrap();
}

pub fn print_shell_info() {
    println!("Welcome to the FAT32 File System Explorer!");
    println!("Type 'help' for a list of commands.");
    println!("Type 'exit' to quit the program.");
    println!("Press F2 at any time to display the command history.");
}

pub fn print_shell_help() {
    println!("Available commands:\n|  ls: list all files in current dir\n|  cat <filename>: display text file content\n|  cd <directory>: go to dir\n|  file <filename>: display file info\n|  pwd: print working dir\n|  help: display help\n|  clear, cls: Clear the screen\n|  exit: Exit the program");
}

pub fn is_text_file(path: &str) -> io::Result<bool> {
    let mut file = File::open(path)?;
    let mut buffer = [0; 16]; // Read the first 16 bytes for more reliable detection

    let bytes_read = file.read(&mut buffer)?;
    if bytes_read >= 4 {
        // PNG
        if &buffer[..8] == b"\x89PNG\r\n\x1a\n" {
            println!("PNG file detected");
            return Ok(false);
        }
        // GIF
        if &buffer[..4] == b"GIF8" {
            println!("GIF file detected");
            return Ok(false);
        }
        // PDF
        if &buffer[..4] == b"%PDF" {
            println!("PDF file detected");
            return Ok(false);
        }
        // RAR
        if &buffer[..7] == b"Rar!\x1A\x07\x00" {
            println!("RAR file detected");
            return Ok(false);
        }
        // 7z
        if &buffer[..6] == b"7z\xBC\xAF\x27\x1C" {
            println!("7z file detected");
            return Ok(false);
        }
        // ZIP
        if &buffer[..4] == b"PK\x03\x04" {
            println!("ZIP file detected");
            return Ok(false);
        }
        // TAR
        if &buffer[..5] == b"ustar" {
            println!("TAR file detected");
            return Ok(false);
        }
        // WebP
        if &buffer[..4] == b"RIFF" && &buffer[8..12] == b"WEBP" {
            println!("WebP file detected");
            return Ok(false);
        }
        // MP4, MOV
        if &buffer[..4] == b"\x00\x00\x00\x20" && &buffer[4..8] == b"ftyp" {
            println!("MP4, MOV file detected");
            return Ok(false);
        }
        // FLV
        if &buffer[..4] == b"FLV\x01" {
            println!("FLV file detected");
            return Ok(false);
        }
        // AVI
        if &buffer[..4] == b"RIFF" && &buffer[8..12] == b"AVI " {
            println!("AVI file detected");
            return Ok(false);
        }
        // MKV
        if &buffer[..4] == b"EBML" {
            println!("MKV file detected");
            return Ok(false);
        }
        // OGG
        if &buffer[..4] == b"fLaC" {
            println!("OGG, FLAC file detected");
            return Ok(false);
        }
        // WAVE
        if &buffer[..4] == b"RIFF" && &buffer[8..12] == b"WAVE" {
            println!("WAVE file detected");
            return Ok(false);
        }
        // Unix elf
        if &buffer[..4] == b".ELF" {
            println!("Unix ELF file detected");
            return Ok(false);
        }
    } else if bytes_read >= 3 {
        // MP3
        if &buffer[..3] == b"ID3" {
            println!("MP3 file detected");
            return Ok(false);
        }
    } else if bytes_read >= 2 {
        // JPEG, JPG
        if &buffer[..2] == b"\xFF\xD8" {
            println!("JPEG file detected");
            return Ok(false);
        }
        // Executable, DLL (Windows)
        if &buffer[..2] == b"MZ" {
            println!("Executable file (DLL) detected");
            return Ok(false);
        }
        // BMP
        if &buffer[..2] == b"BM" {
            println!("BMP file detected");
            return Ok(false);
        }
        // ICO
        if &buffer[..2] == b"\x00\x00" && &buffer[2..4] == b"\x01\x00" {
            println!("ICO file detected");
            return Ok(false);
        }
    }

    let mut is_text = true;
    for &byte in &buffer[..bytes_read] {
        // Check if the byte is a printable ASCII character
        if !(0x20..=0x7E).contains(&byte) && byte != 0x0A && byte != 0x0D {
            is_text = false;
            break;
        }
    }

    Ok(is_text)
}

pub fn calc_fat32_used_space(
    fats: &Vec<FATTable>,
    sectors_per_cluster: u64,
    bytes_per_sector: u64,
) -> u64 {
    let mut used_space = 0;

    // Iterate over the FAT entries
    for i in 0..fats[0].fat.len() {
        if fats[0].is_in_bounds(fats[0].fat[i]) {
            used_space += sectors_per_cluster * bytes_per_sector;
        }
    }

    used_space
}

pub fn format_attributes(attrs: u32) -> String {
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

pub fn system_time_to_local_strings(time: SystemTime) -> (String, String) {
    let datetime: DateTime<Local> = time.into();
    (
        datetime.format("%Y-%m-%d").to_string(),
        datetime.format("%H:%M:%S").to_string(),
    )
}

pub fn get_fat32_file_metadata(file: FATEntry) -> Result<FileMetadata, String> {
    let (created_date, created_time) = system_time_to_local_strings(file.created);

    Ok(FileMetadata {
        name: file.path,
        attributes: format_attributes(file.attributes.0.into()),
        created_date,
        created_time,
        size: if !file.is_directory {
            file.size.into()
        } else {
            0
        },
    })
}
