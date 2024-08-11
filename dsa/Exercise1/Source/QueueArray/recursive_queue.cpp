#include <iostream>
#include "recursive_queue.h"

template <typename T>
void RecursiveQueue<T>::init(unsigned int queueSize) {
  front = rear = -1;
  count = 0;
  maxSize = queueSize;
  items = new T[queueSize];
}

template <typename T>
void RecursiveQueue<T>::recurCopyQueue(const RecursiveQueue<T>& q, int i,
                                       int n) {
  if (i == n) return;
  items[i] = q.items[i];
  recurCopyQueue(q, i + 1, n);
}

template <typename T>
void RecursiveQueue<T>::copyQueue(const RecursiveQueue<T>& q) {
  release();

  front = q.front;
  rear = q.rear;
  count = q.count;
  maxSize = q.maxSize;
  items = new T[maxSize];

  recurCopyQueue(q, front, rear);
}

template <typename T>
void RecursiveQueue<T>::release() {
  if (!items) return;

  front = rear = -1;
  count = maxSize = 0;

  delete[] items;
  items = nullptr;
}

template <typename T>
bool RecursiveQueue<T>::isEmpty() {
  return front > rear || count == 0;
}

template <typename T>
void RecursiveQueue<T>::enqueue(T newItem) {
  if (count == maxSize) throw full_message.c_str();

  if (rear == int(maxSize - 1)) shift();

  if (++count == 1) front = 0;
  items[++rear] = newItem;
}

template <typename T>
T RecursiveQueue<T>::dequeue() {
  if (isEmpty()) throw empty_message.c_str();
  return --count, items[front++];
}

template <typename T>
T RecursiveQueue<T>::frontValue() {
  if (isEmpty()) throw empty_message.c_str();
  return items[front];
}

template <typename T>
void RecursiveQueue<T>::recurPrint(int i) {
  if (i > rear) return;
  std::cout << items[i] << " ";
  recurPrint(i + 1);
}

template <typename T>
void RecursiveQueue<T>::print() {
  if (isEmpty()) return;
  recurPrint(front);
}

template <typename T>
void RecursiveQueue<T>::recurShift(int i) {
  if (i > rear) return;
  items[i - front] = items[i];
  recurShift(i + 1);
}

template <typename T>
void RecursiveQueue<T>::shift() {
  recurShift(front);

  rear -= front;
  front = 0;
}
