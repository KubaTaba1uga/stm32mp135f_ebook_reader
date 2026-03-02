/*****************************************************************************
* | File        :   GPIOD.c
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
#include "RPI_gpiod.h"
#include <fcntl.h>
#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct gpiod_chip *gpiochip2;
struct gpiod_chip *gpiochip6;
struct gpiod_chip *gpiochip7;
struct gpiod_line *gpioline;
int ret;

int GPIOD_Export() {
  gpiochip2 = gpiod_chip_open("/dev/gpiochip2");
  if (gpiochip2 == NULL) {
    GPIOD_Debug("gpiochip2 Export Failed\n");
    return -1;
  }

  gpiochip6 = gpiod_chip_open("/dev/gpiochip6");
  if (gpiochip6 == NULL) {
    GPIOD_Debug("gpiochip6 Export Failed\n");
    return -1;
  }

  gpiochip7 = gpiod_chip_open("/dev/gpiochip7");
  if (gpiochip7 == NULL) {
    GPIOD_Debug("gpiochip7 Export Failed\n");
    return -1;
  }

  
  return 0;
}

static struct gpiod_chip *GPIOD_Chip(int Pin) {
  if (Pin == GPIO_PIN_RST) {
    return gpiochip2;
  }
  if (Pin == GPIO_PIN_BUSY) {
    return gpiochip6;
  }
  if (Pin == GPIO_PIN_CS) {
    return gpiochip7;
  }  
  puts("INVALID GPIOCHIP");
  return NULL;
}

static struct gpiod_line *GPIOD_Line(int Pin) {
  return gpiod_chip_get_line(GPIOD_Chip(Pin), Pin);
}

int GPIOD_Unexport(int Pin) {
  gpioline = GPIOD_Line(Pin);
  if (gpioline == NULL) {
    GPIOD_Debug("Export Failed: Pin%d\n", Pin);
    return -1;
  }

  gpiod_line_release(gpioline);

  GPIOD_Debug("Unexport: Pin%d\r\n", Pin);

  return 0;
}

int GPIOD_Unexport_GPIO(void) {
  gpiod_line_release(gpioline);
  gpiod_chip_close(gpiochip2);
  gpiod_chip_close(gpiochip6);
  return 0;
}

int GPIOD_Direction(int Pin, int Dir) {
  gpioline = GPIOD_Line(Pin);
  if (gpioline == NULL) {
    GPIOD_Debug("Export Failed: Pin%d\n", Pin);
    return -1;
  }

  if (Dir == GPIOD_IN) {
    ret = gpiod_line_request_input(gpioline, "gpio");
    if (ret != 0) {
      GPIOD_Debug("Export Failed: Pin%d\n", Pin);
      return -1;
    }
    GPIOD_Debug("Pin%d:intput\r\n", Pin);
  } else {
    ret = gpiod_line_request_output(gpioline, "gpio", 0);
    if (ret != 0) {
      GPIOD_Debug("Export Failed: Pin%d\n", Pin);
      return -1;
    }
    GPIOD_Debug("Pin%d:Output\r\n", Pin);
  }
  return 0;
}

int GPIOD_Read(int Pin) {
  gpioline = GPIOD_Line(Pin);
  if (gpioline == NULL) {
    GPIOD_Debug("Export Failed: Pin%d\n", Pin);
    return -1;
  }

  ret = gpiod_line_get_value(gpioline);
  if (ret < 0) {
    GPIOD_Debug("failed to read value!\n");
    return -1;
  }

  return (ret);
}

int GPIOD_Write(int Pin, int value) {
  gpioline = GPIOD_Line(Pin);
  if (gpioline == NULL) {
    GPIOD_Debug("Export Failed: Pin%d\n", Pin);
    return -1;
  }

  ret = gpiod_line_set_value(gpioline, value);
  if (ret != 0) {
    GPIOD_Debug("failed to write value! : Pin%d\n", Pin);
    return -1;
  }
  return 0;
}
