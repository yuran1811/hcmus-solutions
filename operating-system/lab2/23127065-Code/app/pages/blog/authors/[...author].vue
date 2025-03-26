<script setup lang="ts">
import { useRouteParams } from '@vueuse/router';

const rawAuthor = useRouteParams('author');
const author = ref(rawAuthor.value?.at(0) || 'Anonymous');

const { data: rawPosts } = await useAsyncData('blog-authors', () =>
  queryCollection('blog')
    .select('title', 'date', 'tags', 'path', 'author')
    .order('date', 'DESC')
    .all(),
);

const posts = computed(() =>
  rawAuthor.value
    ? rawPosts.value?.filter((post) =>
        author.value === 'Anonymous'
          ? !post.author
          : post.author === author.value,
      ) || []
    : rawPosts.value,
);

const breadCrumbItems = ref([
  { label: 'Home', icon: 'lucide:house', to: '/' },
  { label: 'Blog', icon: 'lucide:book-open', to: '/blog' },
  { label: 'Author', icon: 'lucide:users-round', to: '/blog/authors' },
  {
    label: rawAuthor.value ? author.value : 'All authors',
    icon: 'lucide:user-round',
  },
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
