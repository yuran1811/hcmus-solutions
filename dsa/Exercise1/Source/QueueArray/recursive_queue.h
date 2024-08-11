#pragma once

template <typename T>
struct RecursiveQueue {
  T* items = nullptr;
  int front, rear;
  unsigned int count, maxSize;

  const std::string full_message =
      "Queue is full! Please dequeue before enqueueing";
  const std::string empty_message =
      "Queue is empty! Please enqueue before dequeuing";

  void init(unsigned int queueSize);

  void recurCopyQueue(const RecursiveQueue<T>& q, int i, int n);
  void copyQueue(const RecursiveQueue<T>& q);

  void release();

  bool isEmpty();
  void enqueue(T newItem);
  T dequeue();
  T frontValue();

  void recurPrint(int i);
  void print();

  void recurShift(int i);
  void shift();
};

#include "recursive_queue.cpp"