/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#pragma once

#include <sys/resource.h>
#include <sys/time.h>
#include "timebench/Error.h"
#include "timebench/Proc.h"

struct ProcessInfo {
  int waitstatus;
  struct rusage rusage;
  struct timeval elapsed;
};

/// Thread safe. Can measure a child process no matter how many children the
///  current process has.
Error measureProcess(ProcessInfo& info, Process& process);
