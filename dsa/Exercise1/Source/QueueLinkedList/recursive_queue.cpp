#include <iostream>
#include "recursive_queue.h"

template <typename T>
void RecursiveQueue<T>::init() {
  front = rear = nullptr;
}

template <typename T>
inline void RecursiveQueue<T>::recurRelease(Node* p) {
  if (!p || !p->next) return;

  recurRelease(p->next);
  delete p;
  p = nullptr;
}

template <typename T>
void RecursiveQueue<T>::release() {
  recurRelease(front);
  front = rear = nullptr;
}

template <typename T>
bool RecursiveQueue<T>::isEmpty() {
  return !front || !rear;
}

template <typename T>
void RecursiveQueue<T>::enqueue(T newItem) {
  Node* newNode = new Node{newItem, nullptr};

  if (isEmpty()) {
    front = rear = newNode;
  } else {
    rear->next = newNode;
    rear = newNode;
  }
}

template <typename T>
T RecursiveQueue<T>::dequeue() {
  if (isEmpty()) throw empty_message.c_str();

  T value = front->data;
  Node* temp = front;
  front = front->next;
  delete temp;

  return value;
}

template <typename T>
T RecursiveQueue<T>::frontValue() {
  if (isEmpty()) throw empty_message.c_str();
  return front->data;
}

template <typename T>
inline void RecursiveQueue<T>::recurPrint(Node* p) {
  if (!p) return;

  std::cout << p->data << " ";
  recurPrint(p->next);
}

template <typename T>
void RecursiveQueue<T>::print() {
  recurPrint(front);
  std::cout << std::endl;
}
