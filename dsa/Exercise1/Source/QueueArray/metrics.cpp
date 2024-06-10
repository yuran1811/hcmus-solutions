#include <iostream>
#include <chrono>
#include <random>

#include "queue.h"
#include "recursive_queue.h"

using namespace std;
using namespace std::chrono;

const int n = 1e7;
const int m = 1e8;

time_point<system_clock> start, stop;
duration<double> elapsed_time;

template <typename T>
void measure_enqueue(T& queue, const string& version) {
  start = high_resolution_clock::now();
  for (int i = 0; i < n; i++) queue.enqueue(rand() % 100);
  stop = high_resolution_clock::now();
  elapsed_time = stop - start;
  cout << "\tEnqueue with " << version << " version: " << elapsed_time.count()
       << "s\n";
}

template <typename T>
void measure_queue_copy(T& src_queue, T& dest_queue, const string& version) {
  start = high_resolution_clock::now();
  dest_queue.copyQueue(src_queue);
  stop = high_resolution_clock::now();
  elapsed_time = stop - start;
  cout << "\tCopy queue with " << version
       << " version: " << elapsed_time.count() << "s\n";
}

int main() {
  srand(time(nullptr));

  Queue<int> queue, src_queue, dest_queue;
  RecursiveQueue<int> recursive_queue, src_recursive_queue,
      dest_recursive_queue;

  queue.init(n);
  recursive_queue.init(n);

  src_queue.init(m);
  src_recursive_queue.init(m);

  cout << "Enqueue " << n << " elements\n";
  measure_enqueue<Queue<int>>(queue, "loop");
  measure_enqueue<RecursiveQueue<int>>(recursive_queue, "recursive");

  cout << "Copy queue (" << m << " elements)\n";
  measure_queue_copy<Queue<int>>(src_queue, dest_queue, "loop");
  measure_queue_copy<RecursiveQueue<int>>(src_recursive_queue,
                                          dest_recursive_queue, "recursive");

  queue.release();
  recursive_queue.release();

  src_queue.release();
  dest_queue.release();

  src_recursive_queue.release();
  dest_recursive_queue.release();

  return 0;
}
