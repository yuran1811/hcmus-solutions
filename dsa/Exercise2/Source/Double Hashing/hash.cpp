#include "hash.h"

#include <iostream>

int countCollision = 0;

bool is_prime(int n) {
  if (n < 2) return false;

  for (int i = 2; i * i <= n; i++)
    if (n % i == 0) return false;

  return true;
}

template <class K, class V>
void HashTable<K, V>::init(unsigned int hashSize) {
  hashSize *= 195;
  while (!is_prime(hashSize)) hashSize++;
  cout << "Hash size: " << hashSize << endl;

  capacity = hashSize;
  collisionCount.assign(capacity, 0);
  table.assign(capacity, nullptr);
}

template <class K, class V>
void HashTable<K, V>::release() {
  for (int i = 0; i < capacity; i++) {
    collisionCount[i] = 0;

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
unsigned int HashTable<K, V>::hashFunction2(int key) {
  return 1 + (key % (capacity - 1));
}

template <class K, class V>
unsigned int HashTable<K, V>::hashFunction2(string key) {
  return 1 + (hashFunction(key) % (capacity - 1));
}

template <class K, class V>
void HashTable<K, V>::add(K key, V value) {
  unsigned int hashKey = hashFunction(key);
  unsigned int hashKey2 = hashFunction2(key);
  unsigned int index = hashKey;

  for (int loopCount = 0; loopCount <= collisionCount[hashKey] &&
                          table[index] && table[index]->key != key;
       loopCount++) {
    index = (hashKey + loopCount * hashKey2) % capacity;
  }

  if (table[index]) {
    if (table[index]->key != key) return void(countCollision++);

    table[index]->value = value;
  } else {
    collisionCount[hashKey]++;
    table[index] = new HashNode(key, value);
  }
}

template <class K, class V>
V* HashTable<K, V>::searchValue(K key) {
  unsigned int hashKey = hashFunction(key);
  unsigned int hashKey2 = hashFunction2(key);
  unsigned int index = hashKey;

  for (int loopCount = 0; loopCount <= collisionCount[hashKey] &&
                          table[index] && table[index]->key != key;
       loopCount++) {
    index = (hashKey + loopCount * hashKey2) % capacity;
  }

  return table[index] && table[index]->key == key ? &table[index]->value
                                                  : nullptr;
}

template <class K, class V>
void HashTable<K, V>::removeKey(K key) {
  unsigned int hashKey = hashFunction(key);
  unsigned int hashKey2 = hashFunction2(key);
  unsigned int index = hashKey;

  for (int loopCount = 0; loopCount <= collisionCount[hashKey] &&
                          table[index] && table[index]->key != key;
       loopCount++) {
    index = (hashKey + loopCount * hashKey2) % capacity;
  }

  if (table[index]) {
    delete table[index];
    table[index] = nullptr;
  }
}