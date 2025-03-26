export const formatByteSize = (bytes: number | string) => {
  const units = ['B', 'KB', 'MB', 'GB', 'TB'];
  let size = Number(bytes);
  for (const unit of units) {
    if (size < 1024) return `${size.toFixed(1)} ${unit}`;
    size /= 1024;
  }
  return `${size.toFixed(1)} PB`;
};

export const formatParitionLabel = (
  partition: Pick<PartitionInfo, 'label' | 'drive_letter'>,
) => {
  let drive_letter = partition.drive_letter;
  if (drive_letter === '') return 'Unallocated';
  if (drive_letter.at(-1) !== ':') drive_letter += ':';

  return partition.label === ''
    ? drive_letter
    : `${partition.label} (${drive_letter})`;
};

export const toTitleCase = (str?: string) => {
  if (!str || typeof str !== 'string') return '';

  return str.replace(
    /\w\S*/g,
    (txt) => txt.charAt(0).toUpperCase() + txt.slice(1).toLowerCase(),
  );
};
