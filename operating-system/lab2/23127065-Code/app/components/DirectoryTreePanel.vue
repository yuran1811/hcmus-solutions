<script setup lang="ts">
defineProps<{ tree: DirectoryNode }>();
</script>

<template>
  <div class="overflow-auto rounded-lg p-4 shadow">
    <h3 class="mb-4 text-lg font-semibold">Directory Structure</h3>
    <DirectoryTreeNode
      :node="tree"
      :depth="0"
      @select="$emit('select', $event)"
    />
  </div>
</template>
