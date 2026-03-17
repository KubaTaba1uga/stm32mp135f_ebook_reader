#include <EPD_IT8951.h>
#include <assert.h>
#include <errno.h>
#include <lvgl.h>
#include <math.h>
#include <stdio.h>
#include <cairo.h>

#include "display/display.h"
#include "misc/lv_color.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/settings.h"
#include "utils/time.h"

struct Display {
  lv_group_t *lv_ingroup;
  lv_display_t *lv_disp;
  IT8951_Dev_Info dev_info;
  UDOUBLE init_mem_addr;
  struct {
    unsigned char *buf;
    int len;
  } render;
};

static struct Trace display_trace = {0};
static const int ui_display_it8951_heigth = 1872;
static const int ui_display_it8951_width = 1404; // Display is 1404 but lvgl in
                                                 // i1 needs byte aligned values
                                                 // 1404 % 8 != 0

static void display_flush_callback(lv_display_t *display, const lv_area_t *area,
                                   uint8_t *px_map);

err_t display_init(display_t *out) {
  display_t display = *out = mem_malloc(sizeof(struct Display));
  *display = (struct Display){0};

  display->lv_disp =
      lv_display_create(ui_display_it8951_width, ui_display_it8951_heigth);
  if (!display->lv_disp) {
    goto error_out;
  }

  if (DEV_Module_Init() != 0) {
    err_o = err_errnos(EINVAL, "Cannot initialize ET8951 driver");
    goto error_disp_cleanup;
  }

  // Configure IT8951
  UWORD VCOM = (UWORD)(fabs(-1.50) * 1000);
  display->dev_info = EPD_IT8951_Init(VCOM);
  display->init_mem_addr =
      display->dev_info.Memory_Addr_L | (display->dev_info.Memory_Addr_H << 16);

  printf("display->dev_info.Panel_W=%d\n", display->dev_info.Panel_W);
  printf("display->dev_info.Panel_H=%d\n", display->dev_info.Panel_H);
  printf("ui_display_it8951_width=%d\n", ui_display_it8951_width);
  printf("ui_display_it8951_heigth=%d\n", ui_display_it8951_heigth);
  
  EPD_IT8951_Clear_Refresh(display->dev_info, display->init_mem_addr,
                           INIT_Mode);
  // Configure LVGL
  lv_indev_t *lv_indev =
      lv_evdev_create(LV_INDEV_TYPE_KEYPAD, settings_input_path);
  lv_indev_set_display(lv_indev, display->lv_disp);
  display->lv_ingroup = lv_group_create();
  lv_indev_set_group(lv_indev, display->lv_ingroup);

  display->render.len =
      (display->dev_info.Panel_W * display->dev_info.Panel_H * 4);
  display->render.buf = mem_malloc(display->render.len);

  lv_display_set_color_format(display->lv_disp, LV_COLOR_FORMAT_ARGB8888);
  lv_display_set_user_data(display->lv_disp, display);
  lv_display_set_flush_cb(display->lv_disp, display_flush_callback);
  lv_display_set_buffers(display->lv_disp, display->render.buf, NULL,
                         display->render.len, LV_DISPLAY_RENDER_MODE_FULL);
  return 0;

error_disp_cleanup:
  lv_display_delete(display->lv_disp);
error_out:
  mem_free(display);
  return err_o;
}

void display_destroy(display_t *out) {
  if (!out || !*out) {
    return;
  }

  if ((*out)->lv_ingroup) {
    lv_group_delete((*out)->lv_ingroup);
  }

  mem_free(*out);
  *out = NULL;
}


void display_set_trace(void) { display_trace = trace_start("render display"); };

void display_add_to_ingroup(display_t display, void *wx) {
  lv_group_add_obj(display->lv_ingroup, wx);
}

void display_del_from_ingroup(display_t _, void *wx) {
  lv_group_remove_obj(wx);
}

int display_get_x(display_t display) {
  return lv_display_get_horizontal_resolution(NULL);
};
int display_get_y(display_t display) {
  return lv_display_get_vertical_resolution(NULL);
}

unsigned char *dd_wvs75v2b_rotate(int width, int heigth, unsigned char *buf,
                                  int buf_len) {
  display_trace = trace_start(__func__);
  
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

  trace_end(&display_trace);
  
  return dst;
}

static unsigned char *graphic_argb32_to_i1(int w, int h, const uint8_t *src,
                                           int stride) {
  display_trace = trace_start(__func__);  
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

  trace_end(&display_trace);
  
  return dst;
}


void display_refresh(UBYTE *Frame_Buf, UWORD X, UWORD Y, UWORD W, UWORD H,
                     UBYTE Mode, UDOUBLE Target_Memory_Addr,
                     bool Packed_Write) {
  display_trace = trace_start(__func__);
  
  EPD_IT8951_1bp_Refresh(Frame_Buf, X, Y, W,
                         H, GC16_Mode,
                         Target_Memory_Addr, Packed_Write);

  trace_end(&display_trace);  
}

static void display_flush_callback(lv_display_t *display, const lv_area_t *area,
                                   uint8_t *px_map) {
  puts(__func__);
  trace_end(&display_trace);
  
  struct Display *mydisp = lv_display_get_user_data(display);


  
  uint8_t *dst = graphic_argb32_to_i1(
      mydisp->dev_info.Panel_W, mydisp->dev_info.Panel_H, px_map,
      cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32,mydisp->dev_info.Panel_W));
  
  unsigned char *final = dd_wvs75v2b_rotate(
      mydisp->dev_info.Panel_H, mydisp->dev_info.Panel_W, dst,
      mydisp->dev_info.Panel_W * mydisp->dev_info.Panel_H / 8);

  display_refresh(final, 0, 0, mydisp->dev_info.Panel_W,
                         mydisp->dev_info.Panel_H, GC16_Mode,
                         mydisp->init_mem_addr, true);

  free(dst);
  free(final);  
  lv_display_flush_ready(display);
  printf("%s done\n", __func__);
}

void display_panic(display_t display) {
  puts(__func__);
  EPD_IT8951_Reset();
  EPD_IT8951_Sleep();
}
