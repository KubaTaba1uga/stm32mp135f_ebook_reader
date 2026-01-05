#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "turtle_horizontal.h"
#include "turtle_vertical.h"

static void print_bits_grid(unsigned char *buf, size_t buf_len,
                            int bits_per_row);

int get_bit(int i, unsigned char *buf, uint32_t buf_len) {
  int byte_number = floor(i / 8);

  if (byte_number < 0 || byte_number >= buf_len) {
    return -1;
  }

  // If MSB
  int bit_number = 7 - (i % 8);
  // If LSB
  /* int bit_number = ((i) % 8) - 1; */
  if (bit_number < 0) {
    bit_number = 0;
  }

  return (buf[byte_number] & (1 << bit_number)) > 0;
}

void set_bit(int i, int val, unsigned char *buf, uint32_t buf_len) {
  int byte_number = floor(i / 8);

  if (byte_number < 0 || byte_number >= buf_len) {
    return;
  }

  // If MSB
  int bit_number = 7 - (i % 8);
  // If LSB
  /* int bit_number = ((i) % 8) - 1; */
  if (bit_number < 0) {
    bit_number = 0;
  }

  if (val == 0) {
    buf[byte_number] &= ~(1 << bit_number);
  } else {
    buf[byte_number] |= (1 << bit_number);
  }
}

int get_pixel(int x, int y, int width, unsigned char *buf, uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return -1;
  }

  int bit = width * y + x;

  return get_bit(bit, buf, buf_len);
}

void set_pixel(int x, int y, int width, int val, unsigned char *buf,
               uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return;
  }

  int bit = width * y + x;

  set_bit(bit, val, buf, buf_len);
}

int rotate(int width, int heigth, unsigned char *buf, uint32_t buf_len) {
  unsigned char *rotated = malloc(buf_len);
  TEST_ASSERT(rotated != NULL);
  /* unsigned char *new_img = rotated; */
  memset(rotated, 0, buf_len);

  int first_pixel = width * (heigth - 1);
  int last_pixel = first_pixel + width;

  int rows = 0;
  int columns = 0;
  (void)rows;
  (void)columns;

  for (int src_i = first_pixel, dst_i = 0; src_i < last_pixel; src_i++) {
    for (int src_k = 0; src_k < heigth; src_k++, dst_i++) {
      int bit_number = src_i - (width * src_k);
      int bit_value = get_bit(bit_number, buf, buf_len);
      set_bit(dst_i, bit_value, rotated, buf_len);
    }
  }
  /*   for (int y = 0; y < heigth; y++) { */

  /* for (int x = 0, xcp = 0; x > 0; x++, xcp++) { */
  /*   for (int y = 0; y < heigth; y++) { */

  /* for (int x = width - 1, xcp = 0; x > 0; x--, xcp++) { */
  /*   for (int y = 0; y < heigth; y++) { */
  /*     set_pixel(xcp, y, width, get_pixel(x, y, width, buf, buf_len), rotated,
   */
  /*               buf_len); */

  /*     printf("wrote %dx%d=%d\n", rows, columns, */
  /*            get_pixel(x, y, width, buf, buf_len)); */
  /*     columns++; */
  /*   } */
  /*   columns = 0; */
  /*   rows++; */
  /* } */

  /* TEST_ASSERT(last_pixel - first_pixel == width); */
  /* // We are iterating pixels */
  /* int counter = 0; */
  /* for (int i = first_pixel; i < last_pixel; i++) { */
  /*   unsigned char byte = 0; */
  /*   int bits_i = 0; */
  /*   for (int k = 0; k < heigth; k++) { */
  /*     int bit_number = i - (width * k); */
  /*     byte |= get_bit(bit_number, buf, buf_len) << (7 - bits_i); */
  /*     printf("wrote %dx%d=\n", rows, columns); */

  /*     if (bits_i == 7) { */
  /*       counter++; */
  /*       printf("byte %d=%d\n", counter, byte); */
  /*       bits_i = 0; */

  /*       *new_img = byte; */
  /*       new_img++; */
  /*       byte = 0; */
  /*     } */

  /*     bits_i++;       */
  /*     columns++; */
  /*   } */
  /*   columns = 0; */
  /*   rows++; */
  /* } */

  memcpy(buf, rotated, buf_len);

  free(rotated);
  return 0;
}

void test_rotate(void) {
  int buf16x8w = 16;
  int buf16x8h = 8;
  unsigned char buf16x8[] = {
      // 2  bytes x 8  = 16  bytes
      // 16 bits  x 8  = 128 bits
      0b00000000, 0b00010000, //
      0b00000000, 0b00010000, //
      0b00000000, 0b00010000, //
      0b00000000, 0b00010000, //
      0b00000000, 0b00000000, //
      0b00000000, 0b00000000, //
      0b00000000, 0b00000000, //
      0b00000000, 0b00000000, //
  };

  unsigned char buf16x8roatetd[] = {
      // 2  bytes x 2  = 4  bytes
      // 16 bits  x 2  = 32 bits
      0b00000000, //
      0b00000000, //
      0b00000000, //
      0b00000000, //
      0b00000000, //
      0b00000000, //
      0b00000000, //
      0b00000000, //
      0b00000000, //
      0b00000000, //
      0b00000000, //
      0b00001111, //
      0b00000000, //
      0b00000000, //
      0b00000000, //
      0b00000000, //
  };

  /* memset(buf16x8, 0, sizeof(buf16x8)); */

  print_bits_grid(buf16x8, sizeof(buf16x8), buf16x8w);
  puts("");

  int err = rotate(buf16x8w, buf16x8h, buf16x8, sizeof(buf16x8));
  TEST_ASSERT_EQUAL(0, err);
  TEST_ASSERT_EQUAL_CHAR_ARRAY(buf16x8roatetd, buf16x8, sizeof(buf16x8));

  print_bits_grid(buf16x8, sizeof(buf16x8), buf16x8h);
  puts("");
  
  int buf24x16w = 24;
  int buf24x16h = 16;
  unsigned char buf24x16[] = {
      // 2  bytes x 8  = 16  bytes
      // 16 bits  x 8  = 128 bits
      0b00000000, 0b00000000, 0b00000000, //
      0b00111000, 0b00000000, 0b00000000, //
      0b00111000, 0b00000000, 0b00000000, //
      0b00111000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
      0b00000000, 0b00000000, 0b00000000, //
  };

  unsigned char buf24x16roatetd[] = {
      // 2  bytes x 2  = 4  bytes
      // 16 bits  x 2  = 32 bits
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000,  //
      0b00000000,       0b00001110, //
      0b00000000,       0b00001110, //
      0b00000000,       0b00001110, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
      0b00000000,       0b00000000, //
  };

  print_bits_grid(buf24x16, sizeof(buf24x16), buf24x16w);
  puts("");
  print_bits_grid(buf24x16roatetd, sizeof(buf24x16), buf24x16h);


  err = rotate(buf24x16w, buf24x16h, buf24x16, sizeof(buf24x16));
  puts("");
  print_bits_grid(buf24x16, sizeof(buf24x16), buf24x16h);
  puts("");
  
  TEST_ASSERT_EQUAL(0, err);
  TEST_ASSERT_EQUAL_CHAR_ARRAY(buf24x16roatetd, buf24x16, sizeof(buf24x16));
  
  /* for (int y = 0; y < buf2x2h; y++) { */
  /*   for (int x = 0; x < buf2x2w;) { */
  /*     printf("%d", get_pixel(x, y, buf2x2w, buf2x2,sizeof(buf2x2))); */
  /*     x++; */
  /*     if (x > 0 && x % 8 == 0) { */
  /*       printf(", "); */
  /*     } */
  /*   } */
  /*   printf("\n"); */
  /* } */
  /* TEST_ASSERT_EQUAL_CHAR_ARRAY(turtle_vertical, turtle_horizontal, */
  /*                              sizeof(turtle_vertical)); */

  /* unsigned char buf[48000]; */
  /* memset(buf, 0, sizeof(buf)); */
  /* int err = rotate(buf, sizeof(buf)); */
  /* TEST_ASSERT_EQUAL(0, err); */

  /* TEST_ASSERT_EQUAL_CHAR_ARRAY(turtle_vertical, turtle_horizontal, */
  /* sizeof(turtle_vertical)); */

  /* err = rotate(800, 480, turtle_horizontal, sizeof(turtle_horizontal)); */
  /* TEST_ASSERT_EQUAL(0, err); */

  /* TEST_ASSERT_EQUAL_CHAR_ARRAY(turtle_vertical, turtle_horizontal, */
  /*                              sizeof(turtle_vertical)); */
}

void test_get_bit(void) {
  unsigned char bytes[] = {0x40}; // 0b0100_0000

  TEST_ASSERT_EQUAL(0, get_bit(0, bytes, 1));
  TEST_ASSERT_EQUAL(1, get_bit(1, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(2, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(3, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(4, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(5, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(6, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(7, bytes, 1));

  memcpy(bytes, (unsigned char[]){0x04}, 1); // 0b0000_0100

  TEST_ASSERT_EQUAL(0, get_bit(0, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(1, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(2, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(3, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(4, bytes, 1));
  TEST_ASSERT_EQUAL(1, get_bit(5, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(6, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_bit(7, bytes, 1));

  unsigned char bytes2[] = {0x80, 0x01}; // 1000_0000 0000_0001

  // First byte
  TEST_ASSERT_EQUAL(1, get_bit(0, bytes2, 2)); // MSB of byte[0]
  TEST_ASSERT_EQUAL(0, get_bit(1, bytes2, 2));
  TEST_ASSERT_EQUAL(0, get_bit(2, bytes2, 2));
  TEST_ASSERT_EQUAL(0, get_bit(3, bytes2, 2));
  TEST_ASSERT_EQUAL(0, get_bit(4, bytes2, 2));
  TEST_ASSERT_EQUAL(0, get_bit(5, bytes2, 2));
  TEST_ASSERT_EQUAL(0, get_bit(6, bytes2, 2));
  TEST_ASSERT_EQUAL(0, get_bit(7, bytes2, 2)); // LSB of byte[0]

  // Second byte
  TEST_ASSERT_EQUAL(0, get_bit(8, bytes2, 2)); // MSB of byte[1]
  TEST_ASSERT_EQUAL(0, get_bit(9, bytes2, 2));
  TEST_ASSERT_EQUAL(0, get_bit(10, bytes2, 2));
  TEST_ASSERT_EQUAL(0, get_bit(11, bytes2, 2));
  TEST_ASSERT_EQUAL(0, get_bit(12, bytes2, 2));
  TEST_ASSERT_EQUAL(0, get_bit(13, bytes2, 2));
  TEST_ASSERT_EQUAL(0, get_bit(14, bytes2, 2));
  TEST_ASSERT_EQUAL(1, get_bit(15, bytes2, 2)); // LSB of byte[1]
}

void test_get_pixel(void) {
  unsigned char bytes[] = {0x40}; // 0b0100_0000

  TEST_ASSERT_EQUAL(0, get_pixel(0, 0, 1, bytes, 1));
  TEST_ASSERT_EQUAL(1, get_pixel(1, 0, 1, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(2, 0, 1, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(3, 0, 1, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(4, 0, 1, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(5, 0, 1, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(6, 0, 1, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(7, 0, 1, bytes, 1));

  memcpy(bytes, (unsigned char[]){0x04}, 1); // 0b0000_0100
}

static void print_bits_grid(unsigned char *buf, size_t buf_len,
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
