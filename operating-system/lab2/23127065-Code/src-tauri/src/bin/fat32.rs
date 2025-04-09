use std::{
    cmp::Ordering,
    io::{self, Write},
};

use yupartify_lib::{
    models::{
        fat_fs::{FAT32FileSystem, FAT32},
        fs::{FileSystem, Result},
    },
    utils::{
        base::{
            clear_current_line, disable_raw_mode, enable_raw_mode, is_text_file, print_shell_help,
            print_shell_info,
        },
        data::{parser::parse_file_content, reader::read_mbr_disk},
        list_partitions::list_fat32_paritions,
    },
};

#[allow(unused_variables)]
fn read_input(hist: &Vec<String>, prompt: &str) -> String {
    let stdin = io::stdin();
    let mut input = String::new();

    stdin.read_line(&mut input).unwrap();

    input
}

fn main() -> Result<()> {
    println!("[info] - Scanning disk images to find FAT32 partitions...");
    let fat32_partitions = list_fat32_paritions();
    if fat32_partitions.len() == 0 {
        eprintln!("[err] - No FAT32 partitions found.");
        return Ok(());
    }

    // Display the list of FAT32 partitions found
    println!("+ Found {} FAT32 partitions:", fat32_partitions.len());
    for (i, (part, _)) in fat32_partitions.iter().enumerate() {
        println!(
            "  | [{}] - {} (size: {} bytes)",
            i, part.raw_label, part.size
        );
    }
    println!(
        "> Select a partition to open (0-{}):",
        fat32_partitions.len() - 1
    );

    // Read user input for partition selection
    let mut input = String::new();
    let mut part_idx;
    loop {
        io::stdin().read_line(&mut input).unwrap();
        part_idx = input.trim().parse().unwrap_or(-1);

        if 0 <= part_idx && part_idx < fat32_partitions.len() as i32 {
            break;
        } else {
            println!("[err] - Invalid partition selected. Try again.");
        }
    }
    let selected_part = fat32_partitions[part_idx as usize].clone();
    println!(
        "[info] - Opening partition: {} (size: {} bytes)",
        selected_part.0.raw_label, selected_part.0.size
    );

    // Read the MBR of the selected partition
    let raw_part_info = read_mbr_disk(&selected_part.1, Some(part_idx as usize))?;
    let mut fat = FAT32::open(r"\\.\F:", Some(raw_part_info[0].clone()))?;
    print_shell_info();

    // Main loop for shell commands
    let mut hist: Vec<String> = Vec::new();
    enable_raw_mode();
    loop {
        clear_current_line();

        let prompt = format!("\n{} $ ", fat.current_path());
        print!("{}", prompt);
        io::stdout().flush().unwrap();

        let input = read_input(&hist, &prompt);
        let trimmed = input.trim();
        if trimmed.is_empty() {
            continue;
        }

        hist.push(trimmed.to_string());
        if hist.len() > 10 {
            hist.remove(0);
        }

        if trimmed == "exit" {
            break;
        } else if trimmed == "help" {
            print_shell_help();
        } else if trimmed == "clear" || trimmed == "cls" {
            print!("\x1B[2J\x1B[1;1H");
        } else if trimmed == "info" {
            print_shell_info();
        } else if trimmed == "pinfo" {
            fat.print_info();
        } else if trimmed == "pwd" {
            println!("\"{}\"", fat.current_path());
        } else if trimmed == "ls" {
            match fat.list_dir(&fat.current_path()) {
                Ok(mut entries) => {
                    entries.sort_by(|a, b| {
                        match (a.is_directory, b.is_directory) {
                            (true, false) => Ordering::Less, // a is a folder, b is not → a comes first
                            (false, true) => Ordering::Greater, // b is a folder, a is not → b comes first
                            _ => a.name.cmp(&b.name),           // both are same type: sort by name
                        }
                    });
                    for entry in entries {
                        println!(
                            "{}{}",
                            entry.name,
                            if entry.is_directory { "/" } else { "" }
                        );
                    }
                }
                Err(e) => println!("[error] - {}", e),
            }
        } else if trimmed.starts_with("cd") {
            if let Some(dir) = trimmed.split_whitespace().nth(1) {
                match fat.cd(&dir) {
                    Ok(_) => {}
                    Err(e) => println!("[error] - {}", e),
                }
            } else {
                println!("Usage: cd <directory>");
            }
        } else if trimmed.starts_with("cat") {
            if let Some((_first, rest)) = trimmed.split_once(' ') {
                let filename = rest.trim();
                let filepath = format!("{}\\{}", fat.current_path(), filename);
                match is_text_file(&filepath) {
                    Ok(is_text) => {
                        if !is_text {
                            println!("[error] - {} is not a text file", filename);
                            continue;
                        }

                        match fat.read_file(&filepath, Some(is_text)) {
                            Ok(data) => {
                                println!("{}", parse_file_content(&data.0[..50.min(data.0.len())]));
                            }
                            Err(e) => {
                                println!("[error] - {}", e)
                            }
                        }
                    }
                    Err(e) => {
                        println!("[error] - {}", e);
                        continue;
                    }
                }
            } else {
                println!("Usage: cat <filename>");
            }
        } else if trimmed.starts_with("file") {
            if let Some((_first, rest)) = trimmed.split_once(' ') {
                let filename = rest.trim();
                let filepath = format!("{}\\{}", fat.current_path(), filename);
                println!("Fetching: {} (from {})", filename, filepath);

                match fat.fetch_file(&filepath) {
                    Ok(file_info) => {
                        println!("{}", file_info);
                    }
                    Err(e) => println!("[error] - {}", e),
                }
            } else {
                println!("Usage: file <filename>");
            }
        } else {
            println!("command not found");
        }
    }
    disable_raw_mode();

    Ok(())
}
