#include <iostream>
#include "queue.h"

template <typename T>
void Queue<T>::init(unsigned int queueSize) {
  front = rear = -1;
  count = 0;
  maxSize = queueSize;
  items = new T[queueSize];
}

template <typename T>
void Queue<T>::copyQueue(const Queue<T>& q) {
  release();
  
  front = q.front;
  rear = q.rear;
  count = q.count;
  maxSize = q.maxSize;
  items = new T[maxSize];

  for (int i = front; i <= rear; i++) {
    items[i] = q.items[i];
  }
}

template <typename T>
void Queue<T>::release() {
  if (!items) return;

  front = rear = -1;
  count = maxSize = 0;

  delete[] items;
  items = nullptr;
}

template <typename T>
bool Queue<T>::isEmpty() {
  return front > rear || count == 0;
}

template <typename T>
void Queue<T>::enqueue(T newItem) {
  if (count == maxSize) throw full_message.c_str();

  if (rear == int(maxSize - 1)) shift();

  if (++count == 1) front = 0;
  items[++rear] = newItem;
}

template <typename T>
T Queue<T>::dequeue() {
  if (isEmpty()) throw empty_message.c_str();
  return --count, items[front++];
}

template <typename T>
T Queue<T>::frontValue() {
  if (isEmpty()) throw empty_message.c_str();
  return items[front];
}

template <typename T>
void Queue<T>::print() {
  if (isEmpty()) return;
  for (int i = front; i <= rear; i++) {
    std::cout << items[i] << " ";
  }
}

template <typename T>
void Queue<T>::shift() {
  for (int i = front; i <= rear; i++) {
    items[i - front] = items[i];
  }

  rear -= front;
  front = 0;
}
