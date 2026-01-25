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

/* static const unsigned char *book_module_pdf_book_get_page(book_t book, int x,
 */
/*                                                           int y, int page_no,
 */
/*                                                           int *buf_len) { */
/*   pdf_book_t pdf_book = book->private; */
/*   assert(pdf_book != NULL); */

/*   if (pdf_book->page) { */
/*     mem_free(pdf_book->page); */
/*   } */

/*   PopplerDocument *doc = pdf_book->document; */
/*   assert(doc != NULL); */
/*   PopplerPage *page = poppler_document_get_page(doc, page_no); */
/*   assert(page != NULL); */
/*   cairo_surface_t *surface; */
/*   cairo_t *cr; */

/*   surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x, y); */
/*   cr = cairo_create(surface); */
/*   cairo_set_source_rgb(cr, 1, 1, 1); */
/*   cairo_paint(cr); */

/*   /\* double pw, ph; // page size in points *\/ */
/*   /\* poppler_page_get_size(page, &pw, &ph); *\/ */

/*   /\* double zoom = 1.3;                 // >1 zoom in, <1 zoom out *\/ */
/*   /\* double s = fmin((double)x / pw, (double)y / ph) * zoom;  // uniform
 * scale *\/ */

/*   /\* /\\* // size of the scaled page in surface pixels *\\/ *\/ */
/*   /\* double rw = pw * s; *\/ */
/*   /\* double rh = ph * s; *\/ */

/*   /\* /\\* /\\\* center offsets (if zoom makes it larger than surface,
 * offsets become negative = cropped) *\\\/ *\\/ *\/ */
/*   /\* double dx = ((double)x - rw) * 0.3; *\/ */
/*   /\* double dy = ((double)y - rh) * 0.3; *\/ */

/*   /\* cairo_translate(cr, dx, dy); *\/ */
/*   /\* cairo_scale(cr, s, s); *\/ */
/*   /\* cairo_save(cr); *\/ */

/*   /\* double pw, ph; // page size in points *\/ */
/*   /\* Poppler_page_get_size(page, &pw, &ph); *\/ */

/*   /\* double sx = (double)x / pw; *\/ */
/*   /\* double sy = (double)y / ph; *\/ */
/*   /\* cairo_scale(cr, sx, sy); *\/ */
/*   /\* cairo_save(cr); *\/ */

/*   poppler_page_render(page, cr); */
/*   cairo_surface_flush(surface); */

/*   unsigned char *sdata = cairo_image_surface_get_data(surface); */
/*   int sw = cairo_image_surface_get_width(surface); */
/*   int sh = cairo_image_surface_get_height(surface); */
/*   int stride = cairo_image_surface_get_stride(surface); */
/*   *buf_len = ((sw + 7) / 8 ) * sh; */
/*   /\* *  buf_len = sw * sh / 8 + 8; *\/ */
/*   printf("sw=%d, sh=%d, stride=%d, buf_len=%d\n", sw, sh, stride, *buf_len);
 */

/*   pdf_book->page = mem_malloc(*buf_len); */

/*   graphic_argb32_to_i1(pdf_book->page, sw, sh, sdata, stride); */

/*   lv_color32_t *pal = (lv_color32_t *)pdf_book->page; */
/*   pal[0] = (lv_color32_t){ */
/*       .red = 255, .green = 255, .blue = 255, .alpha = 255}; // index 0 =
 * white */
/*   pal[1] = (lv_color32_t){ */
/*       .red = 0, .green = 0, .blue = 0, .alpha = 255}; // index 1 = black */

/*   cairo_destroy(cr); */
/*   cairo_surface_destroy(surface); */
/*   g_object_unref(page); */

/*   return pdf_book->page; */
/* } */

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
  cairo_surface_t *surface;
  cairo_t *cr;

  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x, y);
  cr = cairo_create(surface);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);

  double pw, ph;
  poppler_page_get_size(page, &pw, &ph);

  PopplerRectangle *rects = NULL;
  guint n_rects = 0;

  /* Returns rectangles for glyphs/words depending on Poppler version/build.
     If this function is available in your poppler-glib, use it. */
  if (poppler_page_get_text_layout(page, &rects, &n_rects) && n_rects > 0) {
    double min_x = 1e30, min_y = 1e30;

    for (guint i = 0; i < n_rects; i++) {
      /* rects[i] has x1,y1,x2,y2 in page coordinates (points) */
      if (rects[i].x1 < min_x) {
        min_x = rects[i].x1;
      }
      if (rects[i].y1 < min_y) {
        min_y = rects[i].y1;
      }
    }

    double pw, ph; // page size in points
    poppler_page_get_size(page, &pw, &ph);

    double zoom = 1.7; // >1 zoom in, <1 zoom out
    double s = fmin((double)x / pw, (double)y / ph) * zoom; // uniform scale

    cairo_scale(cr, s, s);
    cairo_save(cr);

    /* /\* scale as you normally do (points->pixels) *\/ */
    /* double sx = (double)x / pw; */
    /* double sy = (double)y / ph; */

    /* cairo_scale(cr, sx, sy); */

    /* move first text to (0,0); add padding in points if you want */
    double pad_pt = 20.0;
    cairo_translate(cr, -(min_x - pad_pt), -(min_y - pad_pt));

    poppler_page_render(page, cr);

    g_free(rects);
  } else {
    /* fallback: normal render */
    poppler_page_render(page, cr);
  }

  cairo_surface_flush(surface);
  /* double pw, ph; // page size in points */
  /* poppler_page_get_size(page, &pw, &ph); */

  /* double sx = (double)x / pw; */
  /* double sy = (double)y / ph; */
  /* cairo_scale(cr, sx, sy); */
  /* cairo_save(cr); */

  /* poppler_page_render(page, cr); */

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

  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  g_object_unref(page);

  return pdf_book->page;
}
