import { z } from 'zod';

const querySchema = z.object({
  limit: z.string().min(0).default('0'),
  page: z.string().min(0).default('0'),
});

export default defineEventHandler(async (ev) => {
  const res = await queryCollection(ev, 'blog').order('date', 'DESC').all();

  const { limit, page } = await getValidatedQuery(ev, querySchema.parse);
  const reqLimit = limit ? Number.parseInt(limit.replace(/\D*/g, '')) : 0;
  const reqPage = page ? Number.parseInt(page.replace(/\D*/g, '')) : 0;

  const allPosts = res.map((post) => ({
    id: post.id,
    title: post.title,
    date: post.date,
  }));

  if (!page || !reqLimit || reqLimit > res.length) return allPosts;
  return allPosts.slice(reqPage * reqLimit, (reqPage + 1) * reqLimit);
});
