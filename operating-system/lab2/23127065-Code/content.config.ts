import { defineCollection, defineContentConfig, z } from '@nuxt/content';

export default defineContentConfig({
  collections: {
    blog: defineCollection({
      type: 'page',
      source: 'blog/*.{md,mdx}',
      schema: z.object({
        author: z.string(),
        date: z.date(),
        tags: z.array(z.string()),
        image: z.string(),
      }),
    }),
  },
});
