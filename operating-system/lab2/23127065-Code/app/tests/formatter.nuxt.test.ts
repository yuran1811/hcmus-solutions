import { describe, expect, it } from 'vitest';
import {
  formatByteSize,
  formatParitionLabel,
  toTitleCase,
} from '~/utils/formatter';

describe('formatter', () => {
  describe('formatByteSize', () => {
    it('should format byte values correctly', () => {
      expect(formatByteSize(0)).toBe('0.0 B');
      expect(formatByteSize(100)).toBe('100.0 B');
      expect(formatByteSize(1023)).toBe('1023.0 B');
    });

    it('should format kilobyte values correctly', () => {
      expect(formatByteSize(1024)).toBe('1.0 KB');
      expect(formatByteSize(1536)).toBe('1.5 KB');
      expect(formatByteSize(1024 * 1023)).toBe('1023.0 KB');
    });

    it('should format megabyte values correctly', () => {
      expect(formatByteSize(1024 * 1024)).toBe('1.0 MB');
      expect(formatByteSize(1.5 * 1024 * 1024)).toBe('1.5 MB');
    });

    it('should format gigabyte values correctly', () => {
      expect(formatByteSize(1024 * 1024 * 1024)).toBe('1.0 GB');
      expect(formatByteSize(2.5 * 1024 * 1024 * 1024)).toBe('2.5 GB');
    });

    it('should format terabyte values correctly', () => {
      expect(formatByteSize(1024 * 1024 * 1024 * 1024)).toBe('1.0 TB');
    });

    it('should format petabyte values correctly', () => {
      expect(formatByteSize(1024 * 1024 * 1024 * 1024 * 1024)).toBe('1.0 PB');
    });

    it('should handle string input correctly', () => {
      expect(formatByteSize('1024')).toBe('1.0 KB');
    });
  });

  describe('formatParitionLabel', () => {
    it('should return empty string when both label and drive_letter are empty', () => {
      expect(formatParitionLabel({ label: '', drive_letter: '' })).toBe(
        'Unallocated',
      );
    });

    it('should return only label when drive_letter is empty', () => {
      expect(formatParitionLabel({ label: 'System', drive_letter: '' })).toBe(
        'Unallocated',
      );
    });

    it('should return only drive_letter with colon when label is empty', () => {
      expect(formatParitionLabel({ label: '', drive_letter: 'C:' })).toBe('C:');
    });

    it('should return formatted string with label and drive_letter when both are present', () => {
      expect(formatParitionLabel({ label: 'Windows', drive_letter: 'C' })).toBe(
        'Windows (C:)',
      );
    });

    it('should handle non-standard drive letters', () => {
      expect(formatParitionLabel({ label: 'Data', drive_letter: 'Z' })).toBe(
        'Data (Z:)',
      );
    });
  });

  describe('toTitleCase', () => {
    it('should convert a lowercase string to title case', () => {
      expect(toTitleCase('hello')).toBe('Hello');
      expect(toTitleCase('world')).toBe('World');
    });

    it('should convert an uppercase string to title case', () => {
      expect(toTitleCase('HELLO')).toBe('Hello');
      expect(toTitleCase('WORLD')).toBe('World');
    });

    it('should convert a mixed case string to title case', () => {
      expect(toTitleCase('hElLo')).toBe('Hello');
      expect(toTitleCase('WoRlD')).toBe('World');
    });

    it('should convert multiple words to title case', () => {
      expect(toTitleCase('hello world')).toBe('Hello World');
      expect(toTitleCase('the quick brown fox')).toBe('The Quick Brown Fox');
    });

    it('should handle strings with special characters', () => {
      expect(toTitleCase('hello-world')).toBe('Hello-world');
      expect(toTitleCase('hello_world')).toBe('Hello_world');
      expect(toTitleCase('hello.world')).toBe('Hello.world');
    });

    it('should handle strings with unusual spacing', () => {
      expect(toTitleCase('  hello  world  ')).toBe('  Hello  World  ');
      expect(toTitleCase('hello   world')).toBe('Hello   World');
    });

    it('should handle empty string', () => {
      expect(toTitleCase('')).toBe('');
    });

    it('should handle null or undefined input', () => {
      expect(toTitleCase(undefined)).toBe('');
      expect(toTitleCase(null as unknown as string)).toBe('');
    });

    it('should handle non-string input', () => {
      expect(toTitleCase(123 as unknown as string)).toBe('');
      expect(toTitleCase(true as unknown as string)).toBe('');
      expect(toTitleCase({} as unknown as string)).toBe('');
    });
  });
});
