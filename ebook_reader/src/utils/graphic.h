#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <stdbool.h>
#include <stdint.h>

unsigned char *graphic_argb32_to_i1(int w, int h, const uint8_t *src,
				    int stride);
int graphic_calc_screen_buf_size(int x, int y);

extern int cairo_color_format;
extern int lvgl_color_format;

#endif // GRAPHIC_H
