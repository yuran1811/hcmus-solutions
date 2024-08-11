#include "hash.h"

template <class K, class V>
void HashTable<K, V>::init(unsigned int hashSize) {
  capacity = hashSize;
  table.assign(capacity, nullptr);
}

template <class K, class V>
void HashTable<K, V>::release() {
  for (int i = 0; i < capacity; i++) {
    if (table[i])
      while (table[i])
        table[i] = deleteNode<HashNode, K>(table[i], table[i]->key);
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

  table[hashKey] = insertNode<HashNode, K, V>(table[hashKey], key, value);
}

template <class K, class V>
V* HashTable<K, V>::searchValue(K key) {
  unsigned int hashKey = hashFunction(key);

  HashNode* node = searchNode<HashNode, K>(table[hashKey], key);
  if (!node) return nullptr;

  return &node->value;
}

template <class K, class V>
void HashTable<K, V>::removeKey(K key) {
  unsigned int hashKey = hashFunction(key);

  table[hashKey] = deleteNode<HashNode, K>(table[hashKey], key);
}
