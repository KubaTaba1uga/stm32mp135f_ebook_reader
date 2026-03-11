#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "cat_big.c"
#include "cat_big_not_rot.c"

static unsigned char *dd_wvs75v2b_rotate(int width, int heigth,
                                         unsigned char *buf, int buf_len);
static inline long long now_ns(void);
static inline void print_ms(const char *name, long long dt_ns);
static inline int src_get_bit(int i, unsigned char *buf, uint32_t buf_len);
static inline int src_get_pixel(int x, int y, int width, unsigned char *buf,
                                uint32_t buf_len);
static inline void dst_set_bit(int i, int val, unsigned char *buf,
                               uint32_t buf_len);
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

int src_byte;
int src_bit;
int dst_byte;
int dst_bit;
static unsigned char *dd_wvs75v2b_rotate(int width, int heigth,
                                         unsigned char *buf, int buf_len) {
  int dst_i = 0;
  int v;

  FILE *bytes = fopen("bytes.h", "w");
  char struct_rot[] =
      "struct Rotation { int src_byte; int src_bit; int dst_byte; int dst_bit; "
      "};\n struct Rotation rotation_table[] = {\n";
  fwrite(struct_rot, sizeof(struct_rot) -1, 1, bytes);

  char result[1024];
  unsigned char *dst = malloc(buf_len);
  for (int x = width - 1; x >= 0; --x) {
    for (int y = 0; y < heigth; ++y) {
      v = src_get_pixel(x, y, width, buf, buf_len);
      dst_set_bit(dst_i++, v, dst, buf_len);

      sprintf(result, 
               "{ .src_byte=%d, .src_bit=%d, .dst_byte=%d, .dst_bit=%d,},\n",
               src_byte, src_bit, dst_byte, dst_bit);
      fwrite(result, strlen(result), 1, bytes);
    }
  }
  fwrite("};", 3, 1, bytes);
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

static inline int src_get_pixel(int x, int y, int width, unsigned char *buf,
                                uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return -1;
  }

  int bit = width * y + x;

  return src_get_bit(bit, buf, buf_len);
}

static inline int src_get_bit(int i, unsigned char *buf, uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) {
    return -1;
  }
  src_byte = i >> 3; // same as i/8 but faster
  src_bit = (i % 8);
  return (buf[src_byte] >> src_bit) & 1;
}

static inline void dst_set_bit(int i, int val, unsigned char *buf,
                               uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) {
    return;
  }
  dst_byte = i >> 3; // same as i/8 but faster
  dst_bit = (i % 8);

  if (val) {
    buf[dst_byte] |= (1u << dst_bit);
  } else {
    buf[dst_byte] &= ~(1u << dst_bit);
  }
}
