<script setup lang="ts">
const { tabs } = defineProps<{ tabs?: string[] }>();

const uiTabs = computed(() => tabs?.map((tab) => ({ label: tab })));
</script>

<template>
  <UTabs
    :items="uiTabs"
    orientation="horizontal"
    class="[&_button]:cursor-pointer"
  >
    <template #content="{ index }">
      <slot :name="`tab${index + 1}`" />
    </template>
  </UTabs>
</template>
