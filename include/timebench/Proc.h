/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#pragma once

#include <sys/types.h>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class Process {
  friend class ProcessBuilder;

  // keep here not in create_process() so that this outlives the child process.
  std::vector<const char*> to_process;

  int stdin_fd, stdout_fd, stderr_fd;

  Process(std::vector<const char*>& to_process, int stdin_fd, int stdout_fd,
          int stderr_fd)
      : to_process(to_process),
        stdin_fd(stdin_fd),
        stdout_fd(stdout_fd),
        stderr_fd(stderr_fd) {}

 public:
  pid_t run();
};

/// Creates argv and other environment set up before execve(2)'ing the program.
/// It is important that the ProcessBuilder exist longer than the process
/// being run because of the life time of the strings in the argv array it
/// creates. ProcessBuilder keeps std::strings and not std::string_view or
/// similar, because the underlying process can technically modifiy it's argv.
/// Looking at the source, Linux does copy these strings on sys_execve, but it
///  isn't required by POSIX that they be copied.
class ProcessBuilder {
  std::optional<std::string> shell_to_use = {};
  std::optional<std::string> shell_commands = "-c";

  std::vector<std::string> command;

  int stdin_fd = 0;
  int stdout_fd = 1;
  int stderr_fd = 2;

  mutable std::vector<std::string> argv;

  bool ready = false;

 public:
  ProcessBuilder() = default;
  ProcessBuilder(std::string_view command) : command(split_string(command)) {}
  ProcessBuilder(std::vector<std::string> command) : command(command) {}

  ProcessBuilder& shell(std::string sh = "/bin/sh");
  ProcessBuilder& shell_args(std::string args);
  ProcessBuilder& command_push_back(std::string arg);

  ProcessBuilder& stdin(int fd);
  ProcessBuilder& stdout(int fd);
  ProcessBuilder& stderr(int fd);

  /// If any of these are -1 then the fd from this will be used instead of these
  /// new ones.
  Process build(int in = -1, int out = -1, int err = -1) const;

  bool isReady() const { return ready; }

  std::string command_name();

 private:
  static std::vector<std::string> split_string(std::string_view str);

  void create_argv() const;
  std::vector<const char*> create_pass_to_exec() const;
};
