export const getNodeInfoItems = (node: DirectoryNode) => {
  const items: { label: string }[] = [];
  const metadata: { label: string }[] = [];

  const generateLabel = (key: string, value: string) => {
    return `${toTitleCase(key.replace(/[_-]/, ' '))}: ${value}`;
  };

  Object.entries(node).forEach(([key, value]) => {
    if (!['name', 'path'].includes(key)) return;
    items.push({ label: generateLabel(key, value) });
  });

  Object.entries(node?.metadata || {}).forEach(([key, value]) => {
    if (key === 'size' && !node.is_file) return;
    metadata.push({
      label: generateLabel(
        key === 'name' ? 'full name' : key,
        key === 'size' ? formatByteSize(value) : value,
      ),
    });
  });

  return [
    ...items,
    {
      label: 'Metadata',
      children: metadata,
    },
  ];
};
