<script setup lang="ts">
import { loadIcon } from '@iconify/vue';
import { invoke } from '@tauri-apps/api/core';

const props = defineProps<{
  node: DirectoryNode;
  depth: number;
}>();

const emit = defineEmits(['select']);

const isOpen = ref(false);
const isLoading = ref(false);
const error = ref<any>(null);

const fileIcon = computedAsync(async () => {
  const ext = getFileIcon(props.node);
  let icon = `catppuccin:${ext}`;
  try {
    const iconData = await loadIcon(icon);
    if (!iconData || !ext) icon = 'catppuccin:file';
  } catch {
    icon = 'catppuccin:file';
  }

  return icon;
}, 'catppuccin:file');

const folderIcon = computedAsync(async () => {
  const ext = getFolderIcon(props.node);
  let icon = `catppuccin:folder-${ext}`;
  try {
    const iconData = await loadIcon(icon);
    if (!iconData || !ext) icon = 'catppuccin:folder';
  } catch {
    icon = 'catppuccin:folder';
  }

  return icon;
}, 'catppuccin:folder');

const hasChildren = computed(
  () => props.node?.has_children || props.node?.children?.length > 0,
);

const nodeInfoItems = computed(() => {
  if (!props.node) return [];
  if (error.value) return [{ label: 'Error', value: error.value.message }];

  return getNodeInfoItems(props.node);
});

const toggle = async () => {
  if (props.node?.is_file) {
    emit('select', props.node);
    return;
  }

  try {
    isLoading.value = true;

    const node = await invoke('get_children', {
      path: props.node?.path,
    });

    // @ts-ignore
    Object.assign(props.node?.children, node?.children ?? []);

    error.value = null;
  } catch (e) {
    error.value = e;
  } finally {
    isLoading.value = false;
  }

  isOpen.value = !isOpen.value;
};
</script>

<template>
  <div class="tree-node w-max">
    <UContextMenu :items="nodeInfoItems" :ui="{ content: 'w-48' }">
      <div
        class="node-item hover:bg flex cursor-pointer items-center gap-2 py-1"
        :style="{ paddingLeft: `${(depth || 0) * 20 + 10}px` }"
        @click="toggle"
      >
        <span class="flex-center inline-flex">
          <Icon v-if="error" name="lucide:badge-alert" />
          <Icon v-if="!!node?.is_file" :name="fileIcon" />

          <Icon
            v-show="!node?.is_file && hasChildren && !isOpen"
            :name="folderIcon"
          />
          <Icon
            v-show="!node?.is_file && hasChildren && isOpen"
            :name="`${folderIcon}-open`"
          />
        </span>

        <p class="line-clamp-1 max-w-full text-ellipsis text-(--ui-text)">
          {{ node?.name || '' }}
        </p>
        <Icon v-show="isLoading" name="lucide:loader" />
      </div>
    </UContextMenu>

    <div
      v-show="isOpen && node?.children"
      class="children scrollbar-hidden max-h-96 max-w-full overflow-y-auto"
    >
      <DirectoryTreeNode
        v-for="child in node?.children || []"
        :key="child.path"
        v-memo="[child.path]"
        :node="child"
        :depth="(depth || 0) + 1"
        @select="$emit('select', $event)"
      />
    </div>
  </div>
</template>
