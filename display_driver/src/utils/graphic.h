#ifndef DISPLAY_DRIVER_GRAPHIC_H
#define DISPLAY_DRIVER_GRAPHIC_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int dd_graphic_get_bit(int i, unsigned char *buf, uint32_t buf_len);
void dd_graphic_set_bit(int i, int val, unsigned char *buf, uint32_t buf_len);
int dd_graphic_get_pixel(int x, int y, int width, unsigned char *buf,
                         uint32_t buf_len);

#endif // DISPLAY_DRIVER_MEM_H
