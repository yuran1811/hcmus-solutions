export const usePartitionStore = defineStore<
  string,
  PartitionStoreState,
  any,
  PartitionStoreActions
>('partition_store', {
  state: () => ({
    currentPartition: {
      node: null,
      info: null,
    },
    selectedFile: null,
  }),
  actions: {
    setCurrentPartition(partition, node) {
      this.currentPartition.info = partition;
      this.currentPartition.node = node;
    },

    setSelectedFile(file) {
      this.selectedFile = file;
    },
  },
});
