#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

static inline void print_bits_grid(unsigned char *buf, size_t buf_len,
                            int bits_per_row);

static inline int get_bit(int i, unsigned char *buf, uint32_t buf_len) {
  int byte_number = i / 8;

  if (byte_number < 0 || byte_number >= buf_len) {
    return -1;
  }

  // If MSB
  int bit_number = 7 - (i & 7);
  /* int bit_number = 7 - (i % 8); */
  // If LSB
  /* int bit_number = (i) % 8; */
  if (bit_number < 0) {
    bit_number = 0;
  }

  return (buf[byte_number] & (1 << bit_number)) > 0;
}

static inline void set_bit(int i, int val, unsigned char *buf, uint32_t buf_len) {
  int byte_number = i / 8;

  // If MSB
  int bit_number = 7 - (i & 7);
  /* int bit_number = 7 - (i % 8); */
  // If LSB
  /* int bit_number = (i) % 8; */

  if (val == 0) {
    buf[byte_number] &= ~(1 << bit_number);
  } else {
    buf[byte_number] |= (1 << bit_number);
  }
}

static inline int get_pixel(int x, int y, int width, unsigned char *buf, uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return -1;
  }

  int bit = width * y + x;

  return get_bit(bit, buf, buf_len);
}

static inline void set_pixel(int x, int y, int width, int val, unsigned char *buf,
               uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return;
  }

  int bit = width * y + x;

  set_bit(bit, val, buf, buf_len);
}

static inline int rotate(int width, int heigth, unsigned char *buf, uint32_t buf_len) {
  unsigned char *rotated = malloc(buf_len);
  TEST_ASSERT(rotated != NULL);
  
  /* unsigned char *new_img = rotated; */
  memset(rotated, 0, buf_len);

  int rows = 0;
  int columns = 0;
  (void)rows;
  (void)columns;

  printf("buf_len=%d\n", buf_len);

  int v;
  int dst_i = 0  ;
  for (int x = width - 1; x > 0; --x) {
  for (int y = 0; y < heigth; ++y) {
    v = get_pixel(x, y, width, buf, buf_len);
    set_bit(dst_i++, v, rotated, buf_len);
  }
}
  
  memcpy(buf, rotated, buf_len);

  free(rotated);
  return 0;
}

static inline  void print_bits_grid(unsigned char *buf, size_t buf_len,
                            int bits_per_row) {
  const int total_bits = (int)(buf_len * 8);

  for (int i = 0; i < total_bits; i++) {
    printf("%d", get_bit(i, buf, buf_len));

    if ((i + 1) % 8 == 0) {
      printf(", ");
    }
    if (bits_per_row > 0 && (i + 1) % bits_per_row == 0) {
      printf("\n");
    }
  }
}
