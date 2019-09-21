/**
 *  Alex Brachet (abrachet@cs.purdue.edu)
 */

#include <unistd.h>
#include <iostream>
#include <string_view>
#include "timebench/PerfMeasure.h"

constexpr int minute = 60;
constexpr int hour = 60 * minute;

/// Man page of time(1) says it is [hours:]minutes:seconds, but it seems to be
/// actually doing hours:minutes.seconds. This follows GNU time 1.7's actual
/// output, not the man page.
static void hms(FILE* out, struct timeval elapsed) {
  unsigned hours = elapsed.tv_sec / hour;
  fprintf(out, "%u:", hours);
  elapsed.tv_sec -= hours * hour;

  unsigned minutes = elapsed.tv_sec / minute;
  fprintf(out, "%.02u.", minutes);
  elapsed.tv_sec -= minutes * minute;

  assert(elapsed.tv_sec < 60);

  fprintf(out, "%.02ld", elapsed.tv_sec);
}

/// Prints in seconds.milliseconds.
static void sec(FILE* out, const struct timeval& elapsed) {
  fprintf(out, "%ld.%02u", elapsed.tv_sec, elapsed.tv_usec / 10000);
}

struct timeval operator+(struct timeval a, const struct timeval& b) {
  a.tv_sec += b.tv_sec;
  a.tv_usec += b.tv_usec;
  if (int secs = a.tv_usec / 1e9) {
    a.tv_usec -= secs * 1e9;
    a.tv_sec += secs;
  }

  return a;
}

unsigned operator/(struct timeval a, const struct timeval& b) {
  return !b.tv_sec ? 0 : a.tv_sec / b.tv_sec;
}

Error writeToStream(FILE* out, std::string_view format_string,
                    const ProcessInfo& performance) {
  const struct rusage& r = performance.rusage;

  const auto end = format_string.end();
  for (auto current = format_string.begin(); current != end; ++current) {
    char c = *current;
    switch (c) {
      case '%':
        ++current;
        switch (*current) {
          default:
            return Error::fatal(std::string("unkown option: '%") + *current +
                                '\'');
          case '%':
            fputc('%', out);
            break;

            // Time related output

          case 'E':
            hms(out, performance.elapsed);
            break;
          case 'e':
            sec(out, performance.elapsed);
            break;
          case 'S':
            sec(out, r.ru_stime);
            break;
          case 'U':
            sec(out, r.ru_utime);
            break;
          case 'P':  // (%U + %S) / %E
            // TODO: this is completly broken...
            fprintf(out, "%u%%",
                    ((r.ru_stime + r.ru_utime) / performance.elapsed) * 100);
            break;

            // Memory related output

          case 'M':  // in Kb
            fprintf(out, "%lu", r.ru_maxrss);
            break;

          case 't':
            return Error::warn("%t is currently not supported");
            break;

          case 'K':
            fprintf(out, "%lu", r.ru_ixrss + r.ru_idrss + r.ru_isrss);
            break;

          case 'D':
            fprintf(out, "%lu", r.ru_idrss + r.ru_isrss);
            break;

          case 'p':
            fprintf(out, "%lu", r.ru_isrss);
            break;

          case 'X':
            fprintf(out, "%lu", r.ru_ixrss);
            break;

          case 'Z':
            fprintf(out, "%d", getpagesize());
            break;

          case 'F':
            fprintf(out, "%ld", r.ru_majflt);
            break;

          case 'R':
            fprintf(out, "%ld", r.ru_minflt);
            break;

          case 'W':
            fprintf(out, "%ld", r.ru_nswap);
            break;

          case 'c':
            fprintf(out, "%ld", r.ru_nivcsw);
            break;

          case 'w':
            fprintf(out, "%ld", r.ru_nvcsw);
            break;

            // I/O related output
        }
        break;

      case '\\':
        current++;
        switch (*current) {
          case '\\':
            fputc('\\', out);
            break;
          case 'n':
            fputc('\n', out);
            break;
          case '\t':
            fputc('\t', out);
            break;
        }
        break;

      default:
        fputc(c, out);
    }
  }

  return Error::success();
}
