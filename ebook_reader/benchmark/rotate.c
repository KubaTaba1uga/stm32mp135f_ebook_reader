#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timing.h"

unsigned char *dd_wvs75v2b_rotate(int width, int heigth,
				  unsigned char *buf,
				  int buf_len) {
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

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("%s <x> <y>\n", argv[0]);
    return 1;
  }
  int x = atoi(argv[1]);
  int y = atoi(argv[2]);
  unsigned  char *buf = malloc(x * y / 8);
  memset(buf, 0xFF, x * y / 8);
  
  long long t0 = now_ns();

  unsigned char *rotated =
      dd_wvs75v2b_rotate(x, y, buf, x*y/8);

  long long t1 = now_ns();
  print_ms("Cat rotated", (t1 - t0));

  (void)rotated;
  (void)buf;  
  free(rotated);
  free(buf);  
  return 0;
}
