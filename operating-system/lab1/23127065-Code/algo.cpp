#include "algo.hpp"

void proc_schedule(Process *&proc, scheduler_q &readyq, bool waiting = false,
                   const int time = 0) {
  if (readyq.empty()) return;

  proc = readyq.top();
  readyq.pop();

  proc->waiting += waiting ? time - proc->last_ready : 0;
}

void proc_handle(Process *&proc, scheduler_q &readyq, scheduler_q &res1q,
                 scheduler_q &res2q, string &rtimeline, const int time) {
  string timeline_seg = "_ ";
  if (proc) {
    timeline_seg[0] = '1' + proc->id;

    // Check if current burst just finished or the quantum expired when using
    // RR.
    proc->is_pending = false;
    if (--proc->remain == 0) {
      // Current burst finished. Then move to next segment.
      Segment *next_seg = proc->next_seg();
      if (next_seg) {
        proc->remain = next_seg->time;
        proc->from_io = true;

        if (!next_seg->is_cpu) {
          // Next segment is I/O burst.
          if (!next_seg->rid)
            res1q.push(proc);
          else
            res2q.push(proc);
        } else {
          // Next segment is CPU burst.
          proc->last_ready = time + 1;
          readyq.push(proc);
        }
      } else
        proc->finish_time = time + 1;

      proc = nullptr;
    }
  }
  rtimeline += timeline_seg;
}

void simulate(AlgoType algo, const int time_quantum, vector<Process *> &procs) {
  CPUState cpu_state{nullptr, time_quantum};
  Process *r1_proc = nullptr, *r2_proc = nullptr;

  scheduler_q readyq(algo == FCFS || algo == RR ? FCFSComparator
                     : algo == SRTN             ? SRTNComparator
                                                : SJFComparator),
      res1q(FCFSComparator), res2q(FCFSComparator);

  // Simulation continues until all procs are finished and CPU, res are idle.
  for (int time = 0; !time || cpu_state.proc || r1_proc || r2_proc ||
                     readyq.size() || res1q.size() || res2q.size();
       time++) {
    for (Process *p : procs)
      if (p->arrival == time) readyq.push(p);

    // Schedule processes if CPU, resources are idle.
    if (!cpu_state.proc) proc_schedule(cpu_state.proc, readyq, true, time);
    if (!r1_proc) proc_schedule(r1_proc, res1q);
    if (!r2_proc) proc_schedule(r2_proc, res2q);

    // CPU Processing
    proc_handle(cpu_state.proc, readyq, res1q, res2q, cpu_timeline, time);

    if (algo == RR) {
      if (cpu_state.proc) {
        if (--cpu_state.quantum_rem == 0) {
          cpu_state.quantum_rem = time_quantum;

          // Round Robin quantum expired; preempt process.
          cpu_state.proc->last_ready = time + 1;
          readyq.push(cpu_state.proc);

          cpu_state.proc = nullptr;
        }
      } else
        // If starting a new CPU burst, the remain should already be set.
        cpu_state.quantum_rem = time_quantum;
    } else if (algo == SRTN) {
      if (cpu_state.proc) {
        // Preempt current process
        cpu_state.proc->last_ready = time + 1;
        readyq.push(cpu_state.proc);
      }

      cpu_state.proc = nullptr;
    }

    // Resource Processing
    proc_handle(r1_proc, readyq, res1q, res2q, res_timeline1, time);
    proc_handle(r2_proc, readyq, res1q, res2q, res_timeline2, time);
  }
}
