use yupartify_lib::utils::{
    data::reader::read_mbr_disk,
    list_partitions::{list_fat32_paritions_by_letter, list_windows_disks},
};

fn main() {
    #[cfg(target_os = "linux")]
    list_linux_partitions();

    #[cfg(target_os = "windows")]
    list_windows_disks();

    #[cfg(target_os = "macos")]
    list_mac_partitions();

    let path = r"\\.\PhysicalDrive1";
    match read_mbr_disk(path, None) {
        Ok(partitions) => {
            for (i, partition) in partitions.iter().enumerate() {
                println!("+  Partition {} {}", i + 1, partition);
            }
        }
        Err(e) => {
            eprintln!("[err] - An error occurs while reading disk: {}", e)
        }
    }

    println!("\n> List FAT32 partitions by letter");
    let parts = list_fat32_paritions_by_letter();
    for part in parts.iter() {
        println!("+ {}", part);
    }
}
