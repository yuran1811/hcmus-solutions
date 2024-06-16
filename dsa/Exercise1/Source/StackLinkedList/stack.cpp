#include <iostream>
#include "stack.h"

template <typename T>
void Stack<T>::init() {
  top = nullptr;
}

template <typename T>
void Stack<T>::copyStack(const Stack<T>& s) {
  release();

  top = nullptr;
  for (Node *p = s.top, *cur = nullptr; p; p = p->next) {
    if (!top)
      top = new Node{p->data, nullptr}, cur = top;
    else {
      cur->next = new Node{p->data, nullptr};
      cur = cur->next;
    }
  }
}

template <typename T>
void Stack<T>::release() {
  for (Node* p = top; p;) {
    Node* temp = p;
    p = p->next;
    delete temp;
  }

  top = nullptr;
}

template <typename T>
bool Stack<T>::isEmpty() {
  return top == nullptr;
}

template <typename T>
void Stack<T>::push(T newItem) {
  top = new Node{newItem, top};
}

template <typename T>
T Stack<T>::pop() {
  if (isEmpty()) throw empty_message.c_str();

  Node* temp = top;
  T value = top->data;
  top = top->next;
  delete temp;

  return value;
}

template <typename T>
T Stack<T>::topValue() {
  if (isEmpty()) throw empty_message.c_str();
  return top->data;
}

template <typename T>
void Stack<T>::print() {
  for (Node* p = top; p; p = p->next) {
    std::cout << p->data << " ";
  }
  std::cout << std::endl;
}
