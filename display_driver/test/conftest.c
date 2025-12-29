#define _GNU_SOURCE
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/mem.h"

#define GPIOD_MOCK_MAX_LINES 256u

struct gpiod_chip {
  char *path;
};

struct gpiod_line {
  int pin;
  bool requested_out;
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

static struct gpiod_line gpiod_lines_pool[GPIOD_MOCK_MAX_LINES];

/* Optional helper to reset the pool (call from setUp) */
void gpiod_mock_reset_lines_pool(void) {
  memset(gpiod_lines_pool, 0, sizeof(gpiod_lines_pool));
}

bool enable_gpiod_chip_get_line_mock = false;
int gpiod_chip_get_line_mock_called = 0;
struct gpiod_line *__real_gpiod_chip_get_line(struct gpiod_chip *chip,
                                              unsigned int offset);
struct gpiod_line *__wrap_gpiod_chip_get_line(struct gpiod_chip *chip,
                                              unsigned int offset) {
  if (!enable_gpiod_chip_get_line_mock) {
    return __real_gpiod_chip_get_line(chip, offset);
  }

  gpiod_chip_get_line_mock_called++;
  printf("%s mocked\n", __func__);

  if (offset >= GPIOD_MOCK_MAX_LINES) {
    return NULL;
  }

  gpiod_lines_pool[offset].pin = (int)offset;
  return &gpiod_lines_pool[offset];
}

bool enable_gpiod_line_request_output_mock = false;
int gpiod_line_request_output_mock_called = 0;
int __real_gpiod_line_request_output(struct gpiod_line *line,
                                     const char *consumer, int default_val);
int __wrap_gpiod_line_request_output(struct gpiod_line *line,
                                     const char *consumer, int default_val) {
  if (!enable_gpiod_line_request_output_mock) {
    return __real_gpiod_line_request_output(line, consumer, default_val);
  }

  gpiod_line_request_output_mock_called++;
  printf("%s mocked\n", __func__);

  if (line) {
    line->requested_out = true;
  }
  
  return 0;
}

bool enable_gpiod_line_request_input_mock = false;
int gpiod_line_request_input_mock_called = 0;
int __real_gpiod_line_request_input(struct gpiod_line *line,
                                    const char *consumer);
int __wrap_gpiod_line_request_input(struct gpiod_line *line,
                                    const char *consumer) {
  if (!enable_gpiod_line_request_input_mock) {
    return __real_gpiod_line_request_input(line, consumer);
  }

  gpiod_line_request_input_mock_called++;
  printf("%s mocked\n", __func__);

  if (line) {
    line->requested_out = false;
  }
  
  return 0;
}

bool enable_gpiod_line_release_mock = false;
int gpiod_line_release_mock_called = 0;
void __real_gpiod_line_release(struct gpiod_line *line);
void __wrap_gpiod_line_release(struct gpiod_line *line) {
  (void)line;

  if (!enable_gpiod_line_release_mock) {
    return __real_gpiod_line_release(line);
  }

  gpiod_line_release_mock_called++;
  printf("%s mocked\n", __func__);
}

bool enable_gpiod_line_request_output_flags_mock = false;
int gpiod_line_request_output_flags_mock_called = 0;
int __real_gpiod_line_request_output_flags(struct gpiod_line *line,
                                           const char *consumer,
                                           int flags,
                                           int default_val);

int __wrap_gpiod_line_request_output_flags(struct gpiod_line *line,
                                           const char *consumer,
                                           int flags,
                                           int default_val) {
  if (!enable_gpiod_line_request_output_flags_mock) {
    return __real_gpiod_line_request_output_flags(line, consumer, flags, default_val);
  }

  gpiod_line_request_output_flags_mock_called++;
  if (line) line->requested_out = true;
  return 0;
}
