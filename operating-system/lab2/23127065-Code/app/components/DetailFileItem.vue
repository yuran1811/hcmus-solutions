<script setup lang="ts">
defineProps({
  label: String,
  value: [String, Number],
});
</script>

<template>
  <div class="detail-item grid grid-cols-3 gap-4">
    <span class="font-medium capitalize">{{ label }}</span>
    <span class="col-span-2 text-sm">{{ value }}</span>
  </div>
</template>
