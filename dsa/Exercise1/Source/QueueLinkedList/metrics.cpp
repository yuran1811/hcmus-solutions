#include <iostream>
#include <chrono>
#include <random>

#include "queue.h"
#include "recursive_queue.h"

using namespace std;
using namespace std::chrono;

const int n = 4e4;

time_point<system_clock> start, stop;

template <typename T>
void measure_enqueue(T& queue, const string& version) {
  start = high_resolution_clock::now();
  for (int i = 0; i < n; i++) queue.enqueue(rand() % 100);
  stop = high_resolution_clock::now();

  auto elapsed_time = duration_cast<microseconds>(stop - start);
  cout << "\tEnqueue with " << version << " version: " << elapsed_time.count()
       << " microseconds\n";
}

template <typename T>
void measure_release(T& q, const string& version) {
  start = high_resolution_clock::now();
  q.release();
  stop = high_resolution_clock::now();

  auto elapsed_time = duration_cast<microseconds>(stop - start);
  cout << "\tRelease with " << version << " version: " << elapsed_time.count()
       << " microseconds\n";
}

int main() {
  srand(time(nullptr));

  Queue<int> queue;
  RecursiveQueue<int> recursive_queue;

  queue.init();
  recursive_queue.init();

  cout << "Enqueue " << n << " elements\n";
  measure_enqueue<Queue<int>>(queue, "loop");
  measure_enqueue<RecursiveQueue<int>>(recursive_queue, "recursive");

  cout << "Release time\n";
  measure_release<Queue<int>>(queue, "loop");
  measure_release<RecursiveQueue<int>>(recursive_queue, "recursive");

  return 0;
}
