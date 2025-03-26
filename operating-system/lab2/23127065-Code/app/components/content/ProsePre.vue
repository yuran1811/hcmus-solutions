<script setup lang="ts">
const props = defineProps({
  code: {
    type: String,
    default: '',
  },
  language: {
    type: String,
    default: null,
  },
  filename: {
    type: String,
    default: null,
  },
  highlights: {
    type: Array as () => number[],
    default: () => [],
  },
  meta: {
    type: String,
    default: null,
  },
  class: {
    type: String,
    default: null,
  },
});

const breadCrumbItems = computed(() =>
  props.filename
    ? props.filename.split('/').map((item) => ({
        label: item.trim(),
      }))
    : [],
);
</script>

<template>
  <CodeBlock>
    <div class="[&_ol]:mb-0 [&_ol]:gap-0 [&_ol]:ps-2">
      <div
        v-if="filename"
        class="rounded-t-md bg-(--ui-bg-muted) font-mono text-sm text-(--ui-text)"
      >
        <UBreadcrumb :items="breadCrumbItems">
          <template #item-label="{ item, active }">
            <span
              :class="{
                'text-(--ui-primary)': active,
                'text-(--ui-text)': !active,
              }"
              >{{ item.label }}</span
            >
          </template>
          <template #separator>
            <span class="text-(--ui-text)">/</span>
          </template>
        </UBreadcrumb>
      </div>

      <pre
        class="!mt-0"
        :class="{
          [props.class as string]: true,
          'rounded-t-none': props.filename,
        }"
      ><slot /></pre>
    </div>
  </CodeBlock>
</template>

<style scoped>
pre code .line {
  display: block;
}
</style>
