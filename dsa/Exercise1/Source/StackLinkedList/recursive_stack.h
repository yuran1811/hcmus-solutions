#pragma once

template <typename T>
struct RecursiveStack {
  struct Node {
    T data;
    Node* next = nullptr;
  }* top = nullptr;

  const std::string full_message = "Stack is full! Please pop before pushing";
  const std::string empty_message =
      "Stack is empty! Please push before popping";

  void init();

  void recurCopyStack(const RecursiveStack<T>& s, Node* p);
  void copyStack(const RecursiveStack<T>& s);

  void recurRelease(Node* p);
  void release();

  bool isEmpty();
  void push(T newItem);
  T pop();
  T topValue();

  void recurPrint(Node* p);
  void print();
};

#include "recursive_stack.cpp"