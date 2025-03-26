declare interface PartitionStoreState {
  currentPartition: { node: DirectoryNode | null; info: PartitionInfo | null };
  selectedFile: DirectoryNode | null;
}

declare interface PartitionStoreActions {
  setCurrentPartition: (
    partition: PartitionStoreState['currentPartition']['info'],
    node: PartitionStoreState['currentPartition']['node'],
  ) => void;

  setSelectedFile: (file: PartitionStoreState['selectedFile']) => void;
}

declare interface DockItemType {
  name: string;
  icon: string;
  type: 'iframe' | 'component' | 'link';
  iframe?: {
    url: string;
    width: number;
    height: number;
  };
  position?: {
    top: string;
    left: string;
  };
  link?: string;
  component?: Component;
  color?: string;
  background?: string;
}

declare interface TOC {
  depth: number;
  id: string;
  text: string;
  children?: TOC[];
}
