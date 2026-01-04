#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "turtle_horizontal.h"
#include "turtle_vertical.h"

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

int rotate(unsigned char *buf, uint32_t buf_len) {
  char *rotated = malloc(buf_len);
  TEST_ASSERT(rotated != NULL);
  char *new_img = rotated;
  memset(rotated, 0, buf_len);

  const int width = 800;
  const int heigth = 480;

  int first_pixel = width * (heigth - 1);
  int last_pixel = first_pixel + width;

  int rows = 0;
  int columns = 0;

  TEST_ASSERT(last_pixel - first_pixel == width);
  // We are iterating pixels
  int counter = 0;
  for (int i = first_pixel; i < last_pixel; i++) {
    unsigned char byte = 0;
    int bits_i = 0;
    for (int k = 0; k < heigth; k++) {
      int bit_number = i - (width * k);      
      byte |= get_bit(bit_number, buf, buf_len) << (7 - bits_i);
      printf("wrote %dx%d=\n", rows, columns);
      if (bits_i == 7) {
        counter++;
        printf("byte %d=%d\n", counter, byte);
        bits_i = 0;


        *new_img = byte;
        new_img++;
        byte = 0;
        
      } else {
        bits_i++;
      }

      columns++;
    }
    columns = 0;
    rows++;
  }
  

  memcpy(buf,rotated, buf_len);

  free(rotated);
  return 0;
}

void test_rotate(void) {
  /* unsigned char buf[48000]; */
  /* memset(buf, 0, sizeof(buf)); */
  /* int err = rotate(buf, sizeof(buf)); */
  /* TEST_ASSERT_EQUAL(0, err); */

  /* TEST_ASSERT_EQUAL_CHAR_ARRAY(turtle_vertical, turtle_horizontal, */
  /*                              sizeof(turtle_vertical)); */

  int err = rotate(turtle_horizontal, sizeof(turtle_horizontal));
  TEST_ASSERT_EQUAL(0, err);

  TEST_ASSERT_EQUAL_CHAR_ARRAY(turtle_vertical, turtle_horizontal,
                               sizeof(turtle_vertical));
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
