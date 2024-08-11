#pragma once
#include <vector>
#include <string>

using namespace std;

template <class K, class V>
struct HashTable {
  struct HashNode {
    K key;
    V value;
    HashNode* next;

    HashNode(K key, V value) : key(key), value(value), next(nullptr) {}
  };

  int capacity;
  vector<HashNode*> table;

  void init(unsigned int hashSize);
  void release();

  unsigned int hashFunction(int key);
  unsigned int hashFunction(string key);

  void add(K key, V value);
  V* searchValue(K key);
  void removeKey(K key);
};

#include "hash.cpp"