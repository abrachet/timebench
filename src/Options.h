/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#pragma once

#include <string>
#include <vector>

struct TimebenchOptions {
  bool version = false;
  bool help = false;
  bool verbose = false;

  std::vector<std::string> command;

  std::string format = "\nreal    %e\nuser    %U\nsys     %S\n";

  // Number of warmup runs to execute before benchmarking.
  unsigned warmup = 0;
  unsigned runs = 1;

  std::string output;

  bool append = false;
  bool average = false;

  std::vector<std::string> unrecognized_options;
};

TimebenchOptions parseTimeBench(int argc, const char** argv);

void printHelp(std::ostream& out);
