<script setup lang="ts">
defineProps<{ fixed?: boolean }>();
</script>

<template>
  <div>
    <SiteNavbar
      :class="fixed ? 'fixed w-full' : 'sticky bg-(--ui-bg)/75 backdrop-blur'"
    />
    <SiteSidebar />

    <slot />
  </div>
</template>
