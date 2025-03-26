<script setup lang="ts">
import type { BlogCollectionItem } from '@nuxt/content';
import { DefaultDateFormat } from '@/constants';

defineProps<{
  posts?: Pick<BlogCollectionItem, 'title' | 'date' | 'path' | 'tags'>[];
}>();
</script>

<template>
  <div
    class="grid auto-rows-fr grid-cols-1 gap-8 py-10 md:grid-cols-2 lg:grid-cols-3"
  >
    <template v-for="post in posts" :key="post.id">
      <article
        class="hover:animate-background rounded-xl bg-gradient-to-r from-green-300 via-blue-500 to-purple-600 p-0.5 shadow-xl transition hover:bg-[length:400%_400%] hover:shadow-xs hover:[animation-duration:_4s] dark:shadow-gray-700/25"
      >
        <div
          class="relative flex h-full flex-col items-start justify-end overflow-hidden rounded-[10px] bg-white p-4 !pt-18 sm:p-6 dark:bg-gray-900"
        >
          <p class="block text-xs text-gray-500 dark:text-gray-400">
            {{ useDateFormat(post.date || new Date(), DefaultDateFormat) }}
          </p>

          <NuxtLink
            :to="post.path"
            class="mt-0.5 line-clamp-2 text-lg font-medium text-gray-900 dark:text-white"
          >
            {{ post.title || 'Untitled' }}
          </NuxtLink>

          <div class="mt-4 flex flex-wrap gap-1">
            <template v-for="tag in post.tags || []" :key="tag">
              <NuxtLink :to="`/blog/tags/${tag}`">
                <span
                  class="rounded-full px-2.5 py-0.5 text-xs whitespace-nowrap dark:bg-purple-600 dark:text-purple-100"
                >
                  {{ tag }}
                </span>
              </NuxtLink>
            </template>
          </div>
        </div>
      </article>
    </template>
  </div>
</template>
