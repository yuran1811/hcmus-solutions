#pragma once

template <typename T>
struct Queue {
  T* items = nullptr;
  int front, rear;
  unsigned int count, maxSize;

  const std::string full_message =
      "Queue is full! Please dequeue before enqueueing";
  const std::string empty_message =
      "Queue is empty! Please enqueue before dequeuing";

  void init(unsigned int queueSize);
  void copyQueue(const Queue<T>& q);
  void release();

  bool isEmpty();
  void enqueue(T newItem);
  T dequeue();
  T frontValue();

  void print();

  void shift();
};

#include "queue.cpp"