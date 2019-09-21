/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/wait.h>
#include <cstdio>
#include "IOTest.h"
#include "timebench/Proc.h"

TEST(ProcTest, RunProg) {
  ProcessBuilder falseproc("false");
  pid_t pid = falseproc.build().run();
  int status;
  ASSERT_EQ(wait(&status), pid);

  EXPECT_TRUE(WEXITSTATUS(status));

  ProcessBuilder trueproc("true");
  pid = trueproc.build().run();
  ASSERT_EQ(wait(&status), pid);
  EXPECT_EQ(WEXITSTATUS(status), 0);
}

TEST(ProcTest, DoesntExist) {
  ProcessBuilder p("this executable doesn't exist, hopefully");
  pid_t pid = p.build().run();
  int status;
  ASSERT_EQ(wait(&status), pid);
  EXPECT_EQ(WEXITSTATUS(status), 127);
}

TEST_F(IORedirectTest, BasicEcho) {
  auto echo = ProcessBuilder("echo hello").stdout(getStdoutFD());
  pid_t pid = echo.build().run();
  int status;
  ASSERT_EQ(wait(&status), pid);
  readStdout("hello\n");
}

TEST_F(IORedirectTest, MultiArgEcho) {
  auto echo = ProcessBuilder("echo hello world").stdout(getStdoutFD());
  pid_t pid = echo.build().run();
  int status;
  ASSERT_EQ(wait(&status), pid);
  readStdout("hello world\n");
}
