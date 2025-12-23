/*
  This file is meant to test whether GPIO used by display are working properly
  on the board. We are using the same library as the waveshare driver.

 Test GPIO  |  Display GPIO
        H6 <-- A4
        D1 --> G3
        B1 --> C2
        C0 <-- I0

*/

#include <gpiod.h>
#include <stdio.h>

#define DIR_INPUT 1
#define DIR_OUTPUT 0

int configure_gpio_direction(struct gpiod_chip *chip, int pin, int direction);
int read_gpio(struct gpiod_chip *chip, int Pin);
int write_gpio(struct gpiod_chip *chip, int Pin, int value);
void gpio_pin_close(struct gpiod_chip *chip, int Pin);

static struct gpiod_chip *a_gpiochip;
static struct gpiod_chip *b_gpiochip;
static struct gpiod_chip *c_gpiochip;
static struct gpiod_chip *d_gpiochip;
static struct gpiod_chip *g_gpiochip;
static struct gpiod_chip *h_gpiochip;
static struct gpiod_chip *i_gpiochip;

int main(void) {
  a_gpiochip = gpiod_chip_open("/dev/gpiochip0");
  if (a_gpiochip == NULL) {
    puts("gpiochip0 Export Failed\n");
    return -1;
  }

  b_gpiochip = gpiod_chip_open("/dev/gpiochip1");
  if (b_gpiochip == NULL) {
    puts("gpiochip1 Export Failed\n");
    return -1;
  }

  c_gpiochip = gpiod_chip_open("/dev/gpiochip2");
  if (c_gpiochip == NULL) {
    puts("gpiochip2 Export Failed\n");
    return -1;
  }

  d_gpiochip = gpiod_chip_open("/dev/gpiochip3");
  if (d_gpiochip == NULL) {
    puts("gpiochip3 Export Failed\n");
    return -1;
  }

  g_gpiochip = gpiod_chip_open("/dev/gpiochip6");
  if (g_gpiochip == NULL) {
    puts("gpiochip6 Export Failed\n");
    return -1;
  }

  h_gpiochip = gpiod_chip_open("/dev/gpiochip7");
  if (h_gpiochip == NULL) {
    puts("gpiochip7 Export Failed\n");
    return -1;
  }

  i_gpiochip = gpiod_chip_open("/dev/gpiochip8");
  if (i_gpiochip == NULL) {
    puts("gpiochip8 Export Failed\n");
    return -1;
  }

  if (configure_gpio_direction(a_gpiochip, 4, DIR_OUTPUT) != 0) {
    puts("A4 dir config failed\n");
    return -1;
  }

  if (configure_gpio_direction(b_gpiochip, 1, DIR_INPUT) != 0) {
    puts("B1 dir config failed\n");
    return -1;
  }

  if (configure_gpio_direction(c_gpiochip, 0, DIR_INPUT) != 0) {
    puts("C0 dir config failed\n");
    return -1;
  }

  if (configure_gpio_direction(c_gpiochip, 2, DIR_OUTPUT) != 0) {
    puts("C2 dir config failed\n");
    return -1;
  }

  if (configure_gpio_direction(d_gpiochip, 1, DIR_OUTPUT) != 0) {
    puts("D1 dir config failed\n");
    return -1;
  }

  if (configure_gpio_direction(g_gpiochip, 3, DIR_INPUT) != 0) {
    puts("G3 dir config failed\n");
    return -1;
  }
  if (configure_gpio_direction(h_gpiochip, 6, DIR_INPUT) != 0) {
    puts("G6 dir config failed\n");
    return -1;
  }

  if (configure_gpio_direction(i_gpiochip, 0, DIR_OUTPUT) != 0) {
    puts("I0 dir config failed\n");
    return -1;
  }

  write_gpio(a_gpiochip, 4, 0);
  if (read_gpio(h_gpiochip, 6) != 0) {
    puts("gpioh6 value != 0\n");
    return -1;
  } else {
    puts("gpioh6 value == 0\n");
  }
  write_gpio(a_gpiochip, 4, 1);
  if (read_gpio(h_gpiochip, 6) != 1) {
    puts("gpioh6 value != 1\n");
    return -1;
  } else {
    puts("gpioh6 value == 1\n");
  }

  write_gpio(d_gpiochip, 1, 0);
  if (read_gpio(g_gpiochip, 3) != 0) {
    puts("gpiog3 value != 0\n");
    return -1;
  } else {
    puts("gpiog3 value == 0\n");
  }
  write_gpio(d_gpiochip, 1, 1);
  if (read_gpio(g_gpiochip, 3) != 1) {
    puts("gpiog3 value != 1\n");
    return -1;
  } else {
    puts("gpiog3 value == 1\n");
  }

  write_gpio(c_gpiochip, 2, 0);
  if (read_gpio(b_gpiochip, 1) != 0) {
    puts("gpiob1 value != 0\n");
    return -1;
  } else {
    puts("gpiob1 value == 0\n");
  }
  write_gpio(c_gpiochip, 2, 1);
  if (read_gpio(b_gpiochip, 1) != 1) {
    puts("bpiob1 value != 1\n");
    return -1;
  } else {
    puts("gpiob1 value == 1\n");
  }

  write_gpio(i_gpiochip, 0, 0);
  if (read_gpio(c_gpiochip, 0) != 0) {
    puts("gpioc0 value != 0\n");
    return -1;
  } else {
    puts("gpioc0 value == 0\n");
  }
  write_gpio(i_gpiochip, 0, 1);
  if (read_gpio(c_gpiochip, 0) != 1) {
    puts("ipioc0 value != 1\n");
    return -1;
  } else {
    puts("gpioc0 value == 1\n");
  }

  gpio_pin_close(a_gpiochip, 4);
  gpio_pin_close(b_gpiochip, 1);
  gpio_pin_close(c_gpiochip, 0);
  gpio_pin_close(c_gpiochip, 2);
  gpio_pin_close(d_gpiochip, 1);
  gpio_pin_close(g_gpiochip, 3);
  gpio_pin_close(h_gpiochip, 6);
  gpio_pin_close(i_gpiochip, 0);

  gpiod_chip_close(a_gpiochip);
  gpiod_chip_close(b_gpiochip);
  gpiod_chip_close(c_gpiochip);
  gpiod_chip_close(d_gpiochip);
  gpiod_chip_close(g_gpiochip);
  gpiod_chip_close(i_gpiochip);

  puts("DONE");

  return 0;
}

// 1 == INPUT, 0 == OUTPUT
int configure_gpio_direction(struct gpiod_chip *chip, int pin, int direction) {
  struct gpiod_line *gpioline = gpiod_chip_get_line(chip, pin);
  int ret = 0;
  if (gpioline == NULL) {
    printf("Export Failed: Pin%d\n", pin);
    return -1;
  }
  if (direction == 1) {
    ret = gpiod_line_request_input_flags(
        gpioline, "gpio", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_DOWN);
    if (ret != 0) {
      printf("Export Failed: Pin%d\n", pin);
      return -1;
    }
    printf("Pin%d:intput\r\n", pin);
  } else {
    ret = gpiod_line_request_output_flags(
        gpioline, "gpio", 0, GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_DOWN);
    if (ret != 0) {
      printf("Export Failed: Pin%d\n", pin);
      return -1;
    }
    printf("Pin%d:Output\r\n", pin);
  }

  return 0;
}

int read_gpio(struct gpiod_chip *chip, int Pin) {
  struct gpiod_line *gpioline = gpiod_chip_get_line(chip, Pin);
  int ret = 0;
  if (gpioline == NULL) {
    printf("Export Failed: Pin%d\n", Pin);
    return -1;
  }

  ret = gpiod_line_get_value(gpioline);
  if (ret < 0) {
    printf("failed to read value!\n");
    return -1;
  }

  return (ret);
}

int write_gpio(struct gpiod_chip *chip, int Pin, int value) {
  struct gpiod_line *gpioline = gpiod_chip_get_line(chip, Pin);
  int ret = 0;
  if (gpioline == NULL) {
    printf("Export Failed: Pin%d\n", Pin);
    return -1;
  }

  ret = gpiod_line_set_value(gpioline, value);
  if (ret != 0) {
    printf("failed to write value! : Pin%d\n", Pin);
    return -1;
  }

  return 0;
}

void gpio_pin_close(struct gpiod_chip *chip, int Pin) {
  struct gpiod_line *gpioline = gpiod_chip_get_line(chip, Pin);
  if (gpioline == NULL) {
    printf("Export Failed: Pin%d\n", Pin);
    return;
  }

  gpiod_line_release(gpioline);

  printf("Unexport: Pin%d\r\n", Pin);
}
