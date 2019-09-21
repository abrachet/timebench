/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#pragma once

#include <ostream>
#include <string_view>
#include "timebench/PerfMeasure.h"

// This uses C's file I/O not C++ because there isn't an easy way to make cerr
// unbuffered but there is an easy way to make stderr unbuffered.
Error writeToStream(FILE* out, std::string_view format_string,
                    const ProcessInfo& performance);
