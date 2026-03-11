#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cat_big.c"
#include "cat_big_not_rot.c"
#include "timing.h"

static unsigned char *dd_wvs75v2b_rotate(int width, int heigth,
                                         unsigned char *buf, int buf_len);
int main(void) {
  long long t0 = now_ns();

  unsigned char *rotated =
      dd_wvs75v2b_rotate(1404, 1872, big_cat_not_rot, sizeof(big_cat_not_rot));

  long long t1 = now_ns();
  print_ms("Cat rotated", (t1 - t0));
  (void)rotated;
  free(rotated);
  return 0;
}

static unsigned char *dd_wvs75v2b_rotate(int width, int heigth,
                                         unsigned char *buf, int buf_len) {
  unsigned char *dst = calloc(buf_len, 1);
  for (int y = 0; y < heigth; y++) {
    int src_row = y * width;
    for (int x = 0; x < width; x++) {
      int src_bit = src_row + (width - 1 - x);
      int dst_bit = x * heigth + y;
      int v = (buf[src_bit >> 3] >> (src_bit & 7)) & 1;
      if (v) {
        dst[dst_bit >> 3] |= (1 << (dst_bit & 7));
      }
    }
  }

  return dst;
}

int dd_graphic_get_bit(int i, unsigned char *buf, uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) {
    return -1;
  }

  int byte = i >> 3; // same as i/8 but faster
  int bit = (i % 8);

  return (buf[byte] >> bit) & 1;
}

void dd_graphic_set_bit(int i, int val, unsigned char *buf, uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) {
    return;
  }

  int byte = i >> 3; // same as i/8 but faster
  int bit = (i % 8);

  if (val) {
    buf[byte] |= (1u << bit);
  } else {
    buf[byte] &= ~(1u << bit);
  }
}

int dd_graphic_get_pixel(int x, int y, int width, unsigned char *buf,
                         uint32_t buf_len) {
  int bit = width * y + x;

  return dd_graphic_get_bit(bit, buf, buf_len);
}

void dd_graphic_set_pixel(int x, int y, int v, int width, unsigned char *buf,
                          uint32_t buf_len) {
  int bit = width * y + x;

  dd_graphic_set_bit(bit, v, buf, buf_len);
}

unsigned char *dd_wvs75v2b_rotate_not_optimized(int width, int heigth,
                                                unsigned char *buf,
                                                int buf_len) {
  int v;

  unsigned char *dst = calloc(buf_len, 1);
  for (int y = 0; y < heigth; ++y) {
    for (int x = width - 1, x2 = 0; x >= 0; --x, x2++) {
      v = dd_graphic_get_pixel(x, y, width, buf, buf_len);
      if (v) {
        dd_graphic_set_pixel(y, x2, v, heigth, dst, buf_len);
      }
    }
  }

  return dst;
}
