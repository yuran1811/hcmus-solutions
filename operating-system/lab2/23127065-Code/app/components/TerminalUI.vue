<script setup lang="ts">
defineProps<{
  code?: number;
  command?: string;
  output?: string;
  error?: string;
  hideTitle?: boolean;
}>();

const {
  app: { name },
} = useAppConfig();
</script>

<template>
  <div class="w-full">
    <div
      class="overflow-hidden rounded-lg bg-(--ui-bg-elevated) px-5 py-4 font-mono text-sm leading-normal text-(--ui-text) subpixel-antialiased shadow-lg"
    >
      <div v-if="!hideTitle" class="top mb-4 flex">
        <div class="h-3 w-3 rounded-full bg-red-400"></div>
        <div class="ml-2 h-3 w-3 rounded-full bg-orange-300"></div>
        <div class="ml-2 h-3 w-3 rounded-full bg-green-400"></div>
      </div>

      <div class="flex">
        <span class="text-(--ui-primary)">{{ name }} $</span>
        <p class="typing flex-1 items-center pl-2">
          {{ command }}
          <br />
        </p>
      </div>
      <div v-if="command" class="flex">
        <span class="text-(--ui-primary)">{{ name }} $</span>
        <p class="typing flex-1 items-center pl-2">
          {{ output || error }}
          <br />
        </p>
      </div>
    </div>
  </div>
</template>
