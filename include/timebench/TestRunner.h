/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#pragma once

#include "timebench/Error.h"
#include "timebench/PerfMeasure.h"
#include "timebench/Proc.h"

class TestRunner {
  std::vector<ProcessInfo> runs;
  const ProcessBuilder& pb;

 public:
  TestRunner(unsigned runs, const ProcessBuilder& pb) : runs(runs), pb(pb) {}

  Error warmup(unsigned num) const;
  Error runTests();

  const std::vector<ProcessInfo>& getAllRuns() const { return runs; }
  static ProcessInfo getAverages(const std::vector<ProcessInfo>&);
};
