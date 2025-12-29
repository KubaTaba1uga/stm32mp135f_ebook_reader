#define _GNU_SOURCE
#include "utils/mem.h"
#include <stdbool.h>
#include <string.h>

struct gpiod_chip {
  char *path;
};

struct gpiod_line {
  int pin;
};

struct gpiod_chip *
gpiod_chip_open(const char *path) {
  struct gpiod_chip *gpiod = dd_malloc(sizeof(struct gpiod_chip));
  gpiod->path = strdup(path);
  return gpiod;
};

void gpiod_chip_close(struct gpiod_chip *chip) {
  free(chip->path);
  free(chip);
};

void gpiod_line_release(struct gpiod_line *line) {};
