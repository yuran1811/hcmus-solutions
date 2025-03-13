#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

#include "shared.hpp"

using std::ifstream;
using std::istringstream;
using std::ofstream;
using std::string;

/**
 * @brief Removes leading and trailing whitespace characters from a string.
 *
 * This function trims whitespace characters (spaces, tabs, carriage returns,
 * and newlines) from both the beginning and end of the input string.
 *
 * @param s The input string to be trimmed
 * @return A new string with leading and trailing whitespace removed.
 *         Returns an empty string if the input contains only whitespace.
 */
string trim(const string &s);

/**
 * @brief Parses a token string into a Segment object
 *
 * This function takes a string token and converts it into a Segment object.
 * The token can represent either a CPU burst or an I/O operation:
 *   - Pure numbers (e.g., "5") are interpreted as CPU bursts with the time
 * value
 *   - Numbers followed by parentheses with a resource identifier (e.g.,
 * "3(printer)") are interpreted as I/O operations with the time value and
 * resource ID
 *
 * Resource identifiers are mapped to numeric IDs which are stored in a static
 * map to maintain consistency across multiple calls.
 *
 * @param token The string token to parse
 * @return Segment* - a pointer to a newly allocated Segment object
 *                  (caller is responsible for memory management)
 *
 * @note The returned Segment has the following properties set:
 *       - is_cpu: true for CPU burst, false for I/O operation
 *       - rid: resource ID for I/O (-1 for CPU)
 *       - time: duration of the segment
 */
Segment *parse_token(const string &token);

/**
 * @brief Parses the input file for process scheduling simulation
 *
 * This function reads an input file containing scheduling algorithm parameters
 * and process information. The file format should be:
 * - Line 1: Algorithm number (1-4) representing scheduling algorithm type
 * - Line 2: (Only for RR algorithm) Time quantum value
 * - Line 3: Number of processes
 * - Subsequent lines: Process information with format:
 *   "arrival_time cpu_burst1 io_burst1 cpu_burst2 io_burst2 ..."
 *   where cpu_bursts are represented as "Cx" and io_bursts as "Iy"
 *   (x and y are time units)
 *
 * @param filename Path to the input file
 * @param algo Reference to store the scheduling algorithm type
 * @param time_quantum Reference to store time quantum for RR algorithm (-1 for
 * others)
 * @param procs Reference to vector for storing Process objects
 * @return true If parsing was successful
 * @return false If any error occurred during parsing
 */
bool parse_input(const string &filename, AlgoType &algo, int &time_quantum,
                 vector<Process *> &procs);

/**
 * @brief Exports scheduling simulation results to a file
 *
 * This function writes the CPU timeline, resource usage timelines (if used),
 * turn-around times and waiting times to the specified output file.
 *
 * @param filename Path to the output file
 * @param cpu_timeline String representation of CPU execution timeline
 * @param turn_around Vector of turn-around times for each process
 * @param waiting Vector of waiting times for each process
 *
 * @note The function uses global variables res_timeline1 and res_timeline2 for
 * resource timelines
 * @note Resource timelines will only be written if they contain characters
 * other than '_' and space
 */
void export_result(const string &filename, const string &cpu_timeline,
                   const vector<int> &turn_around, const vector<int> &waiting);

/**
 * @brief Comparator function for First-Come-First-Served (FCFS) process
 * scheduling
 *
 * This function compares two processes to determine their ordering in the FCFS
 * scheduling algorithm. The comparison is based on:
 * 1. last_ready time (primary criteria, in reverse order for priority queue
 * implementation)
 * 2. pending status (secondary criteria, non-pending processes have higher
 * priority)
 * 3. from_io status (tertiary criteria, processes with lower from_io values
 * have higher priority)
 *
 * @param a Pointer to the first Process to compare
 * @param b Pointer to the second Process to compare
 * @return true if process a has higher priority than process b, false otherwise
 */
bool FCFSComparator(Process *a, Process *b);

/**
 * @brief Comparator function for the Shortest Job First (SJF) scheduling
 * algorithm
 *
 * This function compares two processes for ordering in a priority queue
 * with the shortest job first policy. It primarily compares the burst time
 * of the current CPU segment of each process.
 *
 * The comparison logic is as follows:
 * 1. If burst times differ, shorter burst time gets higher priority
 * 2. If burst times are equal:
 *    a. If pending status is the same, process that came from I/O earlier gets
 * priority b. Otherwise, non-pending processes get priority
 *
 * Note: Since this is designed for a priority_queue which is a max-heap by
 * default, the logic is inverted (returns true when a should come after b)
 *
 * @param a Pointer to the first Process to compare
 * @param b Pointer to the second Process to compare
 * @return true if process a should be placed after process b in the queue
 * @return false otherwise
 */
bool SJFComparator(Process *a, Process *b);

/**
 * @brief Comparison function for Shortest Remaining Time Next (SRTN) scheduling
 * algorithm.
 *
 * This function is designed to be used with a priority_queue to implement the
 * SRTN scheduling algorithm. It compares two processes based on their remaining
 * execution time and other factors. Note that it's designed for a max-heap
 * priority_queue, so the comparison logic is inverted (returns true when a
 * should come after b).
 *
 * @param a Pointer to the first Process object to compare
 * @param b Pointer to the second Process object to compare
 *
 * @return true if process a should be lower priority than process b in the
 * queue
 * @return false if process a should be higher priority than or equal to process
 * b
 *
 * Comparison logic:
 * 1. Processes with shorter remaining time get higher priority
 * 2. If remaining times are equal, non-pending processes get higher priority
 * 3. If both are pending or both are non-pending, the process that came from
 * I/O earlier gets higher priority
 */
bool SRTNComparator(Process *a, Process *b);