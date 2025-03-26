<script setup lang="ts">
import { useRouteParams } from '@vueuse/router';

const tags = useRouteParams('tag');
const tag = (tags.value || ['All tags'])[0];

const { data: rawPosts } = await useAsyncData('blog-tags', () =>
  queryCollection('blog')
    .select('title', 'date', 'tags', 'path')
    .order('date', 'DESC')
    .all(),
);

const posts = computed(() =>
  tags.value
    ? rawPosts.value?.filter((post) => post.tags?.includes(tag as string)) || []
    : rawPosts.value,
);

const breadCrumbItems = ref([
  { label: 'Home', icon: 'lucide:house', to: '/' },
  { label: 'Blog', icon: 'lucide:book-open', to: '/blog' },
  { label: 'Tags', icon: 'lucide:tags', to: '/blog/tags' },
  { label: tag, icon: 'lucide:tag' },
]);
</script>

<template>
  <article
    class="mx-auto max-w-2xl space-y-12 bg-(--ui-bg) px-6 pt-8 pb-24 text-(--ui-text)"
  >
    <UBreadcrumb :items="breadCrumbItems" />

    <PostList :posts="posts" />
  </article>
</template>
