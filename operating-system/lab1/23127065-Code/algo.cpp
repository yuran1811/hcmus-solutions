#include "algo.hpp"

void schedule_proc(Process *&proc, scheduler_q &readyq, bool waiting = false,
                   const int time = 0) {
  if (readyq.empty()) return;

  proc = readyq.top();
  readyq.pop();

  proc->waiting += waiting ? time - proc->last_ready : 0;
}

void res_handler(Process *&rproc, scheduler_q &readyq, scheduler_q &res1q,
                 scheduler_q &res2q, string &rtimeline, const int time) {
  string timeline_seg = "_ ";
  if (rproc) {
    timeline_seg[0] = '1' + rproc->id;

    if (--rproc->remain == 0) {
      // Resource burst finished for process rproc.
      Segment *next_seg = rproc->next_seg();
      if (next_seg) {
        rproc->remain = next_seg->time;
        rproc->from_io = true;

        if (!next_seg->is_cpu) {
          // Next segment is I/O burst.
          if (!next_seg->rid)
            res1q.push(rproc);
          else
            res2q.push(rproc);
        } else {
          // Next segment is CPU burst.
          rproc->last_ready = time + 1;
          readyq.push(rproc);
        }
      } else
        rproc->finish_time = time + 1;

      rproc = nullptr;
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
    if (!cpu_state.proc) schedule_proc(cpu_state.proc, readyq, true, time);
    if (!r1_proc) schedule_proc(r1_proc, res1q);
    if (!r2_proc) schedule_proc(r2_proc, res2q);

    // CPU Processing
    string cpuChar = "_ ";
    if (cpu_state.proc) {
      cpuChar[0] = '1' + cpu_state.proc->id;

      // Check if CPU burst just finished or the quantum expired when using RR.
      cpu_state.proc->is_pending = false;
      if (--cpu_state.proc->remain == 0) {
        // CPU segment finished. Then move to next segment.
        Segment *next_seg = cpu_state.proc->next_seg();
        if (next_seg) {
          cpu_state.proc->remain = next_seg->time;

          if (!next_seg->is_cpu) {
            cpu_state.proc->from_io = false;
            // from CPU to I/O burst
            if (!next_seg->rid)
              res1q.push(cpu_state.proc);
            else
              res2q.push(cpu_state.proc);
          } else {
            cpu_state.proc->last_ready = time + 1;
            readyq.push(cpu_state.proc);
          }
        } else
          cpu_state.proc->finish_time = time + 1;

        cpu_state.proc = nullptr;
      }
    }
    cpu_timeline += cpuChar;

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
    res_handler(r1_proc, readyq, res1q, res2q, res_timeline1, time);
    res_handler(r2_proc, readyq, res1q, res2q, res_timeline2, time);
  }
}

void compute_times(const vector<Process *> &procs, vector<int> &tat,
                   vector<int> &wait) {
  for (Process *p : procs) {
    tat.push_back(p->finish_time - p->arrival);
    wait.push_back(p->waiting);
  }
}
