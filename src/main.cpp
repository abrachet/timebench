/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#include <cstring>
#include <iostream>
#include "Options.h"
#include "timebench/Formatting.h"
#include "timebench/OutputStreams.h"
#include "timebench/PerfMeasure.h"
#include "timebench/Proc.h"
#include "timebench/TestRunner.h"

const char* progname;
bool timebench = true;  // if progname is "timebench"

constexpr auto Red = "\u001b[31;1m";
constexpr auto Reset = "\u001b[0m";

static void assignProgname(const char* name) {
  for (size_t i = std::strlen(name); i; --i)
    if (name[i] == '/') {
      progname = name + i + 1;
      return;
    }
  progname = name;
  timebench = !strcmp(progname, "timebench");
}

static int handleExitingOptions(const TimebenchOptions& options) {
  if (options.help) {
    printHelp(std::cerr);
    return 0;
  }

  if (options.version) {
    std::cerr << progname << ": version 0.1\nAlex Brachet\n";
    return 0;
  }

  if (options.unrecognized_options.size() > 0) {
    for (const auto& str : options.unrecognized_options)
      std::cerr << progname << ':' << Red << " error: " << Reset
                << "Unkown option: '" << str << "'\n";
    return 1;
  }

  if (options.append && options.output == "") {
    std::cerr << "appened option is meanigless when writing to stderr\n";
    return 1;
  }

  return -1;
}

std::vector<ProcessInfo> runTests(const TimebenchOptions& options,
                                  std::function<void(Error)> error) {
  ProcessBuilder p(options.command);
  p.stderr(nullfd());
  if (timebench) {
    p.stdout(nullfd());
    p.stderr(nullfd());
  }

  TestRunner testRunner(options.runs, p);
  if (Error e = testRunner.warmup(options.warmup)) error(std::move(e));

  if (Error e = testRunner.runTests()) error(std::move(e));

  return testRunner.getAllRuns();
}

int handleOutput(const TimebenchOptions& options,
                 const std::vector<ProcessInfo>& runs) {
  FILE* out;
  if (!options.output.size()) {
    out = stderr;
    // make stderr buffered, writeToStream writes one char at a time.
    char buf[BUFSIZ];
    setbuf(out, buf);
  } else {
    out = fopen(options.output.c_str(), options.append ? "a+" : "w+");
    assert(out);
  }

  if (options.average) {
    ProcessInfo info = TestRunner::getAverages(runs);
    Error e = writeToStream(out, options.format, info);
    if (e) {
      if (e.isFatal()) {
        std::cerr << e.what() << ".\n";
        return 1;
      }

      // e is a warning
      if (options.verbose) {
        std::cerr << e.what();
      }
    }
    return 0;
  }

  for (const ProcessInfo& info : runs) {
    Error e = writeToStream(out, options.format, info);
    if (e) {
      if (e.isFatal()) {
        std::cerr << e.what() << ".\n";
        return 1;
      }

      // e is a warning
      if (options.verbose) {
        std::cerr << e.what();
        return 1;
      }
    }
    fputc('\n', out);
  }

  fclose(out);
  stderr = fdopen(2, "w");
  return 0;
}

int main(int argc, const char** argv) {
  assignProgname(argv[0]);

  TimebenchOptions options = parseTimeBench(argc, argv);
  if (int exit_status = handleExitingOptions(options) != -1) return exit_status;

  std::vector<ProcessInfo> runs = runTests(options, [](Error e) {
    std::cerr << e.what() << '\n';
    exit(1);
  });

  return handleOutput(options, runs);
}
