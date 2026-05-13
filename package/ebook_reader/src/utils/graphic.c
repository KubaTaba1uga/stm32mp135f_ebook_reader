#include <cairo.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <lvgl.h>

#include "utils/mem.h"
#include "utils/time.h"

int cairo_color_format = CAIRO_FORMAT_ARGB32;
int lvgl_color_format = LV_COLOR_FORMAT_ARGB8888;

unsigned char *graphic_argb32_to_i1(int w, int h, const uint8_t *src,
                                           int stride) {
  struct Trace display_trace = trace_start(__func__);  
  int dst_stride = (w + 7) / 8;
  int dst_len = dst_stride * h;
  unsigned char *dst = mem_malloc(dst_len);
  memset(dst, 0x00, dst_stride * h); // 0 = white

  for (int y = 0; y < h; y++) {
    const uint32_t *row = (const uint32_t *)(src + y * stride);
    for (int x = 0; x < w; x++) {
      uint32_t p = row[x]; // 0xAARRGGBB on little-endian
      uint8_t r = (p >> 16) & 0xFF;
      uint8_t g = (p >> 8) & 0xFF;
      uint8_t b = (p >> 0) & 0xFF;

      uint16_t lum = (uint16_t)(r * 30 + g * 59 + b * 11) / 100;
      bool black = lum > 130;
      /* bool black = (r + g + b) != 0; */

      int byte_i = y * dst_stride + (x >> 3);

      /* int bit = 7 - (x & 7); // MSB */
      int bit = (x & 7); // LSB
      if (black) {
        dst[byte_i] |= (1u << bit);
      }
    }
  }

  trace_end(&display_trace);
  
  return dst;
}

int graphic_calc_screen_buf_size(int x, int y) {
  return x * y * 4; // RGBA
}
