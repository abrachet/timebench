/**
 * Alex Brachet (abrachet@cs.purdue.edu)
 */

#include "timebench/OutputStreams.h"
#include <assert.h>
#include <fcntl.h>
#include <cstdlib>

int nullfd() {
  static int fd = open("/dev/null", O_RDWR);

// Anyway closing this fd isn't a big deal given that the OS closes all
// file descriptors on _exit(2).
#ifdef __GLIBC__
  static int _ = std::on_exit(
      +[](int, void *fd) { (void)close(reinterpret_cast<uintptr_t>(fd)); },
      reinterpret_cast<void *>(fd));
#endif

  assert(fd > 0 && "open(2) failed");
  assert(fcntl(fd, F_GETFD) != -1 && "File was closed");
  return fd;
}
