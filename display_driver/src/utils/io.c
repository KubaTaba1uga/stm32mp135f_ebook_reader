#define _GNU_SOURCE
#include <fcntl.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <unistd.h>

int dd_io_open(const char *pathname, int flags) {
  return open(pathname, flags);
}

void dd_io_close(int fd) { close(fd); }

int dd_io_ioctl(int fd, unsigned long req, void *arg) {
  return ioctl(fd, req, arg);
}
