#include "utils.hpp"

string trim(const string &s) {
  size_t beg = s.find_first_not_of(" \t\r\n");
  size_t end = s.find_last_not_of(" \t\r\n");

  return (beg == string::npos) ? "" : s.substr(beg, end - beg + 1);
}

Segment *parse_token(const string &token) {
  static map<string, bool> res_map;

  Segment *seg = new Segment();

  size_t parentheses_start = token.find('(');
  if (parentheses_start != string::npos) {
    // Mark as I/O segment
    seg->is_cpu = false;

    // Determine the resource id
    size_t beg = parentheses_start + 1;
    size_t end = token.find(')', beg);
    string res_token = token.substr(beg, end - beg);
    if (res_map.find(res_token) == res_map.end())
      res_map.emplace(res_token, res_map.size());

    seg->rid = res_map[res_token];

    // Extract number before '('
    seg->time = stoi(token.substr(0, parentheses_start));
  } else {
    // CPU burst segment
    seg->is_cpu = true;
    seg->rid = -1;
    seg->time = stoi(token);
  }

  return seg;
}

bool parse_input(const string &filename, AlgoType &algo, int &time_quantum,
                 vector<Process *> &procs) {
  ifstream fin(filename);
  if (!fin.is_open()) return std::cerr << "Cannot open input file!\n", false;

  // First line: scheduling algorithm number (1-4)
  int algo_idx = -1;
  fin >> algo_idx;
  if (algo_idx < 1 || algo_idx > 4)
    return std::cerr << "Invalid algorithm number!\n", false;

  algo = static_cast<AlgoType>(algo_idx);

  // For RR, next line is time quantum
  if (algo == RR) {
    fin >> time_quantum;
    if (time_quantum < 1) return std::cerr << "Invalid time quantum!\n", false;
  } else
    time_quantum = -1;

  // Next line: number of processes
  int num_proc = -1;
  fin >> num_proc;
  if (num_proc < 1) return std::cerr << "Invalid number of processes.\n", false;

  // Read process information
  procs.clear();
  procs.reserve(num_proc);
  fin.ignore();
  for (int i = 0; i < num_proc; i++) {
    string line = "";
    if (!getline(fin, line)) break;

    line = trim(line);
    if (line.empty()) continue;

    // Tokenize line
    istringstream iss(line);
    vector<string> tokens;
    string token;
    while (iss >> token) tokens.push_back(token);
    if (tokens.empty()) continue;

    // Parse tokens
    Process *p = new Process(i, stoi(tokens[0]));
    for (size_t j = 1; j < tokens.size(); j++)
      p->add_segment(parse_token(tokens[j]));
    if (p->segs.size()) p->remain = p->segs[0]->time;

    procs.push_back(p);
  }

  return fin.close(), true;
}

void export_result(const string &filename, const string &cpu_timeline,
                   const vector<int> &turn_around, const vector<int> &waiting) {
  ofstream fout(filename);
  if (!fout.is_open())
    return void(std::cerr << "Error: cannot open output file.\n");

  fout << cpu_timeline << "\n";

  // Only output resource timelines if there was any usage.
  if (res_timeline1.find_first_not_of("_ ") != string::npos)
    fout << res_timeline1 << "\n";
  if (res_timeline2.find_first_not_of("_ ") != string::npos)
    fout << res_timeline2 << "\n";

  for (const int &t : turn_around) fout << t << " ";
  fout << "\n";
  for (const int &w : waiting) fout << w << " ";
  fout << "\n";

  fout.close();
}

bool FCFSComparator(Process *a, Process *b) {
  if (a->last_ready == b->last_ready) {
    if (a->is_pending == b->is_pending) {
      return a->from_io < b->from_io;
    }

    return a->is_pending < b->is_pending;
  }

  return a->last_ready > b->last_ready;
}

bool SJFComparator(Process *a, Process *b) {
  int time_a = a->segs[a->seg_idx]->time;
  int time_b = b->segs[b->seg_idx]->time;

  if (time_a == time_b) {
    if (a->is_pending == b->is_pending) {
      return a->from_io < b->from_io;
    }

    return a->is_pending < b->is_pending;
  }

  return time_a > time_b;
}

bool SRTNComparator(Process *a, Process *b) {
  int remain_a = a->remain;
  int remain_b = b->remain;

  if (remain_a == remain_b) {
    if (a->is_pending == b->is_pending) {
      return a->from_io < b->from_io;
    }

    return a->is_pending < b->is_pending;
  }

  return remain_a > remain_b;
}
