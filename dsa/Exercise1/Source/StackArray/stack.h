#pragma once

template <typename T>
struct Stack {
  T* items = nullptr;
  int top;
  unsigned int maxSize;

  const std::string full_message = "Stack is full! Please pop before pushing";
  const std::string empty_message =
      "Stack is empty! Please push before popping";

  void init(unsigned int stackSize);
  void copyStack(const Stack<T>& s);
  void release();

  bool isEmpty();
  void push(T newItem);
  T pop();
  T topValue();

  void print();
};

#include "stack.cpp"