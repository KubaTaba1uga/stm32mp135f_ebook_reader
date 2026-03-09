#include <arm_neon.h>
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
  unsigned char tmp[4];
  int dst_i = 0;
  int src_y;
  int src_x;
  /* int v; */

  unsigned char *dst = malloc(buf_len);
  for (src_x = width - 1; src_x >= 0; --src_x) {
    for (src_y = 0; src_y < heigth; src_y += 4) {
      tmp[0] = dd_graphic_get_pixel(src_x, src_y, width, buf, buf_len);
      tmp[1] = dd_graphic_get_pixel(src_x, src_y + 1, width, buf, buf_len);
      tmp[2] = dd_graphic_get_pixel(src_x, src_y + 2, width, buf, buf_len);
      tmp[3] = dd_graphic_get_pixel(src_x, src_y + 3, width, buf, buf_len);

      dd_graphic_set_bit(dst_i++, tmp[0], dst, buf_len);
      dd_graphic_set_bit(dst_i++, tmp[1], dst, buf_len);
      dd_graphic_set_bit(dst_i++, tmp[2], dst, buf_len);
      dd_graphic_set_bit(dst_i++, tmp[3], dst, buf_len);
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

int dd_graphic_get_4bits(int i, unsigned char *buf, uint32_t buf_len,
                         unsigned char *tmp) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) {
    return -1;
  };

  /* int byte = i >> 3; // same as i/8 but faster */
  uint16_t bytes[4] = {
      i + 0,
      i + 1,
      i + 2,
      i + 3,
  };
  uint16x4_t nbytes = vld1_u16(bytes);
  uint16x4_t nbytes_shifted = vshr_n_u16(nbytes, 3);
  vst1_u16(bytes, nbytes_shifted);

  uint16_t bits[4] = {
      (i + 0) % 8,
      (i + 1) % 8,
      (i + 2) % 8,
      (i + 3) % 8,
  };
  /* int bit = (i % 8); */
  /* uint16x4_t nbits = vand_u16(i, vdup_n_u16(7)); */
  /* uint16_t bits[4]; */
  /* vst1_u16(bits, nbits); */

  /* return (buf[byte] >> bit) & 1; */
  tmp[0] = buf[bytes[0]] >> bits[0] & 1;
  tmp[1] = buf[bytes[1]] >> bits[1] & 1;
  tmp[2] = buf[bytes[2]] >> bits[2] & 1;
  tmp[3] = buf[bytes[3]] >> bits[3] & 1;
  return 0;
}

static inline void dd_graphic_set_bit(int i, int val, unsigned char *buf,
                                      uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) {
    return;
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
