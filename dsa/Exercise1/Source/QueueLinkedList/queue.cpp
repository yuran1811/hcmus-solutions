#include <iostream>
#include "queue.h"

template <typename T>
void Queue<T>::init() {
  front = rear = nullptr;
}

template <typename T>
void Queue<T>::release() {
  while (front) {
    Node* temp = front;
    front = front->next;
    delete temp;
  }

  front = rear = nullptr;
}

template <typename T>
bool Queue<T>::isEmpty() {
  return !front || !rear;
}

template <typename T>
void Queue<T>::enqueue(T newItem) {
  Node* newNode = new Node{newItem, nullptr};

  if (isEmpty()) {
    front = rear = newNode;
  } else {
    rear->next = newNode;
    rear = newNode;
  }
}

template <typename T>
T Queue<T>::dequeue() {
  if (isEmpty()) throw empty_message.c_str();

  T value = front->data;
  Node* temp = front;
  front = front->next;
  delete temp;

  return value;
}

template <typename T>
T Queue<T>::frontValue() {
  if (isEmpty()) throw empty_message.c_str();
  return front->data;
}

template <typename T>
void Queue<T>::print() {
  for (Node* p = front; p; p = p->next) {
    std::cout << p->data << " ";
  }
  std::cout << std::endl;
}
