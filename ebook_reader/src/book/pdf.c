#include <libgen.h>
#include <lvgl.h>
#include <math.h>
#include <poppler.h>
#include <stdio.h>
#include <string.h>

#include "book/book.h"
#include "book/core.h"
#include "cairo.h"
#include "glib.h"
#include "utils/err.h"
#include "utils/graphic.h"
#include "utils/mem.h"

typedef struct Pdf *pdf_t;
typedef struct PdfBook *pdf_book_t;

struct Pdf {
  book_api_t owner;
};

struct PdfBook {
  PopplerDocument *document;
  unsigned char *thumbnail;
  unsigned char *page;
  char *title;
};

static err_t book_module_pdf_book_init(book_t);
static void book_module_pdf_book_destroy(book_t);
static const char *book_module_pdf_book_get_title(book_t);
static const unsigned char *book_module_pdf_book_get_thumbnail(book_t, int,
                                                               int);
static const unsigned char *book_module_pdf_book_get_page(book_t, int, int, int,
                                                          int *);
static bool book_module_pdf_is_extension(const char *);
static void book_module_pdf_destroy(book_module_t);

err_t book_module_pdf_init(book_module_t module, book_api_t api) {
  pdf_t pdf = mem_malloc(sizeof(struct Pdf));
  pdf->owner = api;
  module->book_init = book_module_pdf_book_init;
  module->book_destroy = book_module_pdf_book_destroy;
  module->book_get_title = book_module_pdf_book_get_title;
  module->book_get_thumbnail = book_module_pdf_book_get_thumbnail;
  module->book_get_page = book_module_pdf_book_get_page;
  module->is_extension = book_module_pdf_is_extension;
  module->destroy = book_module_pdf_destroy;
  module->private = pdf;

  return 0;
}

void book_module_pdf_destroy(book_module_t module) {
  if (!module->private) {
    return;
  }

  mem_free(module->private);
  module->private = NULL;
};

static err_t book_module_pdf_book_init(book_t book) {
  pdf_book_t pdf_book = book->private = mem_malloc(sizeof(struct PdfBook));
  *pdf_book = (struct PdfBook){0};

  int bytes = strlen(book->file_path) + strlen("file:///") + 1;
  char *uri = mem_malloc(bytes);
  snprintf(uri, bytes, "file:///%s", book->file_path);

  pdf_book->document = poppler_document_new_from_file(uri, NULL, NULL);
  if (!pdf_book->document) {
    err_o = err_errnof(ENODATA, "Cannot open pdf: %s", uri);
    goto error_out;
  }

  mem_free(uri);

  return 0;

error_out:
  return err_o;
};

static const char *book_module_pdf_book_get_title(book_t book) {
  pdf_book_t pdf_book = book->private;
  if (pdf_book->title) {
    return pdf_book->title;
  }

  char *title = poppler_document_get_title(pdf_book->document);
  if (!title) {
    pdf_book->title = strdup(book->file_path);
    title = basename(pdf_book->title);
  } else {
    pdf_book->title = strdup(title);
    g_free(title);
    title = pdf_book->title;
  }

  return title;
}

static const unsigned char *book_module_pdf_book_get_thumbnail(book_t book,
                                                               int x, int y) {
  pdf_book_t pdf_book = book->private;
  if (pdf_book->thumbnail) {
    return pdf_book->thumbnail;
  }

  PopplerDocument *doc = pdf_book->document;
  PopplerPage *page = poppler_document_get_page(doc, 0);
  cairo_surface_t *surface;
  cairo_t *cr;

  double pw, ph; // page size in points
  poppler_page_get_size(page, &pw, &ph);

  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x, y);
  cr = cairo_create(surface);

  /* white background */
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);

  /* scale page */
  double sx = (double)x / pw;
  double sy = (double)y / ph;
  cairo_scale(cr, sx, sy);

  /* render at scaled size */
  poppler_page_render(page, cr);
  cairo_surface_flush(surface);

  unsigned char *sdata = cairo_image_surface_get_data(surface);
  int sw = cairo_image_surface_get_width(surface);
  int sh = cairo_image_surface_get_height(surface);
  int stride = cairo_image_surface_get_stride(surface);

  pdf_book->thumbnail = mem_malloc(x * y / 8 + 8);
  lv_color32_t *pal = (lv_color32_t *)pdf_book->thumbnail;
  pal[0] = (lv_color32_t){
      .red = 255, .green = 255, .blue = 255, .alpha = 255}; // index 0 = white
  pal[1] = (lv_color32_t){
      .red = 0, .green = 0, .blue = 0, .alpha = 255}; // index 1 = black

  graphic_argb32_to_i1(pdf_book->thumbnail, sw, sh, sdata, stride);

  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  g_object_unref(page);

  return pdf_book->thumbnail;
};

static bool book_module_pdf_is_extension(const char *file_path) {
  return strstr(file_path, ".pdf") != NULL;
}

static void book_module_pdf_book_destroy(book_t book) {
  if (!book->private) {
    return;
  }

  pdf_book_t pdf_book = book->private;
  g_object_unref(pdf_book->document);
  mem_free(pdf_book->thumbnail);
  mem_free(pdf_book->title);
  mem_free(pdf_book->page);
  mem_free(pdf_book);

  book->private = NULL;
};

static const unsigned char *book_module_pdf_book_get_page(book_t book, int x,
                                                          int y, int page_no,
                                                          int *buf_len) {
  pdf_book_t pdf_book = book->private;
  assert(pdf_book != NULL);

  if (pdf_book->page) {
    mem_free(pdf_book->page);
  }

  PopplerDocument *doc = pdf_book->document;
  assert(doc != NULL);

  PopplerPage *page = poppler_document_get_page(doc, page_no);
  assert(page != NULL);

  cairo_surface_t *hi_res_surface;
  cairo_surface_t *surface;
  cairo_t *hi_res_cr;
  cairo_t *cr;

  const int hi_res_x = 720;
  const int hi_res_y = 1200;

  /* hi_res_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, hi_res_x,
   * hi_res_y); */
  /* hi_res_cr = cairo_create(hi_res_surface); */
  /* cairo_set_source_rgb(hi_res_cr, 1, 1, 1); */
  /* cairo_paint(hi_res_cr); */

  /* { // Solution 1: quite bad, doesnt solve problem at all  */
  /*   surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x, y); */
  /* cairo_surface_set_device_scale(surface, 1.3, 1.3); */
  /* cairo_surface_set_device_offset (surface, -50, -50); */
  /* } */
  { // Solution 2: render hi resolution then create a view 
    hi_res_surface =
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32, hi_res_x, hi_res_y);
    cairo_surface_set_device_scale(hi_res_surface, 1.3, 1.3);
    /* cairo_surface_set_device_offset(hi_res_surface, -50, -50); */
    hi_res_cr = cairo_create(hi_res_surface);
    cairo_set_source_rgb(hi_res_cr, 1, 1, 1);
    cairo_paint(hi_res_cr);
    poppler_page_render(page, hi_res_cr);
    cairo_surface_flush(hi_res_surface);

    surface = cairo_surface_create_similar_image(hi_res_surface,
                                                 CAIRO_FORMAT_ARGB32, x, y);
    cr = cairo_create(surface);
    cairo_set_source_surface (cr, hi_res_surface, 0, 0);
    cairo_rectangle (cr, 0, 0, x, y);
    cairo_fill (cr);

    /* cairo_paint(cr);     */
  }



  unsigned char *sdata = cairo_image_surface_get_data(surface);
  int sw = cairo_image_surface_get_width(surface);
  int sh = cairo_image_surface_get_height(surface);
  int stride = cairo_image_surface_get_stride(surface);
  *buf_len = ((sw + 7) / 8) * sh;
  /* *  buf_len = sw * sh / 8 + 8; */
  printf("sw=%d, sh=%d, stride=%d, buf_len=%d\n", sw, sh, stride, *buf_len);

  pdf_book->page = mem_malloc(*buf_len);

  graphic_argb32_to_i1(pdf_book->page, sw, sh, sdata, stride);

  lv_color32_t *pal = (lv_color32_t *)pdf_book->page;
  pal[0] = (lv_color32_t){
      .red = 255, .green = 255, .blue = 255, .alpha = 255}; // index 0 = white
  pal[1] = (lv_color32_t){
      .red = 0, .green = 0, .blue = 0, .alpha = 255}; // index 1 = black

  cairo_destroy(hi_res_cr);
  cairo_surface_destroy(surface);
  g_object_unref(page);

  return pdf_book->page;
}

/*
    if (outside)
      {
        // add outer subpath in anticlockwise direction
        cairo_new_sub_path(cr);
        cairo_move_to(cr, img_size, 0);
        cairo_line_to(cr, 0, 0);
        cairo_line_to(cr, 0, img_size);
        cairo_line_to(cr, img_size, img_size);
        cairo_close_path(cr);
      }
      cairo_clip(cr);
*/
