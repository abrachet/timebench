/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#include "timebench/Proc.h"
#include <unistd.h>
#include <cerrno>
#include "timebench/Algorithm.h"

ProcessBuilder& ProcessBuilder::shell(std::string sh) {
  shell_to_use = sh;
  return *this;
}

ProcessBuilder& ProcessBuilder::shell_args(std::string args) {
  if (args == "")
    shell_commands = {};
  else
    shell_commands = args;
  return *this;
}

ProcessBuilder& ProcessBuilder::command_push_back(std::string arg) {
  command.push_back(arg);
  return *this;
}

ProcessBuilder& ProcessBuilder::stdin(int fd) {
  stdin_fd = fd;
  return *this;
}

ProcessBuilder& ProcessBuilder::stdout(int fd) {
  stdout_fd = fd;
  return *this;
}

ProcessBuilder& ProcessBuilder::stderr(int fd) {
  stderr_fd = fd;
  return *this;
}

Process ProcessBuilder::build(int in, int out, int err) const {
  create_argv();
  std::vector<const char*> to_process = create_pass_to_exec();
  assert(to_process.back() == nullptr && "Not nullptr terminated array");
  return Process(to_process, in == -1 ? stdin_fd : in,
                 out == -1 ? stdout_fd : out, err == -1 ? stderr_fd : err);
}

std::vector<std::string> ProcessBuilder::split_string(std::string_view str) {
  std::vector<std::string> ret;
  bool in_quote = false;
  size_t index = 0, last_index = 0;
  const char* const data = str.data();
  for (const char c : str) {
    if (c == '\'' || c == '"') in_quote = !in_quote;
    if (c == ' ' && !in_quote) {
      ret.push_back({data + last_index, index - last_index});
      last_index = index + 1;
    }
    ++index;
  }
  ret.push_back({data + last_index, index - last_index});
  return ret;
}

void ProcessBuilder::create_argv() const {
  if (argv.size()) return;
  auto add_string_vec = [this](std::vector<std::string> vec) {
    tb::transform(vec, std::back_inserter(argv),
                  [](std::string str) { return str; });
  };

  if (shell_to_use) {
    argv.push_back(shell_to_use.value().c_str());
    if (shell_commands) add_string_vec(split_string(shell_commands.value()));
  }

  add_string_vec(command);
}

std::vector<const char*> ProcessBuilder::create_pass_to_exec() const {
  assert(!argv.empty() && "create_argv potentially not called");

  std::vector<const char*> to_process;
  for (const std::string& str : argv) to_process.push_back(str.c_str());
#if 0
  tb::transform(argv, std::back_inserter(to_process), [](std::string str) { 
      return str.c_str();
  });
#endif
  to_process.push_back(nullptr);
  return std::move(to_process);
}

pid_t Process::run() {
  char* const* to_exec = const_cast<char* const*>(to_process.data());

  pid_t pid = fork();
  if (pid < 0) return pid;
  if (!pid) {
    if (stdin_fd != 0) {
      int fd = dup2(stdin_fd, 0);
      assert(fd != -1 && "Couldn't dup fd");
      (void)close(stdin_fd);
      (void)fd;
    }

    if (stdout_fd != 1) {
      int fd = dup2(stdout_fd, 1);
      assert(fd != -1 && "Couldn't dup fd");
      (void)close(stdout_fd);
      (void)fd;
    }

    if (stderr_fd != 2) {
#if 0
      int fd = dup2(stderr_fd, 2);
      perror("dup2");
      fprintf(stderr, "%d\n", stderr_fd);
      assert(fd != -1 && "Couldn't dup fd");
      (void)close(stderr_fd);
      (void)fd;
#endif
    }
    execvp(*to_exec, to_exec);
    // use _exit(2) not exit(3) so that atexit functions etc. aren't called.
    _exit(127);
  }

  return pid;
}