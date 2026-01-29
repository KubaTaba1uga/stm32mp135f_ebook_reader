#include <stdint.h>

int dd_graphic_get_bit(int i, unsigned char *buf, uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) {
    return -1;
  }
  int byte = i / 8;
  int bit = 7 - (i % 8);
  return (buf[byte] >> bit) & 1;
}

void dd_graphic_set_bit(int i, int val, unsigned char *buf, uint32_t buf_len) {
  if (i < 0 || (uint32_t)i >= buf_len * 8) {
    return;
  }
  int byte = i / 8;
  int bit = 7 - (i % 8);

  if (val) {
    buf[byte] |= (1u << bit);
  } else {
    buf[byte] &= ~(1u << bit);
  }
}

/* int dd_graphic_get_pixel(int x, int y, int width, unsigned char *buf, */
/*                          uint32_t buf_len) { */
/*   if (x < 0 || y < 0) { */
/*     return -1; */
/*   } */

/*   int bit = width * y + x; */

/*   return dd_graphic_get_bit(bit, buf, buf_len); */
/* } */

int dd_graphic_get_pixel(int x, int y, int width, unsigned char *buf,
                         uint32_t buf_len) {
  if (x < 0 || y < 0) {
    return -1;
  }

  int bit = width * y + x;

  return dd_graphic_get_bit(bit, buf, buf_len);
  
}
