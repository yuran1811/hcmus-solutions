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
    hash = (hash + c * pow) % m;
    pow = (pow * p) % m;
  }

  return hash % capacity;
}

template <class K, class V>
void HashTable<K, V>::add(K key, V value) {
  unsigned int index = hashFunction(key);

  for (int loopCount = 0;
       loopCount < capacity && table[index] && table[index]->key != key;
       loopCount++) {
    index = (index + 1) % capacity;

    if (loopCount == capacity - 1) {
      cout << ">>> Cannot add \"" << key << ":" << value
           << "\"\n";
      return;
    }
  }

  if (table[index]) {
    table[index]->value = value;
  } else {
    table[index] = new HashNode(key, value);
  }
}

template <class K, class V>
V* HashTable<K, V>::searchValue(K key) {
  unsigned int index = hashFunction(key);

  for (int loopCount = 0;
       loopCount < capacity && table[index] && table[index]->key != key;
       loopCount++) {
    index = (index + 1) % capacity;
  }

  return table[index] ? &table[index]->value : nullptr;
}

template <class K, class V>
void HashTable<K, V>::removeKey(K key) {
  unsigned int index = hashFunction(key);

  for (int loopCount = 0;
       loopCount < capacity && table[index] && table[index]->key != key;
       loopCount++) {
    index = (index + 1) % capacity;
  }

  if (table[index]) {
    delete table[index];
    table[index] = nullptr;
  }
}
