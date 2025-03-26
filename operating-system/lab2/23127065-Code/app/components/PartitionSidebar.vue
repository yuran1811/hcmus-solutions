<script setup lang="ts">
defineProps<{
  partitions: any[];
  currentPartition?: PartitionStoreState['currentPartition'];
}>();
</script>

<template>
  <div class="p-4">
    <h2 class="mb-4 text-lg font-bold">Disk Partitions</h2>
    <div class="xs:grid-cols-2 container grid grid-cols-1 gap-4 sm:grid-cols-1">
      <div
        v-for="part in partitions"
        :key="part.drive_letter"
        :class="`partition-item ${currentPartition?.info?.drive_letter === part.drive_letter ? 'active' : ''}`"
        @click="$emit('select', part)"
      >
        <div class="flex items-center gap-2">
          <span
            class="partition-item-info flex-center gap-2 text-(--ui-text-dimmed) transition-colors"
          >
            <IconDisk class="size-8" />
            <span
              class="inline-flex flex-col items-start justify-center font-semibold"
            >
              <span class="text-sm font-bold">
                {{ formatParitionLabel(part) }}</span
              >
              <span
                class="partition-item-type text-xs text-(--ui-text-muted) transition-colors"
              >
                {{ part.fs_type }}
              </span>
            </span>
          </span>
        </div>
        <div class="mt-1 text-xs font-semibold">
          <span class="text-(--ui-secondary)"
            >{{ formatByteSize(part.free_space) }} free</span
          >
          •
          <span class="text-(--ui-text)">{{
            formatByteSize(part.total_size)
          }}</span>
        </div>
      </div>
    </div>
  </div>
</template>
