// @ts-check
import antfu from '@antfu/eslint-config';
import eslintConfigPrettier from 'eslint-config-prettier/flat';
import nuxt from './.nuxt/eslint.config.mjs';

export default nuxt(
  antfu({
    rules: {
      'new-cap': 'off',
      'no-console': 'off',

      'style/semi': 'off',
      'style/indent': 'off',
      'style/quote-props': 'off',
      'style/brace-style': 'off',
      'style/comma-dangle': 'off',
      'style/arrow-parens': 'off',
      'style/member-delimiter-style': 'off',
      'style/operator-linebreak': 'off',

      'ts/ban-ts-comment': 'off',

      'vue/singleline-html-element-content-newline': 'off',
      'vue/require-explicit-emits': 'off',

      'antfu/if-newline': 'off',
      'antfu/top-level-function': 'off',
      'antfu/consistent-list-newline': 'off',
    },
  }),
  eslintConfigPrettier,
);
