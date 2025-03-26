<script setup lang="ts">
import { DockItems } from '@/constants';

const dockItemSize = 48;

const dockItems = ref<NodeListOf<HTMLImageElement> | null>(null);

onMounted(() => {
  dockItems.value = document.querySelectorAll(
    '.dock-item',
  ) as NodeListOf<HTMLImageElement>;
});

const mouseMoveHandler = (e: MouseEvent) => {
  if (!dockItems.value) return;

  dockItems.value.forEach((element: HTMLImageElement) => {
    const { left, width } = element.getBoundingClientRect();
    const leftAlign = left + width / 2;

    const maxDistance = 280;
    const distance = Math.abs(e.clientX - leftAlign);
    const newSize =
      distance < maxDistance
        ? dockItemSize + (maxDistance - distance) * 0.16
        : dockItemSize;

    element.style.width = `${newSize}px`;
  });
};

const mouseLeaveHandler = (_: MouseEvent) => {
  if (!dockItems.value) return;

  dockItems.value.forEach((element: HTMLImageElement) => {
    element.style.width = `${dockItemSize}px`;
  });
};

const clickHandler = (name: string) => {
  const item = DockItems.find((i) => i.name === name);

  if (item?.type === 'link') {
    window.open(item.link);
  }
};
</script>

<template>
  <div
    class="dock"
    @mousemove="mouseMoveHandler"
    @mouseleave="mouseLeaveHandler"
  >
    <div
      v-for="item in DockItems"
      :key="item.name"
      :data-tooltip="item.name"
      :class="{
        opened: false,
      }"
    >
      <img
        class="dock-item"
        :src="item.icon"
        @click="clickHandler(item.name)"
      />
    </div>
  </div>
</template>
