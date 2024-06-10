#include <iostream>
#include <chrono>
#include <random>

#include "stack.h"
#include "recursive_stack.h"

using namespace std;
using namespace std::chrono;

const int n = 1e4;
const int m = 1e8 + n;

time_point<system_clock> start, stop;
duration<double> elapsed_time;

template <typename T>
void measure_push(T& s, const string& version) {
  start = high_resolution_clock::now();
  for (int i = 0; i < n; i++) s.push(rand() % 100);
  stop = high_resolution_clock::now();
  elapsed_time = stop - start;
  cout << "\tPush with " << version << " version: " << elapsed_time.count()
       << "s\n";
}

template <typename T>
void measure_stack_copy(T& src_stack, T& dest_stack, const string& version) {
  start = high_resolution_clock::now();
  dest_stack.copyStack(src_stack);
  stop = high_resolution_clock::now();
  elapsed_time = stop - start;
  cout << "\tCopy stack with " << version
       << " version: " << elapsed_time.count() << "s\n";
}

template <typename T>
void measure_release(T& s, const string& version) {
  start = high_resolution_clock::now();
  s.release();
  stop = high_resolution_clock::now();
  elapsed_time = stop - start;
  cout << "\tRelease with " << version << " version: " << elapsed_time.count()
       << "s\n";
}

int main() {
  srand(time(nullptr));

  Stack<int> stack, src_stack, dest_stack;
  RecursiveStack<int> recursive_stack, src_recursive_stack,
      dest_recursive_stack;

  stack.init();
  recursive_stack.init();

  src_stack.init();
  src_recursive_stack.init();

  cout << "Push " << n << " elements\n";
  measure_push<Stack<int>>(stack, "loop");
  measure_push<RecursiveStack<int>>(recursive_stack, "recursive");

  cout << "Copy stack (" << m << " elements)\n";
  measure_stack_copy<Stack<int>>(src_stack, dest_stack, "loop");
  measure_stack_copy<RecursiveStack<int>>(src_recursive_stack,
                                          dest_recursive_stack, "recursive");

  cout << "Release stack " << n << " elements\n";
  measure_release<Stack<int>>(stack, "loop");
  measure_release<RecursiveStack<int>>(recursive_stack, "recursive");

  src_stack.release();
  dest_stack.release();

  src_recursive_stack.release();
  dest_recursive_stack.release();

  return 0;
}
