#include "../lib/Config/DEV_Config.h"
#include "../lib/GUI/GUI_BMPfile.h"
#include "Raspberry/lib/GUI/GUI_Paint.h"
#include "Raspberry/lib/e-Paper/EPD_IT8951.h"
#include "cat_big.c"
#include "cat_big_not_rot.c"
#include "example.h"
#include "unistd.h"

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

void Handler(int signo) {
  Debug("\r\nHandler:exit\r\n");
  if (Refresh_Frame_Buf != NULL) {
    free(Refresh_Frame_Buf);
    Debug("free Refresh_Frame_Buf\r\n");
    Refresh_Frame_Buf = NULL;
  }
  if (Panel_Frame_Buf != NULL) {
    free(Panel_Frame_Buf);
    Debug("free Panel_Frame_Buf\r\n");
    Panel_Frame_Buf = NULL;
  }
  if (Panel_Area_Frame_Buf != NULL) {
    free(Panel_Area_Frame_Buf);
    Debug("free Panel_Area_Frame_Buf\r\n");
    Panel_Area_Frame_Buf = NULL;
  }
  if (bmp_src_buf != NULL) {
    free(bmp_src_buf);
    Debug("free bmp_src_buf\r\n");
    bmp_src_buf = NULL;
  }
  if (bmp_dst_buf != NULL) {
    free(bmp_dst_buf);
    Debug("free bmp_dst_buf\r\n");
    bmp_dst_buf = NULL;
  }
  if (Dev_Info.Panel_W != 0) {
    Debug("Going to sleep\r\n");
    EPD_IT8951_Sleep();
  }
  DEV_Module_Exit();
  exit(0);
}

#include <stdio.h>
#include <time.h>

static inline long long now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static inline void print_ms(const char *name, long long dt_ns) {
  printf("%s: %.3f ms\n", name, (double)dt_ns / 1e6);
}

#define TIME_CALL(label, expr)                                                 \
  do {                                                                         \
    long long t0 = now_ns();                                                   \
    (expr);                                                                    \
    long long t1 = now_ns();                                                   \
    print_ms((label), (t1 - t0));                                              \
  } while (0)

int main(int argc, char *argv[]) {
  // Exception handling:ctrl + c
  signal(SIGINT, Handler);

  if (argc < 2) {
    Debug("Please input VCOM value on FPC cable!\r\n");
    Debug("Example: sudo ./epd -2.51 [colour id]\r\n");
    exit(1);
  }
  if (argc != 3 && argc != 4) {
    Debug("Please input e-Paper display mode!\r\n");
    Debug("Example: sudo ./epd -2.51 0 or sudo ./epd -2.51 1\r\n");
    Debug("Now, 10.3 inch glass panle is mode1, else is mode0\r\n");
    Debug("If you don't know what to type in just type 0 \r\n");
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


  int buf_len = Dev_Info.Panel_W * Dev_Info.Panel_H / 2;
  uint8_t *buf = malloc(buf_len);

  for (int i = 0; i < 16; i++) {
    memset(buf, 0x00, buf_len);
    TIME_CALL("1bp_Refresh GC16 - Black",
              EPD_IT8951_1bp_Refresh(buf, 0, 0, Dev_Info.Panel_W,
                                     Dev_Info.Panel_H - 4, GC16_Mode,
                                     Init_Target_Memory_Addr, true));
    sleep(5);

    TIME_CALL("1bp_Refresh GC16 - Cat",
              EPD_IT8951_1bp_Refresh(
                                     big_cat,
                  0, 0, Dev_Info.Panel_W, Dev_Info.Panel_H - 4, GC16_Mode,
                  Init_Target_Memory_Addr, true));
    sleep(5);

    TIME_CALL("1bp_Refresh GC16 - Cat rotated",
              EPD_IT8951_1bp_Refresh(
                  dd_wvs75v2b_rotate(Dev_Info.Panel_H, Dev_Info.Panel_W,
                                     big_cat_not_rot, sizeof(big_cat_not_rot)),
                  0, 0, Dev_Info.Panel_W, Dev_Info.Panel_H, GC16_Mode,
                  Init_Target_Memory_Addr, true));
    sleep(5);

    memset(buf, 0xFF, buf_len);
    TIME_CALL("1bp_Refresh GC16 - White",
              EPD_IT8951_1bp_Refresh(buf, 0, 0, Dev_Info.Panel_W,
                                     Dev_Info.Panel_H - 4, GC16_Mode,
                                     Init_Target_Memory_Addr, true));
    sleep(5);

    EPD_IT8951_Sleep();
    sleep(5);    
  }

  EPD_IT8951_Sleep();

  DEV_Module_Exit();

  free(buf);
  return 0;
}

/**
   This function is heavilly optimized, do not touch unless you know what you're doing!
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
