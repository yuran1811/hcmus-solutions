#include "hash.h"

template <class K, class V>
void HashTable<K, V>::init(unsigned int hashSize) {
  capacity = hashSize;
  table.assign(capacity, nullptr);
}

template <class K, class V>
void HashTable<K, V>::release() {
  for (int i = 0; i < capacity; i++) {
    if (table[i]) delete table[i];
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
    hash = (hash + (1LL * c * pow) % m) % m;
    pow = (1LL * pow * p) % m;
  }

  return hash % capacity;
}

template <class K, class V>
void HashTable<K, V>::add(K key, V value) {
  unsigned int hashKey = hashFunction(key);
  unsigned int index = hashKey;

  for (int loopCount = 0; table[index]; loopCount++) {
    if (table[index]->key == key) {
      table[index]->value = value;
      return;
    }

    if (loopCount == capacity) {
      rehashing();
      add(key, value);
      return;
    }

    index = (hashKey + loopCount) % capacity;
  }

  table[index] = new HashNode(key, value);
}

template <class K, class V>
V* HashTable<K, V>::searchValue(K key) {
  unsigned int hashKey = hashFunction(key);
  unsigned int index = hashKey;

  for (int loopCount = 0; table[index]; loopCount++) {
    if (table[index]->key == key) return &table[index]->value;

    if (loopCount == capacity) return nullptr;

    index = (hashKey + loopCount) % capacity;
  }

  return nullptr;
}

template <class K, class V>
void HashTable<K, V>::removeKey(K key) {
  unsigned int hashKey = hashFunction(key);
  unsigned int index = hashKey;

  for (int loopCount = 0; table[index]; loopCount++) {
    if (table[index]->key == key) break;

    if (loopCount == capacity) break;

    index = (hashKey + loopCount) % capacity;
  }

  if (table[index]) {
    delete table[index];
    table[index] = nullptr;
  }
}

template <class K, class V>
inline void HashTable<K, V>::rehashing() {
  vector<HashNode*> _t = table;

  capacity <<= 1;
  table.assign(capacity, nullptr);

  for (HashNode* node : _t) {
    if (node) {
      add(node->key, node->value);
      delete node;
    }
  }
}