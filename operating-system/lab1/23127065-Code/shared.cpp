#include "shared.hpp"

Process::Process(int id, int arrival)
    : id(id), arrival(arrival), last_ready(arrival) {}

Process::~Process() {
  for (Segment *seg : segs) delete seg;
  segs.clear();
}

void Process::add_segment(Segment *seg) {
  segs.push_back(seg);
  if (seg->is_cpu) total_cpu_burst += seg->time;
}

Segment *Process::cur_seg() const {
  return seg_idx < segs.size() ? segs[seg_idx] : nullptr;
}

Segment *Process::next_seg() {
  return seg_idx + 1 < segs.size() ? segs[++seg_idx] : nullptr;
}
