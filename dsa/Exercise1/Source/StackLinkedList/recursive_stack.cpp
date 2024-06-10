#include <iostream>
#include "recursive_stack.h"

template <typename T>
void RecursiveStack<T>::init() {
  top = nullptr;
}

template <typename T>
inline void RecursiveStack<T>::recurCopyStack(const RecursiveStack<T>& s,
                                              Node* p) {
  if (!p) return;

  recurCopyStack(s, p->next);
  push(p->data);
}

template <typename T>
void RecursiveStack<T>::copyStack(const RecursiveStack<T>& s) {
  release();

  top = nullptr;
  recurCopyStack(s, s.top);
}

template <typename T>
void RecursiveStack<T>::recurRelease(Node* p) {
  if (!p || !p->next) return;

  recurRelease(p->next);
  delete p;
  p = nullptr;
}

template <typename T>
void RecursiveStack<T>::release() {
  recurRelease(top);
  top = nullptr;
}

template <typename T>
bool RecursiveStack<T>::isEmpty() {
  return top == nullptr;
}

template <typename T>
void RecursiveStack<T>::push(T newItem) {
  top = new Node{newItem, top};
}

template <typename T>
T RecursiveStack<T>::pop() {
  if (isEmpty()) throw empty_message.c_str();

  Node* temp = top;
  T value = top->data;
  top = top->next;
  delete temp;

  return value;
}

template <typename T>
T RecursiveStack<T>::topValue() {
  if (isEmpty()) throw empty_message.c_str();
  return top->data;
}

template <typename T>
inline void RecursiveStack<T>::recurPrint(Node* p) {
  if (!p) return;

  std::cout << p->data << " ";
  recurPrint(p->next);
}

template <typename T>
void RecursiveStack<T>::print() {
  recurPrint(top);
  std::cout << std::endl;
}
