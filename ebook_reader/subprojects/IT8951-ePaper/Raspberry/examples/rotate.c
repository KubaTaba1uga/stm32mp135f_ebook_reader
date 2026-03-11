#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cat_big.c"
#include "cat_big_not_rot.c"

static unsigned char *dd_wvs75v2b_rotate(int width, int heigth,
                                         unsigned char *buf, int buf_len);
static inline long long now_ns(void);
static inline void print_ms(const char *name, long long dt_ns);
static inline int dd_graphic_get_bit(int i, unsigned char *buf,
                                     uint32_t buf_len);
static inline void dd_graphic_set_bit(int i, int val, unsigned char *buf,
                                      uint32_t buf_len);
static inline int dd_graphic_get_pixel(int x, int y, int width,
                                       unsigned char *buf, uint32_t buf_len);

#define TIME_CALL(label, expr)                                                 \
  do {                                                                         \
    long long t0 = now_ns();                                                   \
    (expr);                                                                    \
    long long t1 = now_ns();                                                   \
    print_ms((label), (t1 - t0));                                              \
  } while (0)

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
  int dst_i = 0;
  int v;

  unsigned char *dst = malloc(buf_len);
  for (int x = width - 1; x >= 0; --x) {
    for (int y = 0; y < heigth; ++y) {
      v = dd_graphic_get_pixel(x, y, width, buf, buf_len);
      dd_graphic_set_bit(dst_i++, v, dst, buf_len);
    }
  }

  return dst;
}

static inline long long now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static inline void print_ms(const char *name, long long dt_ns) {
  printf("%s: %.3f ms\n", name, (double)dt_ns / 1e6);
}

static inline int dd_graphic_get_bit(int i, unsigned char *buf,
                                     uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) {
    return -1;
  }
  int byte = i >> 3; // same as i/8 but faster  
  /* int byte = i / 8; */
  /* int bit = 7 - (i % 8); */
  int bit = (i % 8);
  return (buf[byte] >> bit) & 1;
}

static inline void dd_graphic_set_bit(int i, int val, unsigned char *buf,
                                      uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) {
    return ;
  }

  int byte = i >> 3; // same as i/8 but faster
  /* int bit = 7 - (i % 8); */
  int bit = (i % 8);

  if (val) {
    buf[byte] |= (1u << bit);
  } else {
    buf[byte] &= ~(1u << bit);
  }
}

static inline int dd_graphic_get_pixel(int x, int y, int width,
                                       unsigned char *buf, uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return -1;
  }

  int bit = width * y + x;

  return dd_graphic_get_bit(bit, buf, buf_len);
}
