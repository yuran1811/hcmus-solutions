<script setup lang="ts">
defineProps<{
  title: string;
  description: string;
}>();
</script>

<template>
  <div class="grid grid-cols-1 lg:grid-cols-2">
    <div class="px-6 pt-12 pb-10 sm:pt-16 lg:px-8 lg:pt-22">
      <div class="mx-auto max-w-xl lg:mx-0 lg:max-w-lg">
        <h2 class="text-3xl font-bold tracking-tight">
          {{ title }}
        </h2>
        <p class="mt-6 text-lg leading-8 text-(--ui-text-muted)">
          {{ description }}
        </p>
      </div>
    </div>
    <div
      class="container mx-auto max-w-xl px-6 pt-2 pb-10 sm:pt-8 lg:px-8 lg:pt-22"
    >
      <slot />
    </div>
  </div>
</template>
