#include <iostream>
#include <chrono>
#include <random>

#include "stack.h"
#include "recursive_stack.h"

using namespace std;
using namespace std::chrono;

const int n = 1e7;
const int m = 4e4;

time_point<system_clock> start, stop;

template <typename T>
void generate_stack(T& s) {
  for (int i = 0; i < m; i++) s.push(rand() % 100);
}

template <typename T>
void measure_push(T& s, const string& version) {
  start = high_resolution_clock::now();
  for (int i = 0; i < n; i++) s.push(rand() % 100);
  stop = high_resolution_clock::now();

  auto elapsed_time = duration_cast<milliseconds>(stop - start);
  cout << "\tPush with " << version << " version: " << elapsed_time.count()
       << " milliseconds\n";
}

template <typename T>
void measure_stack_copy(T& src_stack, T& dest_stack, const string& version) {
  start = high_resolution_clock::now();
  dest_stack.copyStack(src_stack);
  stop = high_resolution_clock::now();

  auto elapsed_time = duration_cast<microseconds>(stop - start);
  cout << "\tCopy stack with " << version
       << " version: " << elapsed_time.count() << " microseconds\n";
}

int main() {
  srand(time(nullptr));

  Stack<int> stack, src_stack, dest_stack;
  RecursiveStack<int> recursive_stack, src_recursive_stack,
      dest_recursive_stack;

  stack.init(n);
  recursive_stack.init(n);

  src_stack.init(m);
  src_recursive_stack.init(m);

  generate_stack(src_stack);
  generate_stack(src_recursive_stack);

  cout << "Push " << n << " elements\n";
  measure_push<Stack<int>>(stack, "loop");
  measure_push<RecursiveStack<int>>(recursive_stack, "recursive");

  cout << "Copy stack (" << m << " elements)\n";
  measure_stack_copy<Stack<int>>(src_stack, dest_stack, "loop");
  measure_stack_copy<RecursiveStack<int>>(src_recursive_stack,
                                          dest_recursive_stack, "recursive");

  stack.release();
  recursive_stack.release();

  src_stack.release();
  dest_stack.release();

  src_recursive_stack.release();
  dest_recursive_stack.release();

  return 0;
}
