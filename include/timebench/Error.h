/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#pragma once

#include <string>

class [[nodiscard]] Error {
 public:
  enum Severity {
    Ok = 0,
    Warning,
    Fatal,
  };

 protected:
  Severity severity;
  std::string message;

  mutable bool checked = false;

  Error(Severity severity, std::string message)
      : severity(severity), message(message) {
    assert(!message.data() ||
           message.data()[message.size()] == 0 && "String not null terminated");
  }

 public:
  Error(const Error&) = delete;
  Error(Error &&) = default;

  ~Error() { assert(checked); }

  operator bool() const {
    checked = true;
    return severity;
  }

  const char* what() const { return message.data(); }

  bool isFatal() const { return severity == Fatal; }

  static Error success() { return {Ok, {}}; }

  static Error warn(std::string message) { return {Warning, message}; }

  static Error fatal(std::string message) { return {Fatal, message}; }
};
