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

bool enable_gpiod_chip_open_mock = false;
int gpiod_chip_open_mock_called = 0;
struct gpiod_chip *__real_gpiod_chip_open(const char *path);
struct gpiod_chip *__wrap_gpiod_chip_open(const char *path) {
  if (!enable_gpiod_chip_open_mock) {
    return __real_gpiod_chip_open(path);
  }

  gpiod_chip_open_mock_called++;
  printf("%s mocked\n", __func__);

  struct gpiod_chip *gpiod = dd_malloc(sizeof(struct gpiod_chip));
  gpiod->path = strdup(path);
  return gpiod;
}

bool enable_gpiod_chip_close_mock = false;
int gpiod_chip_close_mock_called = 0;
void __real_gpiod_chip_close(struct gpiod_chip *chip);
void __wrap_gpiod_chip_close(struct gpiod_chip *chip) {
  if (!enable_gpiod_chip_close_mock) {
    return __real_gpiod_chip_close(chip);
  }

  gpiod_chip_close_mock_called++;
  printf("%s mocked\n", __func__);

  free(chip->path);
  free(chip);
}
