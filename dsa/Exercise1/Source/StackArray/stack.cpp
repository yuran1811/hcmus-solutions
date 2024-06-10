#include <iostream>
#include "stack.h"

template <typename T>
void Stack<T>::init(unsigned int stackSize) {
  top = -1;
  maxSize = stackSize;
  items = new T[maxSize];
}

template <typename T>
void Stack<T>::copyStack(const Stack<T>& s) {
  release();

  top = s.top;
  maxSize = s.maxSize;
  items = new T[maxSize];

  for (int i = 0; i <= top; i++) {
    items[i] = s.items[i];
  }
}

template <typename T>
void Stack<T>::release() {
  top = -1;
  maxSize = 0;
  delete[] items;
}

template <typename T>
bool Stack<T>::isEmpty() {
  return top == -1;
}

template <typename T>
void Stack<T>::push(T newItem) {
  if (top == int(maxSize - 1)) throw full_message.c_str();
  items[++top] = newItem;
}

template <typename T>
T Stack<T>::pop() {
  if (isEmpty()) throw empty_message.c_str();
  return items[top--];
}

template <typename T>
T Stack<T>::topValue() {
  if (isEmpty()) throw empty_message.c_str();
  return items[top];
}

template <typename T>
void Stack<T>::print() {
  for (int i = 0; i <= top; i++) {
    std::cout << items[i] << " ";
  }
}
