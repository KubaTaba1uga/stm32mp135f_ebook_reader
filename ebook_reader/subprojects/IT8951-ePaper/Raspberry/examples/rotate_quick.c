#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "cat_big.c"
#include "cat_big_not_rot.c"
#include "rotation_tables.c"

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
  struct Rotation *rot;  
  int v;

  unsigned char *dst = malloc(buf_len);
  for (int i = 0; i < sizeof(rotation_table) / sizeof(struct Rotation); i++) {
    rot = &rotation_table[i];
    v = (buf[rot->src_byte] >> rot->src_bit) & 1;
    if (v) {
        buf[rot->dst_byte] |= (1u << rot->dst_bit);
      } else {
        buf[rot->dst_byte] &= ~(1u << rot->dst_bit);
      }

  }

  return dst;
}

