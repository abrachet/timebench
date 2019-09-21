/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#include <gtest/gtest.h>
#include "IOTest.h"
#include "timebench/OutputStreams.h"
#include "timebench/PerfMeasure.h"
#include "timebench/Proc.h"

// Test that the measured time is close, of course we can't ever be sure
// how much time a process will get.
TEST(PerfMeasure, RoughlyCorrect) {
  ProcessBuilder sleep1("sleep 1");
  Process p = sleep1.build();
  ProcessInfo info;
  Error e = measureProcess(info, p);
  ASSERT_FALSE(e) << "measureProcess failed: " << e.what()
                  << ". Errno is: " << strerror(errno);

  EXPECT_TRUE(1 <= info.elapsed.tv_sec && info.elapsed.tv_sec <= 2);
  // EXPECT_TRUE(info.rusage.ru_nvcsw >= 1);
}

// TODO: come up with better ways to test this. The problem is we can never be
// sure what a process does and doesn't do. It isn't even always true that
// `sleep 1` performs a voluntary context switch, which is crazy.

#if 0
TEST(PerfMeasure, RusageIO) {
  auto doesio = ProcessBuilder("strings `which strings`").stdout(nullfd()).shell();

  ProcessInfo info;
  Error e = measureProcess(info, doesio);
  ASSERT_FALSE(e) << "measureProcess failed: " << e.what();

  EXPECT_EQ(info.rusage.ru_inblock, 1);
}

TEST_F(IORedirectTest, RusageOut) {
  ProcessInfo info;
  auto echo = ProcessBuilder("echo hello").stdout(getStdoutFD());
  Error e1 = measureProcess(info, echo);
  ASSERT_FALSE(e1) << "measureProcess failed: " << e1.what();

  // Only needed to call write once.
  ASSERT_EQ(info.rusage.ru_oublock, 1);
}
#endif
