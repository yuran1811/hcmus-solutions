declare interface ModuleOptions {
  prefix: false | string;
}

declare interface PartitionInfo {
  drive_letter: string;
  label: string;
  fs_type: string;
  total_size: number;
  free_space: number;
}

declare interface FileMetadata {
  name: string;
  attributes: string;
  created_date: string;
  created_time: string;
  size: number;
}

declare interface DirectoryNode {
  name: string;
  path: string;
  is_file: boolean;
  children: DirectoryNode[];
  metadata: FileMetadata;
  has_children: boolean;
  loaded: boolean;
}
