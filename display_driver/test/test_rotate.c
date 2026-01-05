#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "rotate.h"
#include "cat_rotated.h"
#include "cat_not_rotated.h"

void test_rotate(void) {
  /* int buf16x8w = 16; */
  /* int buf16x8h = 8; */
  /* unsigned char buf16x8[] = { */
  /*     // 2  bytes x 8  = 16  bytes */
  /*     // 16 bits  x 8  = 128 bits */
  /*     0b00000000, 0b00010000, // */
  /*     0b00000000, 0b00010000, // */
  /*     0b00000000, 0b00010000, // */
  /*     0b00000000, 0b00010000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /* }; */

  /* unsigned char buf16x8roatetd[] = { */
  /*     // 2  bytes x 2  = 4  bytes */
  /*     // 16 bits  x 2  = 32 bits */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00001111, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /*     0b00000000, // */
  /* }; */
  /* (void)buf16x8roatetd; */
  /* /\* memset(buf16x8, 0, sizeof(buf16x8)); *\/ */

  /* print_bits_grid(buf16x8, sizeof(buf16x8), buf16x8w); */
  /* puts(""); */

  /* int err = rotate(buf16x8w, buf16x8h, buf16x8, sizeof(buf16x8)); */
  /* /\* TEST_ASSERT_EQUAL(0, err); *\/ */
  /* /\* TEST_ASSERT_EQUAL_CHAR_ARRAY(buf16x8roatetd, buf16x8, sizeof(buf16x8)); *\/ */

  /* print_bits_grid(buf16x8, sizeof(buf16x8), buf16x8h); */
  /* puts(""); */

  /* int buf24x16w = 24; */
  /* int buf24x16h = 16; */
  /* unsigned char buf24x16[] = { */
  /*     // 2  bytes x 8  = 16  bytes */
  /*     // 16 bits  x 8  = 128 bits */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00111000, 0b00000000, 0b00000000, // */
  /*     0b00111000, 0b00000000, 0b00000000, // */
  /*     0b00111000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, 0b00000000, // */
  /* }; */

  /* unsigned char buf24x16roatetd[] = { */
  /*     // 2  bytes x 2  = 4  bytes */
  /*     // 16 bits  x 2  = 32 bits */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00001110, // */
  /*     0b00000000, 0b00001110, // */
  /*     0b00000000, 0b00001110, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /*     0b00000000, 0b00000000, // */
  /* }; */

  /* print_bits_grid(buf24x16, sizeof(buf24x16), buf24x16w); */
  /* puts(""); */
  /* print_bits_grid(buf24x16roatetd, sizeof(buf24x16), buf24x16h); */

  /* err = rotate(buf24x16w, buf24x16h, buf24x16, sizeof(buf24x16)); */
  /* puts(""); */
  /* print_bits_grid(buf24x16, sizeof(buf24x16), buf24x16h); */
  /* puts(""); */

  /* TEST_ASSERT_EQUAL(0, err); */
  /* TEST_ASSERT_EQUAL_CHAR_ARRAY(buf24x16roatetd, buf24x16, sizeof(buf24x16)); */

  
int err = rotate(480, 800, cat_not_rotated, sizeof(cat_not_rotated));
  TEST_ASSERT_EQUAL(0, err);

  TEST_ASSERT_EQUAL_CHAR_ARRAY(cat_rotated, cat_not_rotated,
                               sizeof(cat_not_rotated));
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

  // width is in pixels (bits), so 8 pixels in this row
  TEST_ASSERT_EQUAL(0, get_pixel(0, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(1, get_pixel(1, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(2, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(3, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(4, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(5, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(6, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(7, 0, 8, bytes, 1));

  memcpy(bytes, (unsigned char[]){0x04}, 1); // 0b0000_0100

  TEST_ASSERT_EQUAL(0, get_pixel(0, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(1, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(2, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(3, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(4, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(1, get_pixel(5, 0, 8, bytes, 1)); // MSB-first: bit 5 is set
  TEST_ASSERT_EQUAL(0, get_pixel(6, 0, 8, bytes, 1));
  TEST_ASSERT_EQUAL(0, get_pixel(7, 0, 8, bytes, 1));

  int buf16x8w = 16;
  int buf16x8h = 8;
  unsigned char buf16x8[] = {
      0b00000000, 0b00010000, //
      0b00000000, 0b00010000, //
      0b00000000, 0b00010000, //
      0b00000000, 0b00010000, //
      0b00000000, 0b00000000, //
      0b00000000, 0b00000000, //
      0b00000000, 0b00000000, //
      0b00000000, 0b00000000, //
  };

  // Rows 0..3 have a single pixel set at x=11
  for (int y = 0; y < buf16x8h; y++) {
    for (int x = 0; x < buf16x8w; x++) {
      int expected = 0;
      if (y < 4 && x == 11)
        expected = 1;
      TEST_ASSERT_EQUAL(expected,
                        get_pixel(x, y, buf16x8w, buf16x8, sizeof(buf16x8)));
    }
  }
}

void test_set_pixel(void) {
  // --- 1) single byte, MSB-first ---
  unsigned char b1[] = {0x00};

  // set x=1 => bit #6 => 0b0100_0000
  set_pixel(1, 0, 8, 1, b1, 1);
  TEST_ASSERT_EQUAL_HEX8(0x40, b1[0]);
  TEST_ASSERT_EQUAL(1, get_pixel(1, 0, 8, b1, 1));

  // clear it back
  set_pixel(1, 0, 8, 0, b1, 1);
  TEST_ASSERT_EQUAL_HEX8(0x00, b1[0]);
  TEST_ASSERT_EQUAL(0, get_pixel(1, 0, 8, b1, 1));

  // set x=5 => bit #2 => 0b0000_0100
  set_pixel(5, 0, 8, 1, b1, 1);
  TEST_ASSERT_EQUAL_HEX8(0x04, b1[0]);
  TEST_ASSERT_EQUAL(1, get_pixel(5, 0, 8, b1, 1));

  // --- 2) crossing byte boundary: width=16 => 2 bytes per row ---
  unsigned char b2[] = {0x00, 0x00};

  // set last pixel of first byte (x=7) => LSB of byte0 => 0x01
  set_pixel(7, 0, 16, 1, b2, 2);
  TEST_ASSERT_EQUAL_HEX8(0x01, b2[0]);
  TEST_ASSERT_EQUAL_HEX8(0x00, b2[1]);
  TEST_ASSERT_EQUAL(1, get_pixel(7, 0, 16, b2, 2));

  // set first pixel of second byte (x=8) => MSB of byte1 => 0x80
  set_pixel(8, 0, 16, 1, b2, 2);
  TEST_ASSERT_EQUAL_HEX8(0x01, b2[0]);
  TEST_ASSERT_EQUAL_HEX8(0x80, b2[1]);
  TEST_ASSERT_EQUAL(1, get_pixel(8, 0, 16, b2, 2));

  // clear both
  set_pixel(7, 0, 16, 0, b2, 2);
  set_pixel(8, 0, 16, 0, b2, 2);
  TEST_ASSERT_EQUAL_HEX8(0x00, b2[0]);
  TEST_ASSERT_EQUAL_HEX8(0x00, b2[1]);

  // --- 3) multi-row addressing on 16x8 buffer ---
  int w = 16;
  unsigned char img[16] = {0}; // 2 bytes/row * 8 rows = 16 bytes

  // set a pixel at (x=11, y=3)
  set_pixel(11, 3, w, 1, img, sizeof(img));
  TEST_ASSERT_EQUAL(1, get_pixel(11, 3, w, img, sizeof(img)));

  // ensure neighbors are still zero
  TEST_ASSERT_EQUAL(0, get_pixel(10, 3, w, img, sizeof(img)));
  TEST_ASSERT_EQUAL(0, get_pixel(12, 3, w, img, sizeof(img)));
  TEST_ASSERT_EQUAL(0, get_pixel(11, 2, w, img, sizeof(img)));
  TEST_ASSERT_EQUAL(0, get_pixel(11, 4, w, img, sizeof(img)));

  // clear it
  set_pixel(11, 3, w, 0, img, sizeof(img));
  TEST_ASSERT_EQUAL(0, get_pixel(11, 3, w, img, sizeof(img)));

}
