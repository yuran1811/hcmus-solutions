<script setup lang="ts">
definePageMeta({
  name: 'Components Testing',
  icon: 'lucide:test-tube-diagonal',
  description: 'Components Testing for usabilities and functionalities.',
});
</script>

<template>
  <LayoutDefault>
    <section class="space-y-12 pb-24">
      <!-- <div class="pattern-container" /> -->

      <!-- <CursorFluid /> -->

      <GlowGrid />

      <ParallaxCard />

      <div
        class="xs:grid-cols-2 container mx-auto grid max-w-md grid-cols-1 items-center justify-items-center gap-12"
      >
        <Card />
        <Card />
        <Card />
        <Card />
      </div>
    </section>

    <Dock />
  </LayoutDefault>
</template>
