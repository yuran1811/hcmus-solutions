<script setup lang="ts">
import { DefaultDateFormat } from '@/constants';

const props = defineProps<{
  file: PartitionStoreState['selectedFile'];
  content: string;
}>();

const createdDateTime = computed(() => {
  if (!props.file) return '';

  return useDateFormat(
    `${props.file.metadata.created_date || ''} ${props.file.metadata.created_time}`,
    DefaultDateFormat,
  ).value;
});

const formattedSize = computed(() => {
  if (!props.file) return '';

  return formatByteSize(props.file.metadata.size);
});
</script>

<template>
  <div class="file-details container rounded-lg p-4 shadow">
    <h3 class="mb-4 text-lg font-semibold">File Properties</h3>

    <div v-if="file" class="space-y-2">
      <DetailFileItem label="Name" :value="file.metadata.name" />
      <DetailFileItem label="Path" :value="file.path" />
      <DetailFileItem label="Attributes" :value="file.metadata.attributes" />
      <DetailFileItem label="Created" :value="createdDateTime" />
      <DetailFileItem label="Size" :value="formattedSize" />
    </div>

    <div v-if="content" class="mt-6">
      <h4 class="mb-2 font-medium">Content Preview</h4>
      <div class="max-h-96 overflow-auto">
        <ProsePre class="w-full rounded p-4 text-sm">
          {{ content }}
        </ProsePre>
      </div>
    </div>
  </div>
</template>
