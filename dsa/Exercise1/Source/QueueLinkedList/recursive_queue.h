#pragma once

template <typename T>
struct RecursiveQueue {
  struct Node {
    T data;
    Node *next;
  } *front = nullptr, *rear = nullptr;

  const std::string full_message =
      "Queue is full! Please dequeue before enqueueing";
  const std::string empty_message =
      "Queue is empty! Please enqueue before dequeuing";

  void init();

  void recurRelease(Node *p);
  void release();

  bool isEmpty();
  void enqueue(T newItem);
  T dequeue();
  T frontValue();

  void recurPrint(Node *p);
  void print();
};

#include "recursive_queue.cpp"