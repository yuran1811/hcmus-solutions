#pragma once

template <typename T>
struct Queue {
  struct Node {
    T data;
    Node* next;
  } *front, *rear;

  const std::string full_message =
      "Queue is full! Please dequeue before enqueueing";
  const std::string empty_message =
      "Queue is empty! Please enqueue before dequeuing";

  void init();
  void release();

  bool isEmpty();
  void enqueue(T newItem);
  T dequeue();
  T frontValue();

  void print();
};

#include "queue.cpp"