#include <iostream>
#include "recursive_stack.h"

template <typename T>
void RecursiveStack<T>::init(unsigned int stackSize) {
  top = -1;
  maxSize = stackSize;
  items = new T[maxSize];
}

template <typename T>
void RecursiveStack<T>::recurCopyStack(const RecursiveStack<T>& s, int i,
                                       int n) {
  if (i == n) return;
  recurCopyStack(s, i + 1, n);
  items[i] = s.items[i];
}

template <typename T>
void RecursiveStack<T>::copyStack(const RecursiveStack<T>& s) {
  release();

  top = s.top;
  maxSize = s.maxSize;
  items = new T[maxSize];

  recurCopyStack(s, 0, top + 1);
}

template <typename T>
void RecursiveStack<T>::release() {
  top = -1;
  maxSize = 0;
  delete[] items;
}

template <typename T>
bool RecursiveStack<T>::isEmpty() {
  return top == -1;
}

template <typename T>
void RecursiveStack<T>::push(T newItem) {
  if (top == int(maxSize - 1)) throw full_message.c_str();
  items[++top] = newItem;
}

template <typename T>
T RecursiveStack<T>::pop() {
  if (isEmpty()) throw empty_message.c_str();
  return items[top--];
}

template <typename T>
T RecursiveStack<T>::topValue() {
  if (isEmpty()) throw empty_message.c_str();
  return items[top];
}

template <typename T>
inline void RecursiveStack<T>::recurPrint(int i) {
  if (i > top) return;
  std::cout << items[i] << " ";
  recurPrint(i + 1);
}

template <typename T>
void RecursiveStack<T>::print() {
  recurPrint(0);
  std::cout << std::endl;
}