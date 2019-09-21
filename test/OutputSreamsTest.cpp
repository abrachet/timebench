/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#include <gtest/gtest.h>
#include "timebench/OutputStreams.h"

TEST(OutputStreams, NullfdIsOpen) {
  char buf = 0;
  ASSERT_EQ(write(nullfd(), &buf, 1), 1);
}
