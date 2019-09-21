/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#include "timebench/TestRunner.h"
#include <sys/wait.h>
#include "timebench/OutputStreams.h"

Error TestRunner::warmup(unsigned num) const {
  for (int64_t i = 0; i < num; ++i) {
    Process p = pb.build(-1, nullfd(), nullfd());
    pid_t pid = p.run();
    if (pid == -1) return Error::warn("couldn't run command");
    int status;
    (void)waitpid(pid, &status, 0);
    if (WEXITSTATUS(status) == 127) return Error::fatal("command not found");
  }

  return Error::success();
}

Error TestRunner::runTests() {
  size_t num_runs = runs.size();
  for (int64_t i = 0; i < num_runs; ++i) {
    Process p = pb.build();
    if (Error E = measureProcess(runs[i], p)) return E;
  }

  return Error::success();
}

ProcessInfo TestRunner::getAverages(const std::vector<ProcessInfo>& runs) {
  ProcessInfo p;
  std::memset(&p, 0, sizeof(ProcessInfo));

  // add sizeof int to skip wait status;
  constexpr size_t num_longs =
      (sizeof(ProcessInfo) - sizeof(int)) / sizeof(long);
  long* averages =
      reinterpret_cast<long*>(reinterpret_cast<char*>(&p) + sizeof(int));
  for (const ProcessInfo& info : runs) {
    p.waitstatus += info.waitstatus;
    const long* pinfo = reinterpret_cast<const long*>(
        reinterpret_cast<const char*>(&info) + sizeof(int));
    for (int i = 0; i < num_longs; ++i) averages[i] += pinfo[i];
  }

  for (int i = 0; i < num_longs; ++i) averages[i] /= runs.size();

  return p;
}
