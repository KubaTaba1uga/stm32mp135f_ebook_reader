#include <string.h>
#include <assert.h>
#include <poppler.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
  This is quite good quality:
  pdftoppm -f 7 -l 7 /mnt/sdcard/Operating_Systems_From_0_to_1.pdf -png -mono >
page480x800.png

Display has res: 1872×1404

Build this example like this:

✦ ❯ gcc apps/main.c $(pkg-config --cflags --libs poppler) $(pkg-config --cflags
--libs poppler-glib) $(pkg-config --cflags --libs glib-2.0) $(pkg-config
--cflags --libs gio-2.0) $(pkg-config --cflags --libs cairo)


*/
static void pdf_render_page(PopplerDocument *doc, int x, int y);
static void pdf_render_page2(PopplerDocument *doc, int x, int y);

int main(int argc, char *argv[]) {
  if (argc < 4) {
    return 1;
  }
  (void)pdf_render_page2;
  (void)pdf_render_page;  
  
  const char *filename = argv[1];

  int bytes = strlen(filename) + strlen("file:///") + 1;
  char *uri = malloc(bytes);
  snprintf(uri, bytes, "file:///%s", filename);

  PopplerDocument *doc = poppler_document_new_from_file(uri, NULL, NULL);
  pdf_render_page2(doc, atoi(argv[2]), atoi(argv[3]));
  return 0;
}

struct PixelARGB {
  uint8_t b, g, r, a;
};

static void graphic_argb32_to_monochrome(uint8_t *dst, int w, int h,
                                         const uint8_t *src, int src_stride) {
  int dst_stride = w * sizeof(struct PixelARGB);
  memset(dst, 0xFF, dst_stride * h); // white

  for (int y = 0; y < h; y++) {
    const struct PixelARGB *src_row =
        (const struct PixelARGB *)(src + y * src_stride);
    struct PixelARGB *dst_row = (struct PixelARGB *)(dst + y * src_stride);

    for (int x = 0; x < w; x++) {
      const int threshold = 140;
      bool black = (src_row[x].r < threshold) || (src_row[x].g < threshold) ||
                   (src_row[x].b < threshold);

      uint8_t v = black ? 0x00 : 0xFF;
      dst_row[x] = (struct PixelARGB){.r = v, .g = v, .b = v, .a = 0xFF};
    }
  }
}

static void pdf_render_page(PopplerDocument *doc, int x, int y) {
  cairo_surface_t *surface;
  double page_x, page_y;
  cairo_t *cr;

  PopplerPage *page = poppler_document_get_page(doc, 6);
  assert(page != NULL);

  // Render high resolution image
  poppler_page_get_size(page, &page_x, &page_y);
  
  double scale_x, scale_y;
  scale_x = x / page_x;
  scale_y = y / page_y;  
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x, y);

  cr = cairo_create(surface);
  cairo_scale(cr, scale_x, scale_y);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);
  poppler_page_render(page, cr);
  cairo_surface_flush(surface);

  unsigned char *sdata = cairo_image_surface_get_data(surface);
  int sw = cairo_image_surface_get_width(surface);
  int sh = cairo_image_surface_get_height(surface);
  int stride = cairo_image_surface_get_stride(surface);
  int sdata_len = sw * sh * 4;
  unsigned char *new_sdata = malloc(sdata_len);
  graphic_argb32_to_monochrome(new_sdata, sw, sh, sdata, stride);
  memcpy(sdata, new_sdata, sdata_len);

  char buffer[1024];
  snprintf(buffer, 1024, "out-%dx%d.png", x, y)  ;
  cairo_surface_write_to_png(surface, buffer);

  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  g_object_unref(page);
}


static void pdf_render_page2(PopplerDocument *doc, int x, int y) {
  cairo_surface_t *surface;
  double page_x, page_y;
  cairo_t *cr;

  PopplerPage *page = poppler_document_get_page(doc, 50);
  assert(page != NULL);

  // Render high resolution image
  poppler_page_get_size(page, &page_x, &page_y);
  
  
  double scale_x, scale_y;
  scale_x = x / page_x;
  /* scale_y = page_y * scale_x;   */
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x, y * scale_x);
  cr = cairo_create(surface);
  cairo_scale(cr, scale_x, scale_x);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);
  poppler_page_render(page, cr);
  cairo_surface_flush(surface);

  cairo_surface_t *low_res_surface;
  /* double low_res_page_x, low_res_page_y; */
  cairo_t *low_res_cr;
  
  low_res_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x, y);

  low_res_cr = cairo_create(low_res_surface);
  cairo_set_source_surface(low_res_cr, surface, 0, 0);
  cairo_paint(low_res_cr);
  
  unsigned char *sdata = cairo_image_surface_get_data(low_res_surface);
  int sw = cairo_image_surface_get_width(low_res_surface);
  int sh = cairo_image_surface_get_height(low_res_surface);
  int stride = cairo_image_surface_get_stride(low_res_surface);
  int sdata_len = sw * sh * 4;
  unsigned char *new_sdata = malloc(sdata_len);
  graphic_argb32_to_monochrome(new_sdata, sw, sh, sdata, stride);
  memcpy(sdata, new_sdata, sdata_len);

  char buffer[1024];
  snprintf(buffer, 1024, "out-%dx%d.png", x, y)  ;
  cairo_surface_write_to_png(low_res_surface, buffer);

  cairo_destroy(cr);
  cairo_surface_destroy(low_res_surface);
  g_object_unref(page);
}
