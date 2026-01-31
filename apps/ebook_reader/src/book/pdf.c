#include <assert.h>
#include <ctype.h>
#include <libgen.h>
#include <lvgl.h>
#include <poppler.h>
#include <stdio.h>
#include <string.h>

#include "book/book.h"
#include "book/core.h"
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
  /* PopplerDocument *document; */
  unsigned char *thumbnail;
  char *title;
};

static err_t book_module_pdf_book_init(book_t);
static void book_module_pdf_book_destroy(book_t);
static const char *book_module_pdf_book_get_title(book_t);
static const unsigned char *book_module_pdf_book_get_thumbnail(book_t, int,
                                                               int);
static bool book_module_pdf_is_extension(const char *);
static void book_module_pdf_destroy(book_module_t);

err_t book_module_pdf_init(book_module_t module, book_api_t api) {
  pdf_t pdf = mem_malloc(sizeof(struct Pdf));
  pdf->owner = api;
  module->book_init = book_module_pdf_book_init;
  module->book_destroy = book_module_pdf_book_destroy;
  module->book_get_title = book_module_pdf_book_get_title;
  module->book_get_thumbnail = book_module_pdf_book_get_thumbnail;
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

  return 0;
};

static const char *book_module_pdf_book_get_title(book_t book) {
  pdf_book_t pdf_book = book->private;
  if (pdf_book->title) {
    return pdf_book->title;
  }

  char cmd_buf[4096] = {0};
  snprintf(cmd_buf, sizeof(cmd_buf), "/usr/bin/pdfinfo %s", book->file_path);
  FILE *pdfinfo = popen(cmd_buf, "r");
  if (!pdfinfo) {
    goto error_out;
  }

  fread(cmd_buf, 1, sizeof(cmd_buf), pdfinfo);

  const char *title_start = strstr(cmd_buf, "Title:");
  assert(title_start != NULL);
  const char *title_end = strstr(title_start, "\n");
  assert(title_end != NULL);

  pclose(pdfinfo);

  title_start += strlen("Title:");
  while (isspace(*title_start) && title_start < title_end) {
    title_start++;
  }

  pdf_book->title = mem_malloc(title_end - title_start + 1);
  memset(pdf_book->title, 0, title_end - title_start + 1);
  memcpy(pdf_book->title, title_start, title_end - title_start);

  return pdf_book->title;

error_out:
  puts("ERROR");
  return NULL;
}

static const unsigned char *book_module_pdf_book_get_thumbnail(book_t book,
                                                               int x, int y) {
  pdf_book_t pdf_book = book->private;
  if (pdf_book->thumbnail) {
    return pdf_book->thumbnail;
  }

  char cmd_buf[4096] = {0};
  snprintf(
      cmd_buf, sizeof(cmd_buf),
      "/usr/bin/pdftoppm -f 0 -l 0 -scale-to-x %d -scale-to-y %d -png -mono %s",
      x, y, book->file_path);
  FILE *pdfinfo = popen(cmd_buf, "r");
  if (!pdfinfo) {
    goto error_out;
  }

  /* PopplerDocument *doc = pdf_book->document; */
  /* PopplerPage *page = poppler_document_get_page(doc, 0); */
  /* cairo_surface_t *surface; */
  /* cairo_t *cr; */

  /* double pw, ph; // page size in points */
  /* poppler_page_get_size(page, &pw, &ph); */

  /* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x, y); */
  /* cr = cairo_create(surface); */

  /* /\* white background *\/ */
  /* cairo_set_source_rgb(cr, 1, 1, 1); */
  /* cairo_paint(cr); */

  /* /\* scale page *\/ */
  /* double sx = (double)x / pw; */
  /* double sy = (double)y / ph; */
  /* cairo_scale(cr, sx, sy); */

  /* /\* render at scaled size *\/ */
  /* poppler_page_render(page, cr); */
  /* cairo_surface_flush(surface); */

  /* unsigned char *sdata = cairo_image_surface_get_data(surface); */
  /* int sw = cairo_image_surface_get_width(surface); */
  /* int sh = cairo_image_surface_get_height(surface); */
  /* int stride = cairo_image_surface_get_stride(surface); */

  /* pdf_book->thumbnail = mem_malloc(x * y + 8); */
  /* lv_color32_t *pal = (lv_color32_t *)pdf_book->thumbnail; */
  /* pal[0] = (lv_color32_t){ */
  /*     .red = 255, .green = 255, .blue = 255, .alpha = 255}; // index 0 =
   * white */
  /* pal[1] = (lv_color32_t){ */
  /*     .red = 0, .green = 0, .blue = 0, .alpha = 255}; // index 1 = black */

  /* graphic_argb32_to_i1(pdf_book->thumbnail, sw, sh, sdata, stride); */

  /* cairo_destroy(cr); */
  /* cairo_surface_destroy(surface); */
  /* g_object_unref(page); */

  return pdf_book->thumbnail;

error_out:
  return NULL;
};

static bool book_module_pdf_is_extension(const char *file_path) {
  return strstr(file_path, ".pdf") != NULL;
}

static void book_module_pdf_book_destroy(book_t book) {
  if (!book->private) {
    return;
  }

  pdf_book_t pdf_book = book->private;
  /* g_object_unref(pdf_book->document); */
  mem_free(pdf_book->thumbnail);
  mem_free(pdf_book->title);
  mem_free(pdf_book);

  book->private = NULL;
};
