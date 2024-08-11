#include "hash.h"

#include <iostream>

template <class K, class V>
void HashTable<K, V>::init(unsigned int hashSize) {
  capacity = hashSize;
  table.assign(capacity, nullptr);
}

template <class K, class V>
void HashTable<K, V>::release() {
  for (int i = 0; i < capacity; i++) {
    if (table[i]) {
      for (HashNode* p = table[i]; p;) {
        HashNode* tmp = p;
        p = p->next;
        delete tmp;
      }
    }

    table[i] = nullptr;
  }
}

template <class K, class V>
unsigned int HashTable<K, V>::hashFunction(int key) {
  return key % capacity;
}

template <class K, class V>
unsigned int HashTable<K, V>::hashFunction(string key) {
  const int m = 1e9 + 9;
  const int p = 31;

  unsigned int hash = 0;
  int pow = p;

  for (const char& c : key) {
    hash = (hash + c * pow) % m;
    pow = (pow * p) % m;
  }

  return hash % capacity;
}

template <class K, class V>
void HashTable<K, V>::add(K key, V value) {
  unsigned int hashKey = hashFunction(key);

  HashNode* newNode = new HashNode(key, value);
  HashNode* p = table[hashKey];

  if (!p) {
    table[hashKey] = newNode;
    return;
  }

  for (; p; p = p->next) {
    if (p->key == key) {
      p->value = value;
      return;
    }

    if (!p->next) p->next = newNode;
  }
}

template <class K, class V>
V* HashTable<K, V>::searchValue(K key) {
  unsigned int hashKey = hashFunction(key);

  for (HashNode* p = table[hashKey]; p; p = p->next)
    if (p->key == key) return &p->value;

  return nullptr;
}

template <class K, class V>
void HashTable<K, V>::removeKey(K key) {
  unsigned int hashKey = hashFunction(key);

  HashNode* node = table[hashKey];
  HashNode* prev = nullptr;

  while (node) {
    if (node->key == key) {
      if (prev) {
        prev->next = node->next;
      } else {
        table[hashKey] = node->next;
      }

      delete node;
      return;
    }

    prev = node;
    node = node->next;
  }
}
