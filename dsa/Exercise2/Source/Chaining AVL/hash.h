#pragma once

#include "avl.h"

#include <vector>
#include <string>

using namespace std;
using namespace AVL;

template <class K, class V>
struct HashTable {
  struct HashNode {
    K key;
    V value;
    HashNode* left;
    HashNode* right;

    int height;

    HashNode(K key, V value)
        : key(key), value(value), left(nullptr), right(nullptr), height(1) {}
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