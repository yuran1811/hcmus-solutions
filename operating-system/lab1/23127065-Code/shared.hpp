#pragma once

#include <map>
#include <queue>
#include <string>
#include <vector>

using std::map;
using std::priority_queue;
using std::queue;
using std::string;
using std::vector;

enum AlgoType { FCFS = 1, RR, SJF, SRTN };

struct Segment {
  bool is_cpu;  // check if using CPU or resource (I/O)
  int rid;      // resource ID, used when is_cpu == false (1 for R1, ...)
  int time;
};

struct Process {
  const int id;
  const int arrival = 0;

  // record the time this process was last placed in ready queue.
  int last_ready;

  int remain = 0;
  int waiting = 0;
  int finish_time = -1;

  int total_cpu_burst = 0;

  bool is_pending = true;
  bool from_io = false;

  // list of segments (CPU and resource bursts, alternating)
  vector<Segment *> segs;
  int seg_idx = 0;

  Process(int id, int arrival);
  ~Process();

  void add_segment(Segment *seg);

  Segment *cur_seg() const;
  Segment *next_seg();
};

struct CPUState {
  Process *proc = nullptr;

  // Track the remaining quantum for the current process when using RR
  int quantum_rem = 0;
};

inline string cpu_timeline, res_timeline1, res_timeline2;
