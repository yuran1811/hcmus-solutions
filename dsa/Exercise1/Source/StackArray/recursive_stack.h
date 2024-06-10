#pragma once

template <typename T>
struct RecursiveStack {
  T* items = nullptr;
  int top;
  unsigned int maxSize;

  const std::string full_message = "Stack is full! Please pop before pushing";
  const std::string empty_message =
      "Stack is empty! Please push before popping";

  void init(unsigned int stackSize);

  void recurCopyStack(const RecursiveStack<T>& s, int i, int n);
  void copyStack(const RecursiveStack<T>& s);

  void release();

  bool isEmpty();
  void push(T newItem);
  T pop();
  T topValue();

  void recurPrint(int i);
  void print();
};

#include "recursive_stack.cpp"