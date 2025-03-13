#include "algo.hpp"

int main(int argc, char *argv[]) {
  const string executor = argv[0];
  if (argc != 3)
    return std::cerr << "Usage: " + executor + " <INPUT> <OUTPUT>\n", 1;

  const string finp = argv[1], fout = argv[2];

  AlgoType algo;
  int time_quantum;
  vector<Process *> procs;
  if (!parse_input(finp, algo, time_quantum, procs)) return 1;

  simulate(algo, time_quantum, procs);

  vector<int> turn_around, waiting;
  compute_times(procs, turn_around, waiting);
  export_result(fout, cpu_timeline, turn_around, waiting);

  for (Process *p : procs) delete p;

  return 0;
}
