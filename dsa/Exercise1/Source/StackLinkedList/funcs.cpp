#include <iostream>
#include "funcs.h"

using std::cin;
using std::cout;

void getMenuChoice(int& choice) {
  cout << ">>> Stack Lib\n";
  cout << "1. Push\n";
  cout << "2. Pop\n";
  cout << "0. Exit\n\n";
  cout << "=: Enter your choice: ";

  cin >> choice;

  cout << '\n';
}

unsigned int get_stack_size() {
  cout << "\n=: Enter stack max size: ";
  int stack_size = 0;
  cin >> stack_size;
  while (stack_size < 1) {
    cout << "!: Invalid stack max size.\n=: Enter again: ";
    cin >> stack_size;
  }
  cout << '\n';

  return (unsigned int)stack_size;
}

template <typename StackType>
bool stack_uninitialized_handler(unsigned int& stack_size, StackType*& s) {
  cout << "!: Stack is not initialized.\n";
  cout << "?: Do you want to initialize the stack? Yes(1) - No(0)\n";
  int c = 0;
  cin >> c;
  if (c == 0) return false;

  if (s) s->release();
  delete s;
  s = new StackType();
  s->init();

  return true;
}