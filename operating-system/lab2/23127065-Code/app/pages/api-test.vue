<script setup lang="ts">
definePageMeta({
  name: 'API Testing',
  icon: 'lucide:puzzle',
  description: 'API Testing for usabilities and functionalities.',
  category: 'system',
});

const { data: latestPostsData } = await useFetch('/api/latest-posts?limit=2');
const { data: helloData } = await useFetch('/hello', { method: 'POST' });

const items = ref<any[]>([
  {
    label: 'GET /api/latest-post?limit=2',
    content: latestPostsData,
  },
  {
    label: 'POST /hello',
    content: helloData,
  },
]);
</script>

<template>
  <LayoutTile
    title="API Testing"
    description="API Testing for usabilities and functionalities."
  >
    <div class="container mx-auto max-w-md">
      <UAccordion :items="items" />
    </div>
  </LayoutTile>
</template>
