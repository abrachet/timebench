/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#pragma once

#include <cstdio>
#include <cstdlib>

/// This is used for syscalls that almost never fail and obviously
/// isn't a replacement for syscalls that can fail and require attention.
template <typename ReturnType>
constexpr void syscall_assert(ReturnType expr,
                              ReturnType failure = ReturnType{-1}) {
  if (expr == failure) {
    std::perror(nullptr);
    std::abort();
  }
}
