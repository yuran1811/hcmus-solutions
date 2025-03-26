import { IconsMapping } from '~/constants';

export const getFileIcon = (node: DirectoryNode) => {
  const lowerName = (node?.name.toLowerCase() as string) || '';
  if (IconsMapping.lockfiles && lowerName in IconsMapping.lockfiles)
    return IconsMapping.lockfiles[lowerName];

  const specialIcon = lowerName.match(
    /(eslint|esbuild|prettier|webpack|postcss|tailwind|readme|license|todo|nuxt|vitest|vite|pnpm-lock|pnpm|npm|changelog|cmake|gitlab|github|git|robots|rollup|makefile|docker|env|babel)/g,
  );
  if (specialIcon) return specialIcon[0];

  const nodeNameTokens = lowerName.split('.') || [];
  const lastToken = nodeNameTokens.at(-1) as string;

  if (lastToken.includes('class')) return 'java-class';
  if (lastToken.includes('nvm')) return 'node';

  if (nodeNameTokens.length <= 1) return '';
  return IconsMapping.fileType &&
    Object.hasOwn(IconsMapping.fileType, lastToken)
    ? IconsMapping.fileType[lastToken]
    : lastToken;
};

export const getFolderIcon = (node: DirectoryNode) => {
  const lowerName = (node?.name.toLowerCase() as string) || '';
  if (IconsMapping.lockfiles && lowerName in IconsMapping.lockfiles)
    return IconsMapping.lockfiles[lowerName];

  const specialIcon = lowerName.match(
    /(yarn|workflows|vscode|vercel|upload|tauri|svg|scripts|nuxt|nix|next|github|git|fonts|android|config|gradle|utils)/g,
  );
  if (specialIcon) return specialIcon[0];

  if (lowerName === 'node_modules') return 'node';

  if (lowerName.length <= 1) return '';
  return IconsMapping.folderType &&
    Object.hasOwn(IconsMapping.folderType, lowerName)
    ? IconsMapping.folderType[lowerName]
    : lowerName;
};
