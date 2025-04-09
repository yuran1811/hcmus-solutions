#[derive(Debug, Clone)]
pub struct PartitionRawInfo {
    pub is_bootable: bool,
    pub partition_type: u8,
    pub start_lba: u32,
    pub total_sectors: u32,

    pub raw_type: String,
    pub raw_label: String,

    pub size: u64,
    pub part_label: String,
    pub part_type: String,
}

impl std::default::Default for PartitionRawInfo {
    fn default() -> Self {
        Self {
            is_bootable: false,
            partition_type: 0,
            start_lba: 0,
            total_sectors: 0,
            raw_type: String::new(),
            raw_label: String::new(),
            size: 0,
            part_label: String::new(),
            part_type: String::new(),
        }
    }
}

impl std::fmt::Display for PartitionRawInfo {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(
            f,
            "({})\n| Boot: {}\n| Type: 0x{:02X} ({})\n| Start: {}\n| Size (sectors): {}",
            self.part_label,
            if self.is_bootable { "Yes" } else { "No" },
            self.partition_type,
            self.part_type,
            self.start_lba,
            self.total_sectors
        )
    }
}

#[derive(Debug, serde::Serialize)]
pub struct PartitionInfo {
    pub drive_letter: String,
    pub label: String,
    pub fs_type: String,
    pub total_size: u64,
    pub free_space: u64,
}

impl std::fmt::Display for PartitionInfo {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(
            f,
            "{} ({}) - {}\n| Size: {} bytes\n| Free: {} bytes",
            self.label, self.drive_letter, self.fs_type, self.total_size, self.free_space
        )
    }
}
