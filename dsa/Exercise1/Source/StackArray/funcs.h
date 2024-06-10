#pragma once

void getMenuChoice(int& choice);
unsigned int get_stack_size();

template <typename StackType>
bool stack_uninitialized_handler(unsigned int& stack_size, StackType*& s);

#include "funcs.cpp"