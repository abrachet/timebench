/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#include "timebench/PerfMeasure.h"
#include "timebench/SyscallAssert.h"

Error measureProcess(ProcessInfo& info, Process& process) {
  struct timeval start;
  syscall_assert(gettimeofday(&start, nullptr));

  pid_t pid = process.run();

  if (wait4(pid, &info.waitstatus, 0, &info.rusage) == -1)
    return Error::fatal("Couldn't wait on child");

  if (int exit = WEXITSTATUS(info.waitstatus))
    return Error::fatal(strerror(exit));

  syscall_assert(gettimeofday(&info.elapsed, nullptr));
  info.elapsed.tv_sec -= start.tv_sec;
  if (info.elapsed.tv_usec < start.tv_usec) {
    info.elapsed.tv_usec += 1000000;
    info.elapsed.tv_sec -= 1;
  }
  info.elapsed.tv_usec -= start.tv_usec;

  return Error::success();
}
