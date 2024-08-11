#include "hash.h"

#include <iostream>

using namespace std;

template <class K, class V>
void printSearchValue(HashTable<K, V> &t, int key) {
  V *value = t.searchValue(key);
  if (value) {
    cout << "Key " << key << " has value \"" << *value << "\"\n";
  } else {
    cout << "Not found value of " << key << endl;
  }
}

int main() {
  HashTable<int, string> table;
  table.init(8);

  cout << "1. add operations\n";
  table.add(2, "two");
  table.add(4, "four");
  table.add(6, "six");
  table.add(10, "ten");
  table.add(3, "three");
  table.add(5, "five");

  for (auto *node : table.table) {
    if (node)
      cout << node->key << " " << node->value << endl;
    else
      cout << "null\n";
  }

  cout << ">>> Update key 2\n";
  table.add(2, "TWO");
  printSearchValue(table, 2);

  cout << "\n2. search operations\n";
  printSearchValue(table, 1);
  printSearchValue(table, 6);
  printSearchValue(table, 10);

  cout << "\n3. remove operations\n";
  table.removeKey(4);
  printSearchValue(table, 4);

  table.release();

  return 0;
}