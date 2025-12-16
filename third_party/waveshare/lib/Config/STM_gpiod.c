/*****************************************************************************
* | File        :   RPI_GPIOD.c
* | Author      :   Waveshare team
* | Function    :   Drive GPIO
* | Info        :   Read and write gpio
*----------------
* |	This version:   V1.0
* | Date        :   2023-11-15
* | Info        :   Basic version
*
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# GPIOD_IN the Software without restriction, including without limitation the
rights # to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell # copies of the Software, and to permit persons to  whom the Software is #
furished to do so, subject to the folGPIOD_LOWing conditions:
#
# The above copyright notice and this permission notice shall be included
GPIOD_IN # all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. GPIOD_IN NO EVENT SHALL
THE # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
LIABILITY WHETHER GPIOD_IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, # GPIOD_OUT OF OR GPIOD_IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS GPIOD_IN # THE SOFTWARE.
#
******************************************************************************/
#include "STM_gpiod.h"
#include <fcntl.h>
#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct GPIOD {
  int pin;
  char bank;
};

static struct gpiod_chip *a_gpiochip;
static struct gpiod_chip *c_gpiochip;
static struct gpiod_chip *g_gpiochip;
static struct gpiod_chip *h_gpiochip;
static struct gpiod_chip *i_gpiochip;
int ret;

static struct GPIOD gpiod_regs[255] = {0};

int STM_GPIOD_register(int pin, char bank) {
  struct GPIOD *free_gpio_slot = NULL;
  int i;
  for (i = 0; i < sizeof(gpiod_regs) / sizeof(struct GPIOD); i++) {
    if (!gpiod_regs[i].bank) {
      free_gpio_slot = &gpiod_regs[i];
      break;
    }
  }

  if (!free_gpio_slot) {
    return -1;
  }

  free_gpio_slot->bank = bank;
  free_gpio_slot->pin = pin;

  return i;
};

static int reg_to_pin(int reg) { return gpiod_regs[reg].pin; }

static struct gpiod_chip *reg_to_chip(int reg) {
  switch (gpiod_regs[reg].bank) {
  case 'A':
    return a_gpiochip;
  case 'C':
    return c_gpiochip;
  case 'G':
    return g_gpiochip;
  case 'H':
    return h_gpiochip;
  case 'I':
    return i_gpiochip;
  default:
    return NULL;
  }
};

int STM_GPIOD_Export() {
  a_gpiochip = gpiod_chip_open("/dev/gpiochip0");
  if (a_gpiochip == NULL) {
    STM_GPIOD_Debug("gpiochip0 Export Failed\n");
    return -1;
  }

  c_gpiochip = gpiod_chip_open("/dev/gpiochip2");
  if (c_gpiochip == NULL) {
    STM_GPIOD_Debug("gpiochip2 Export Failed\n");
    return -1;
  }

  g_gpiochip = gpiod_chip_open("/dev/gpiochip6");
  if (g_gpiochip == NULL) {
    STM_GPIOD_Debug("gpiochip6 Export Failed\n");
    return -1;
  }

  // This GPIO chip may be not needed cause this gpios are handled by SPI driver.
  h_gpiochip = gpiod_chip_open("/dev/gpiochip7");
  if (h_gpiochip == NULL) {
    STM_GPIOD_Debug("gpiochip7 Export Failed\n");
    return -1;
  }

  i_gpiochip = gpiod_chip_open("/dev/gpiochip8");
  if (i_gpiochip == NULL) {
    STM_GPIOD_Debug("gpiochip8 Export Failed\n");
    return -1;
  }

  return 0;
}

int STM_GPIOD_Unexport(int Pin) {
  struct gpiod_line *gpioline = gpiod_chip_get_line(reg_to_chip(Pin), reg_to_pin(Pin));
  if (gpioline == NULL) {
    STM_GPIOD_Debug("Export Failed: Pin%d\n", Pin);
    return -1;
  }

  gpiod_line_release(gpioline);

  STM_GPIOD_Debug("Unexport: Pin%d\r\n", Pin);

  return 0;
}

int STM_GPIOD_Unexport_GPIO(void) {
  gpiod_chip_close(a_gpiochip);
  gpiod_chip_close(c_gpiochip);
  gpiod_chip_close(g_gpiochip);
  gpiod_chip_close(h_gpiochip);
  gpiod_chip_close(i_gpiochip);
  
  return 0;
}

int STM_GPIOD_Direction(int Pin, int Dir) {
  struct gpiod_line *  gpioline = gpiod_chip_get_line(reg_to_chip(Pin), reg_to_pin(Pin));
  if (gpioline == NULL) {
    STM_GPIOD_Debug("Export Failed: Pin%d\n", Pin);
    return -1;
  }

  if (Dir == STM_GPIOD_IN) {
    ret = gpiod_line_request_input_flags(gpioline, "gpio", GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_DOWN);    
    // ret = gpiod_line_request_input(gpioline, "gpio"); TO-DO: streamline this change
    if (ret != 0) {
      STM_GPIOD_Debug("Export Failed: Pin%d\n", Pin);
      return -1;
    }
    STM_GPIOD_Debug("Pin%d:intput\r\n", Pin);
  } else {
    ret = gpiod_line_request_output(gpioline, "gpio", 0);
    if (ret != 0) {
      STM_GPIOD_Debug("Export Failed: Pin%d\n", Pin);
      return -1;
    }
    STM_GPIOD_Debug("Pin%d:Output\r\n", Pin);
  }
  return 0;
}

int STM_GPIOD_Read(int Pin) {
  struct gpiod_line *  gpioline = gpiod_chip_get_line(reg_to_chip(Pin), reg_to_pin(Pin));
  if (gpioline == NULL) {
    STM_GPIOD_Debug("Export Failed: Pin%d\n", Pin);
    return -1;
  }

  ret = gpiod_line_get_value(gpioline);
  if (ret < 0) {
    STM_GPIOD_Debug("failed to read value!\n");
    return -1;
  }

  return (ret);
}

int STM_GPIOD_Write(int Pin, int value) {
  struct gpiod_line *  gpioline = gpiod_chip_get_line(reg_to_chip(Pin), reg_to_pin(Pin));
  if (gpioline == NULL) {
    STM_GPIOD_Debug("Export Failed: Pin%d\n", Pin);
    return -1;
  }

  ret = gpiod_line_set_value(gpioline, value);
  if (ret != 0) {
    STM_GPIOD_Debug("failed to write value! : Pin%d\n", Pin);
    return -1;
  }
  return 0;
}
