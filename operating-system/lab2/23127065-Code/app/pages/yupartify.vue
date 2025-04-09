<script setup lang="ts">
import { invoke } from '@tauri-apps/api/core';

definePageMeta({
  name: 'yupartify',
  icon: 'lucide:package',
  description: 'Disk partition and file explorer',
  category: 'system',
});

const partitionStore = usePartitionStore();

const partitions = ref<PartitionInfo[]>([]);
const fileContent = ref('');

onMounted(async () => {
  partitions.value = await invoke('list_partitions');
});

const handlePartitionSelect = async (partition: PartitionInfo) => {
  if (!partition) {
    fileContent.value = '';
    return;
  }

  try {
    await invoke('update_working_drive', {
      drive: partition.drive_letter,
    });
    const currentTree: DirectoryNode = await invoke('get_children', {
      path: `${partition.drive_letter ?? ''}`,
    });

    partitionStore.setCurrentPartition(partition, currentTree);
  } catch {
    partitionStore.setCurrentPartition(partition, null);
  } finally {
    partitionStore.setSelectedFile(null);
  }
};

const handleFileSelect = async (file: DirectoryNode) => {
  partitionStore.setSelectedFile(file);

  if (file && file?.is_file) {
    try {
      fileContent.value = await invoke('read_text_file', {
        path: file?.path,
      });
    } catch (e) {
      fileContent.value = `Error reading file: ${e}`;
    }
  }
};
</script>

<template>
  <LayoutDefault>
    <div
      class="grid min-h-[calc(100dvh-16rem)] grid-cols-1 items-start sm:grid-cols-2 md:grid-cols-3 lg:grid-cols-4"
    >
      <PartitionSidebar
        :partitions="partitions"
        :current-partition="partitionStore.currentPartition"
        class="col-span-full sm:col-span-1"
        @select="handlePartitionSelect"
      />
      <main class="col-span-full sm:col-span-1 md:col-span-2 lg:col-span-3">
        <DirectoryTreePanel
          v-if="partitionStore.currentPartition.node"
          :tree="partitionStore.currentPartition.node"
          @select="handleFileSelect"
        />
        <FileDetailPanel
          v-if="partitionStore.selectedFile"
          :file="partitionStore.selectedFile"
          :content="fileContent"
          class="mt-4"
        />
      </main>
    </div>
  </LayoutDefault>
</template>
