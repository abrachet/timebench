/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#include <gtest/gtest.h>
#include "timebench/Formatting.h"

class FormattingTest : public ::testing::Test {
  char buf_[4096];
  FILE* stream_;

 protected:
  void SetUp() override {
    std::memset(buf_, 0, 4096);
    stream_ = fmemopen(buf_, 4096, "r+");
  }

  void TearDown() override { fclose(stream()); }

  const char* buf() const { return buf_; }
  FILE* stream() { return stream_; }

  void clear() {
    std::memset(buf_, 0, ftell(stream_));
    std::rewind(stream_);
  }

  void writeToStream(std::string_view str, const ProcessInfo& p) {
    Error e = ::writeToStream(stream_, str, p);
    ASSERT_FALSE(e);
  }
};

TEST_F(FormattingTest, BasicFormatting) {
  ProcessInfo p;
  writeToStream("testing %%\ntest", p);
  EXPECT_STREQ("testing %\ntest", buf());
}

TEST_F(FormattingTest, AppendsToStream) {
  ProcessInfo p;
  writeToStream("first", p);
  EXPECT_STREQ("first", buf());
  writeToStream(" second", p);
  EXPECT_STREQ("first second", buf());
}

TEST_F(FormattingTest, InvalidInput) {
  ProcessInfo p;

  Error e = ::writeToStream(stream(), "%x", p);
  EXPECT_TRUE(e);
  EXPECT_STREQ(e.what(), "unkown option: '%x'");
}

TEST_F(FormattingTest, ZeroSeconds) {
  ProcessInfo p;
  std::memset(&p, 0, sizeof(p));

  writeToStream("%e", p);
  EXPECT_STREQ("0.00", buf());

  clear();
  writeToStream("%S", p);
  EXPECT_STREQ("0.00", buf());

  clear();
  writeToStream("%U", p);
  EXPECT_STREQ("0.00", buf());

  clear();
  writeToStream("%E", p);
  EXPECT_STREQ("0:00.00", buf());

  clear();
  writeToStream("%P", p);
  EXPECT_STREQ("0%", buf());
}

TEST_F(FormattingTest, HoursMinutesSeconds) {
  ProcessInfo p;
  std::memset(&p, 0, sizeof(p));

  p.elapsed.tv_sec = 61;
  writeToStream("%E", p);
  EXPECT_STREQ("0:01.01", buf());

  clear();
  p.elapsed.tv_sec = 60 * 60;
  writeToStream("%E", p);
  EXPECT_STREQ("1:00.00", buf());

  clear();
  p.elapsed.tv_sec += 120 + 17;
  writeToStream("%E", p);
  EXPECT_STREQ("1:02.17", buf());
}

TEST_F(FormattingTest, Mem) {
  ProcessInfo p;
  std::memset(&p, 0, sizeof(p));

  p.rusage.ru_maxrss = 5;
  writeToStream("%M", p);
  EXPECT_STREQ("5", buf());

  p.rusage.ru_maxrss = 0;
  clear();
  p.rusage.ru_isrss = 10000;
  writeToStream("%p%D", p);
  EXPECT_STREQ("1000010000", buf());

  clear();
  p.rusage.ru_idrss = 500;
  writeToStream("%D", p);
  EXPECT_STREQ("10500", buf());

  clear();
  p.rusage.ru_ixrss = 20;
  writeToStream("%X", p);
  EXPECT_STREQ("20", buf());
}
