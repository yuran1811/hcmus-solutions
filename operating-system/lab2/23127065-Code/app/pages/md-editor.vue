<script setup lang="ts">
import { DefaultMDContent } from '@/constants';

definePageMeta({
  name: 'Markdown Editor',
  icon: 'lucide:pen-line',
  description: 'A simple markdown editor using @nuxt/content',
});

const input = ref<string>(DefaultMDContent);
const output = ref<string>(DefaultMDContent);

const debouncedFn = useDebounceFn(() => {
  output.value = input.value;
}, 500);
</script>

<template>
  <div class="w-full">
    <h1 class="pb-6 text-center text-2xl font-bold">Markdown Editor</h1>

    <div class="xs:grid-cols-2 grid h-max min-h-[calc(100dvh-11rem)] *:p-2">
      <textarea
        v-model="input"
        class="xs:h-full h-72 resize-none bg-(--ui-bg-muted) outline-none"
        @input="debouncedFn"
      />
      <MDC class="prose prose-invert container mx-auto" :value="output" />
    </div>
  </div>
</template>
