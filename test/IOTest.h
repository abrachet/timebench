/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#pragma once

#include <gtest/gtest.h>

class IORedirectTest : public ::testing::Test {
 protected:
  char stdout_template[7];
  char stdout_buf[256];
  int stdoutfd;

  void SetUp() override {
    memcpy(stdout_template, "XXXXXX", 7);
    memset(stdout_buf, 0, 256);
    stdoutfd = mkstemp(stdout_template);
    ASSERT_NE(stdoutfd, -1);
    ASSERT_FALSE(errno);
  }

  void TearDown() override { remove(stdout_template); }

 public:
  int getStdoutFD() const { return stdoutfd; }

  void readStdout(const char* str) {
    ASSERT_EQ(lseek(stdoutfd, 0, SEEK_SET), 0);
    EXPECT_NE(read(getStdoutFD(), stdout_buf, 256), -1);
    EXPECT_STREQ(stdout_buf, str);
  }
};
