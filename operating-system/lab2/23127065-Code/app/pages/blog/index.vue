<script setup lang="ts">
definePageMeta({
  name: 'Blog',
  icon: 'lucide:book-open-text',
  description: 'Recent news and updates from our blog',
});

const { data: posts } = await useAsyncData('blog', () =>
  queryCollection('blog').order('date', 'DESC').all(),
);
</script>

<template>
  <div>
    <section class="pt-20 pb-10 lg:pt-[120px] lg:pb-20">
      <div class="container mx-auto">
        <div class="-mx-4 flex flex-wrap justify-center">
          <div class="w-full px-4">
            <div class="mx-auto mb-[60px] max-w-[510px] text-center lg:mb-20">
              <span
                class="mb-2 block text-lg font-semibold text-(--ui-primary)"
              >
                Our Blogs
              </span>
              <h2
                class="mb-4 text-3xl font-bold text-white sm:text-4xl md:text-[40px]"
              >
                Our Recent News
              </h2>
              <p class="text-dark-6 text-base">
                There are many variations of passages of Lorem Ipsum available
                but the majority have suffered alteration in some form.
              </p>
            </div>
          </div>
        </div>

        <PostList :posts="posts" />
      </div>
    </section>
  </div>
</template>
