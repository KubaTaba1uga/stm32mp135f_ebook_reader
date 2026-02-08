#ifndef DISPLAY_DRIVER_IO_H
#define DISPLAY_DRIVER_IO_H

int dd_io_open(const char *pathname, int flags);
void dd_io_close(int fd);
int dd_io_ioctl(int fd, unsigned long req, void *arg);

#endif // DISPLAY_DRIVER_IO_H
