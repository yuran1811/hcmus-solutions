<script setup lang="ts">
import { DefaultDateFormat } from '@/constants';
import { useRouteParams } from '@vueuse/router';

const slug = useRouteParams('slug');

const { data: post } = await useAsyncData(`blog-${slug.value}`, () =>
  queryCollection('blog').path(`/blog/${slug.value}`).first(),
);

const postData = computed(() => ({
  title: post.value?.title || 'Untitled',
  author: post.value?.author || 'Anonymous',
  date: post.value?.date || new Date(),
  tags: post.value?.tags || [],
}));

const tocLinks = computed(() => post.value?.body?.toc?.links || []);

const breadCrumbItems = ref([
  { label: 'Home', icon: 'lucide:house', to: '/' },
  { label: 'Blog', icon: 'lucide:book-open', to: '/blog' },
  {
    label: post.value?.title ? post.value.title : 'Post',
    icon: 'lucide:file',
  },
]);
</script>

<template>
  <article
    class="relative mx-auto max-w-2xl space-y-12 bg-(--ui-bg) px-6 pt-8 pb-24 text-(--ui-text)"
  >
    <UBreadcrumb :items="breadCrumbItems" />

    <div class="mx-auto w-full space-y-4 text-center">
      <p class="text-xs font-semibold tracking-wider uppercase">
        <template v-for="tag in postData.tags" :key="tag">
          <NuxtLink
            :to="`/blog/tags/${tag}`"
            class="inline-block rounded-md bg-(--ui-bg) px-2 py-1 text-xs font-semibold tracking-wider text-(--ui-text-muted) lowercase hover:text-(--ui-text)"
          >
            #{{ tag }}
          </NuxtLink>
        </template>
      </p>

      <h1 class="text-4xl leading-tight font-bold md:text-5xl">
        {{ postData.title }}
      </h1>

      <p class="text-sm text-(--ui-text-muted)">
        by
        <NuxtLink
          :to="`/blog/authors/${postData.author}`"
          class="text-violet-400 underline"
        >
          <span itemprop="name">{{ postData.author }}</span>
        </NuxtLink>
        on
        <time datetime="2021-02-12 15:34:18-0200">{{
          useDateFormat(postData.date, DefaultDateFormat).value
        }}</time>
      </p>
    </div>

    <div class="border-y border-(--ui-text-dimmed) py-12">
      <div
        class="flex flex-col space-y-4 md:flex-row md:space-y-0 md:space-x-6"
      >
        <img
          src="/images/github.png"
          alt=""
          class="h-24 w-24 flex-shrink-0 self-center rounded-full border border-(--ui-text-dimmed) bg-gray-500 md:justify-self-start"
        />
        <div class="flex flex-col gap-2">
          <div
            class="xs:flex-row flex flex-col flex-wrap justify-between gap-4 text-center text-lg font-semibold md:text-left"
          >
            <p>
              {{ postData.author }}
            </p>
            <div
              class="align-center flex justify-center gap-4 *:scale-125 *:transition"
            >
              <a
                rel="noopener noreferrer"
                href="#"
                aria-label="GitHub"
                class="rounded-md p-2 text-(--ui-bg-revert) hover:text-violet-400"
              >
                <svg
                  viewBox="0 0 496 512"
                  xmlns="http://www.w3.org/2000/svg"
                  class="h-4 w-4 fill-current"
                >
                  <path
                    d="M165.9 397.4c0 2-2.3 3.6-5.2 3.6-3.3.3-5.6-1.3-5.6-3.6 0-2 2.3-3.6 5.2-3.6 3-.3 5.6 1.3 5.6 3.6zm-31.1-4.5c-.7 2 1.3 4.3 4.3 4.9 2.6 1 5.6 0 6.2-2s-1.3-4.3-4.3-5.2c-2.6-.7-5.5.3-6.2 2.3zm44.2-1.7c-2.9.7-4.9 2.6-4.6 4.9.3 2 2.9 3.3 5.9 2.6 2.9-.7 4.9-2.6 4.6-4.6-.3-1.9-3-3.2-5.9-2.9zM244.8 8C106.1 8 0 113.3 0 252c0 110.9 69.8 205.8 169.5 239.2 12.8 2.3 17.3-5.6 17.3-12.1 0-6.2-.3-40.4-.3-61.4 0 0-70 15-84.7-29.8 0 0-11.4-29.1-27.8-36.6 0 0-22.9-15.7 1.6-15.4 0 0 24.9 2 38.6 25.8 21.9 38.6 58.6 27.5 72.9 20.9 2.3-16 8.8-27.1 16-33.7-55.9-6.2-112.3-14.3-112.3-110.5 0-27.5 7.6-41.3 23.6-58.9-2.6-6.5-11.1-33.3 2.6-67.9 20.9-6.5 69 27 69 27 20-5.6 41.5-8.5 62.8-8.5s42.8 2.9 62.8 8.5c0 0 48.1-33.6 69-27 13.7 34.7 5.2 61.4 2.6 67.9 16 17.7 25.8 31.5 25.8 58.9 0 96.5-58.9 104.2-114.8 110.5 9.2 7.9 17 22.9 17 46.4 0 33.7-.3 75.4-.3 83.6 0 6.5 4.6 14.4 17.3 12.1C428.2 457.8 496 362.9 496 252 496 113.3 383.5 8 244.8 8zM97.2 352.9c-1.3 1-1 3.3.7 5.2 1.6 1.6 3.9 2.3 5.2 1 1.3-1 1-3.3-.7-5.2-1.6-1.6-3.9-2.3-5.2-1zm-10.8-8.1c-.7 1.3.3 2.9 2.3 3.9 1.6 1 3.6.7 4.3-.7.7-1.3-.3-2.9-2.3-3.9-2-.6-3.6-.3-4.3.7zm32.4 35.6c-1.6 1.3-1 4.3 1.3 6.2 2.3 2.3 5.2 2.6 6.5 1 1.3-1.3.7-4.3-1.3-6.2-2.2-2.3-5.2-2.6-6.5-1zm-11.4-14.7c-1.6 1-1.6 3.6 0 5.9 1.6 2.3 4.3 3.3 5.6 2.3 1.6-1.3 1.6-3.9 0-6.2-1.4-2.3-4-3.3-5.6-2z"
                  />
                </svg>
              </a>
              <a
                rel="noopener noreferrer"
                href="#"
                aria-label="Dribble"
                class="rounded-md p-2 text-(--ui-bg-revert) hover:text-violet-400"
              >
                <svg
                  viewBox="0 0 512 512"
                  xmlns="http://www.w3.org/2000/svg"
                  class="h-4 w-4 fill-current"
                >
                  <path
                    d="M256 8C119.252 8 8 119.252 8 256s111.252 248 248 248 248-111.252 248-248S392.748 8 256 8zm163.97 114.366c29.503 36.046 47.369 81.957 47.835 131.955-6.984-1.477-77.018-15.682-147.502-6.818-5.752-14.041-11.181-26.393-18.617-41.614 78.321-31.977 113.818-77.482 118.284-83.523zM396.421 97.87c-3.81 5.427-35.697 48.286-111.021 76.519-34.712-63.776-73.185-116.168-79.04-124.008 67.176-16.193 137.966 1.27 190.061 47.489zm-230.48-33.25c5.585 7.659 43.438 60.116 78.537 122.509-99.087 26.313-186.36 25.934-195.834 25.809C62.38 147.205 106.678 92.573 165.941 64.62zM44.17 256.323c0-2.166.043-4.322.108-6.473 9.268.19 111.92 1.513 217.706-30.146 6.064 11.868 11.857 23.915 17.174 35.949-76.599 21.575-146.194 83.527-180.531 142.306C64.794 360.405 44.17 310.73 44.17 256.323zm81.807 167.113c22.127-45.233 82.178-103.622 167.579-132.756 29.74 77.283 42.039 142.053 45.189 160.638-68.112 29.013-150.015 21.053-212.768-27.882zm248.38 8.489c-2.171-12.886-13.446-74.897-41.152-151.033 66.38-10.626 124.7 6.768 131.947 9.055-9.442 58.941-43.273 109.844-90.795 141.978z"
                  />
                </svg>
              </a>
              <a
                rel="noopener noreferrer"
                href="#"
                aria-label="Email"
                class="rounded-md p-2 text-(--ui-bg-revert) hover:text-violet-400"
              >
                <svg
                  viewBox="0 0 512 512"
                  xmlns="http://www.w3.org/2000/svg"
                  class="h-4 w-4 fill-current"
                >
                  <path
                    d="M464 64H48C21.49 64 0 85.49 0 112v288c0 26.51 21.49 48 48 48h416c26.51 0 48-21.49 48-48V112c0-26.51-21.49-48-48-48zm0 48v40.805c-22.422 18.259-58.168 46.651-134.587 106.49-16.841 13.247-50.201 45.072-73.413 44.701-23.208.375-56.579-31.459-73.413-44.701C106.18 199.465 70.425 171.067 48 152.805V112h416zM48 400V214.398c22.914 18.251 55.409 43.862 104.938 82.646 21.857 17.205 60.134 55.186 103.062 54.955 42.717.231 80.509-37.199 103.053-54.947 49.528-38.783 82.032-64.401 104.947-82.653V400H48z"
                  />
                </svg>
              </a>
            </div>
          </div>
          <p class="text-(--ui-text-muted)">
            Sed non nibh iaculis, posuere diam vitae, consectetur neque. Integer
            velit ligula, semper in, cursus commodo elit. Pellentesque sit amet
            mi luctus ligula.
          </p>
        </div>
      </div>
    </div>

    <div>
      <TOC :tocs="tocLinks" />

      <ContentRenderer
        v-if="post"
        class="prose prose-invert container mx-auto"
        :prose="true"
        :value="post"
      />
      <div v-else>
        <p class="p-4 text-center font-bold">Post not found</p>
      </div>
    </div>
  </article>
</template>
