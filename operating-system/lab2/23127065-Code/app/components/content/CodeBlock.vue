<script setup lang="ts">
const copied = ref(false);
const codeRef = useTemplateRef('code');
const { copy } = useClipboard();

const copyCode = () => {
  copy(codeRef.value?.querySelector('pre')?.textContent || '');

  copied.value = true;
  setTimeout(() => {
    copied.value = false;
  }, 1500);
};
</script>

<template>
  <div ref="code" class="group relative">
    <slot
      class="overflow-x-auto rounded-lg bg-(--ui-bg-accented)/30 font-mono text-sm"
    />

    <UButton
      class="group absolute top-0 right-0 flex items-center gap-1.5 rounded-md bg-transparent px-3 py-1.5 text-sm font-semibold hover:bg-transparent"
      :aria-label="copied ? 'Copied!' : 'Copy'"
      @click="copyCode"
    >
      <Transition name="fade" mode="out-in">
        <span
          v-if="copied"
          key="copied"
          class="flex items-center text-green-500"
        >
          <Icon name="lucide:check" class="mr-2 size-4" />
          Copied!
        </span>
        <span v-else key="copy" class="flex items-center text-(--ui-text)">
          <Icon name="lucide:clipboard" class="mr-2 size-4" />
          Copy
        </span>
      </Transition>
    </UButton>
  </div>
</template>
