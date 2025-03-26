<script setup lang="ts">
definePageMeta({
  layout: 'home',
  description: 'yupartify Homepage',
});

const { app } = useAppConfig();
</script>

<template>
  <UContainer class="relative h-screen overflow-hidden">
    <div class="grid size-full place-content-center gap-y-8">
      <SvgoLogo
        :filled="true"
        :font-controlled="false"
        class="mx-auto size-16 sm:size-24"
      />

      <div class="flex flex-col items-center gap-y-3">
        <h1
          class="font-heading animate-pulse text-2xl font-bold text-pretty sm:text-3xl md:mb-5"
        >
          {{ app.name.toUpperCase() }}
        </h1>
      </div>

      <div class="flex justify-center">
        <UButton :to="app.repo"> Star on GitHub </UButton>
      </div>
    </div>

    <div class="absolute-center-h fixed bottom-6 text-sm">
      <div class="flex items-center gap-1 text-(--ui-text-muted)">
        <p class="text-sm">Made by</p>
        <ULink :to="app.repo" external target="_blank">
          {{ app.author }}
        </ULink>
      </div>
    </div>
  </UContainer>
</template>
