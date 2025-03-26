<script setup lang="ts">
definePageMeta({
  name: 'Components Testing',
  icon: 'lucide:test-tube-diagonal',
  description: 'Components Testing for usabilities and functionalities.',
});
</script>

<template>
  <LayoutDefault>
    <!-- <div class="pattern-container" /> -->
    <!-- <CursorFluid /> -->

    <ParallaxCard />

    <div
      class="xs:grid-cols-2 container mx-auto grid max-w-md grid-cols-1 items-center justify-items-center gap-12 pb-32"
    >
      <Card />
      <Card />
      <Card />
      <Card />
    </div>

    <Dock />
  </LayoutDefault>
</template>
