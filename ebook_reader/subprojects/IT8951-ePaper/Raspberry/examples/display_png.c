#include "../lib/Config/DEV_Config.h"
#include "Raspberry/lib/e-Paper/EPD_IT8951.h"
#include "example.h"
#include "timing.h"
#include "unistd.h"
#include <cairo.h>

#include <cairo/cairo-deprecated.h>
#include <cairo/cairo.h>
#include <math.h>

#include <signal.h> //signal()
#include <stdint.h>
#include <stdlib.h> //exit()
#include <string.h>

#define Enhance false

#define USE_Factory_Test false

#define USE_Normal_Demo true

#define USE_Touch_Panel false

UWORD VCOM = 2510;

IT8951_Dev_Info Dev_Info = {0, 0};
UWORD Panel_Width;
UWORD Panel_Height;
UDOUBLE Init_Target_Memory_Addr;
int epd_mode = 0; // 0: no rotate, no mirror
                  // 1: no rotate, horizontal mirror, for 10.3inch
                  // 2: no totate, horizontal mirror, for 5.17inch
                  // 3: no rotate, no mirror, isColor, for 6inch color

static unsigned char *dd_wvs75v2b_rotate(int width, int heigth,
                                         unsigned char *buf, int buf_len);
static unsigned char *graphic_argb32_to_i1(int w, int h, const uint8_t *src,
                                           int stride);

void Handler(int signo) {
  DEV_Module_Exit();
  exit(0);
}

int main(int argc, char *argv[]) {
  // Exception handling:ctrl + c
  signal(SIGINT, Handler);

  if (argc < 4) {
    Debug("Please input VCOM value on FPC cable!\r\n");
    Debug("Example: sudo ./epd -2.51 [colour id] path\r\n");
    exit(1);
  }

  // Init the BCM2835 Device
  if (DEV_Module_Init() != 0) {
    return -1;
  }

  double temp;
  sscanf(argv[1], "%lf", &temp);
  VCOM = (UWORD)(fabs(temp) * 1000);
  Debug("VCOM value:%d\r\n", VCOM);
  sscanf(argv[2], "%d", &epd_mode);
  Debug("Display mode:%d\r\n", epd_mode);
  Dev_Info = EPD_IT8951_Init(VCOM);

#if (Enhance)
  Debug("Attention! Enhanced driving ability, only used when the screen is "
        "blurred\r\n");
  Enhance_Driving_Capability();
#endif

  // get some important info from Dev_Info structure
  Panel_Width = Dev_Info.Panel_W;
  Panel_Height = Dev_Info.Panel_H;
  Init_Target_Memory_Addr =
      Dev_Info.Memory_Addr_L | (Dev_Info.Memory_Addr_H << 16);
  char *LUT_Version = (char *)Dev_Info.LUT_Version;
  if (strcmp(LUT_Version, "M641") == 0) {
    // 6inch e-Paper HAT(800,600), 6inch HD e-Paper HAT(1448,1072), 6inch HD
    // touch e-Paper HAT(1448,1072)
    A2_Mode = 4;
    Four_Byte_Align = true;
  } else if (strcmp(LUT_Version, "M841_TFAB512") == 0) {
    // Another firmware version for 6inch HD e-Paper HAT(1448,1072), 6inch HD
    // touch e-Paper HAT(1448,1072)
    A2_Mode = 6;
    Four_Byte_Align = true;
  } else if (strcmp(LUT_Version, "M841") == 0) {
    // 9.7inch e-Paper HAT(1200,825)
    A2_Mode = 6;
  } else if (strcmp(LUT_Version, "M841_TFA2812") == 0) {
    // 7.8inch e-Paper HAT(1872,1404)
    A2_Mode = 6;
  } else if (strcmp(LUT_Version, "M841_TFA5210") == 0) {
    // 10.3inch e-Paper HAT(1872,1404)
    A2_Mode = 6;
  } else {
    // default set to 6 as A2 Mode
    A2_Mode = 6;
  }
  Debug("A2 Mode:%d\r\n", A2_Mode);
  if (argc == 4) {
    puts(argv[3]);
  }
  TIME_CALL(
      "Clear_Refresh INIT",
      EPD_IT8951_Clear_Refresh(Dev_Info, Init_Target_Memory_Addr, INIT_Mode));
  puts("INIT MODE DONE");
  sleep(1);

  cairo_surface_t *cairo_surface = cairo_image_surface_create_from_png(argv[3]);
  uint8_t *buf = cairo_image_surface_get_data(cairo_surface);

  long long now = now_ns();
  uint8_t *dst = graphic_argb32_to_i1(
      Panel_Width, Panel_Height, buf,
      cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, Panel_Width));
  uint8_t *final = dd_wvs75v2b_rotate(Panel_Height, Panel_Width, dst,
                                      Panel_Width * Panel_Height / 8);

  print_ms("Convert and rotate", now_ns() - now);
  TIME_CALL("1bp_Refresh GC16 - PNG",
            EPD_IT8951_1bp_Refresh(final, 0, 0, Dev_Info.Panel_W,
                                   Dev_Info.Panel_H, GC16_Mode,
                                   Init_Target_Memory_Addr, true));
  sleep(5);

  EPD_IT8951_Sleep();

  DEV_Module_Exit();

  free(buf);
  return 0;
}

static unsigned char *graphic_argb32_to_i1(int w, int h, const uint8_t *src,
                                           int stride) {
  int dst_stride = (w + 7) / 8;
  int dst_len = dst_stride * h;
  unsigned char *dst = malloc(dst_len);
  memset(dst, 0x00, dst_stride * h); // 0 = white

  for (int y = 0; y < h; y++) {
    const uint32_t *row = (const uint32_t *)(src + y * stride);
    for (int x = 0; x < w; x++) {
      uint32_t p = row[x]; // 0xAARRGGBB on little-endian
      uint8_t r = (p >> 16) & 0xFF;
      uint8_t g = (p >> 8) & 0xFF;
      uint8_t b = (p >> 0) & 0xFF;

      uint16_t lum = (uint16_t)(r * 30 + g * 59 + b * 11) / 100;
      bool black = lum > 130;
      /* bool black = (r + g + b) != 0; */

      int byte_i = y * dst_stride + (x >> 3);

      /* int bit = 7 - (x & 7); // MSB */
      int bit = (x & 7); // LSB
      if (black) {
        dst[byte_i] |= (1u << bit);
      }
    }
  }

  return dst;
}

/**
   This function is heavilly optimized, do not touch unless you know what you're
   doing!
*/
static unsigned char *dd_wvs75v2b_rotate(int width, int height,
                                         unsigned char *buf, int buf_len) {
  unsigned char *dst = calloc(buf_len, 1);
  for (int y = 0; y < height; y++) {
    int src_row = y * width;
    for (int x = 0; x < width; x++) {
      int src_bit = src_row + (width - 1 - x);
      int dst_bit = x * height + y;
      int v = (buf[src_bit >> 3] >> (src_bit & 7)) & 1;
      if (v) {
        dst[dst_bit >> 3] |= (1 << (dst_bit & 7));
      }
    }
  }

  return dst;
}
