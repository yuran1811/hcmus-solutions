#include <iostream>
#include "funcs.h"

using std::cin;
using std::cout;

void getMenuChoice(int& choice) {
  cout << ">>> Queue Lib\n";
  cout << "1. Enqueue\n";
  cout << "2. Dequeue\n";
  cout << "0. Exit\n\n";
  cout << "=: Enter your choice: ";

  cin >> choice;

  cout << '\n';
}

unsigned int get_queue_size() {
  cout << "\n=: Enter queue max size: ";
  int queue_size = 0;
  cin >> queue_size;
  while (queue_size < 1) {
    cout << "!: Invalid queue max size.\n=: Enter again: ";
    cin >> queue_size;
  }
  cout << '\n';

  return (unsigned int)queue_size;
}

template <typename QueueType>
bool queue_uninitialized_handler(unsigned int& queue_size, QueueType*& q) {
  cout << "!: Queue is not initialized.\n";
  cout << "?: Do you want to initialize the queue? Yes(1) - No(0)\n";
  int c = 0;
  cin >> c;
  if (c == 0) return false;

  if (q) q->release();
  delete q;
  q = new QueueType();
  q->init();

  return true;
}