#pragma once

#include <functional>

#include "utils.hpp"

typedef bool (*ProcessComparator)(Process *, Process *);

#define scheduler_q \
  priority_queue<Process *, vector<Process *>, ProcessComparator>

using std::function;

/**
 * @brief Simulates process scheduling using various algorithms
 *
 * Simulates CPU and resource scheduling for a set of processes using different
 * scheduling algorithms. The simulation runs on a time-unit basis until all
 * processes are completed and all resources are idle. It tracks the execution
 * of processes on the CPU and two resources (R1, R2), handling process
 * transitions between CPU and I/O operations.
 *
 * @param algo The scheduling algorithm to use (FCFS, RR, SRTN, SJF)
 * @param time_quantum Time slice for the Round Robin algorithm (only used when
 * algo is RR)
 * @param procs Reference to the vector of Process pointers to be scheduled
 *
 * @details
 * The function implements:
 * - Process arrival handling
 * - CPU and resource scheduling based on priority queues
 * - Context switching between CPU and I/O operations
 * - Preemption for Round Robin (time quantum expiration) and SRTN (better
 * process arrival)
 * - Timeline tracking for visualization
 *
 * Scheduling algorithms supported:
 * - FCFS (First Come First Served): Processes are scheduled in arrival order
 * - RR (Round Robin): Time-sliced execution with preemption after quantum
 * expires
 * - SRTN (Shortest Remaining Time Next): Preemptive SJF with dynamic remaining
 * time evaluation
 * - SJF (Shortest Job First): Non-preemptive scheduling based on burst time
 */
void simulate(AlgoType algo, const int time_quantum, vector<Process *> &procs);

/**
 * @brief Computes the turnaround and waiting times for a collection of
 * processes
 *
 * This function calculates two important metrics for each process:
 * - Turnaround time: The total time from process arrival to completion
 * - Waiting time: The time a process spends neither using CPU nor performing
 * I/O operations
 *
 * @param procs Vector containing process information with arrival, finish
 * times, and burst details
 * @param tat Output vector to store calculated turnaround times (will be
 * appended to)
 * @param wait Output vector to store calculated waiting times (will be appended
 * to)
 *
 * @note The function does not clear the output vectors before adding new values
 */
void compute_times(const vector<Process *> &procs, vector<int> &tat,
                   vector<int> &wait);
