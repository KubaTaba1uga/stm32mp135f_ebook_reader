#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "timing.h"

static const unsigned char *pdf_get_page(const char *path, int x, int y,
                                         unsigned char *buf, int buf_len) {
  char cmd_buf[4096] = {0};
  snprintf(cmd_buf, sizeof(cmd_buf),
           "/usr/bin/pdftoppm -f %d -l %d -scale-to-x %d -scale-to-y %d "
           "-png -mono %s",
           10, 10, x, y, path);
  FILE *pdfinfo = popen(cmd_buf, "r");
  if (!pdfinfo) {
    perror("POPEN");
    return NULL;
  }

  fread(buf, 1, buf_len, pdfinfo);

  pclose(pdfinfo);

  return buf;
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("%s <path> <x> <y>\n", argv[0]);
    return 1;
  }
  
  int x = atoi(argv[2]);
  int y = atoi(argv[3]);
  int buf_len = x*y*4 ; // ARGB
  unsigned  char *buf = malloc(buf_len);
  memset(buf, 0xFF, buf_len);

  
  long long t0 = now_ns();

  const unsigned char *page =
    pdf_get_page(argv[1], x, y, buf, buf_len);

  long long t1 = now_ns();
  print_ms("Page generated", (t1 - t0));

  (void)page;
  (void)buf;  
  free(buf);  
  return 0;
}

