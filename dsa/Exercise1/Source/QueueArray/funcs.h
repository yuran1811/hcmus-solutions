#pragma once

void getMenuChoice(int& choice);
unsigned int get_queue_size();

template <typename QueueType>
bool queue_uninitialized_handler(unsigned int& queue_size, QueueType*& s);

#include "funcs.cpp"