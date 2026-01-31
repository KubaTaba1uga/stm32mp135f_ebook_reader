#include <assert.h>
#include <ctype.h>
#include <libgen.h>
#include <lvgl.h>
#include <poppler.h>
#include <stdio.h>
#include <string.h>

#include "book/book.h"
#include "book/core.h"
#include "cairo.h"
#include "utils/err.h"
#include "utils/mem.h"

typedef struct Pdf *pdf_t;
typedef struct PdfBook *pdf_book_t;

struct Pdf {
  book_api_t owner;
};

struct PdfBook {
  cairo_surface_t *thumbnail;
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
  if (!title_start) {
    err_o = err_errnof(ENODATA, "No title in: %s", book->file_path);
    goto error_popen_cleanup;
  }

  const char *title_end = strstr(title_start, "\n");
  if (!title_start) {
    err_o = err_errnof(ENODATA, "No title in: %s", book->file_path);
    goto error_popen_cleanup;
  }

  pclose(pdfinfo);

  title_start += strlen("Title:");
  while (isspace(*title_start) && title_start < title_end) {
    title_start++;
  }

  pdf_book->title = mem_malloc(title_end - title_start + 1);
  memset(pdf_book->title, 0, title_end - title_start + 1);
  memcpy(pdf_book->title, title_start, title_end - title_start);

  return pdf_book->title;

error_popen_cleanup:
  pclose(pdfinfo);
error_out:
  puts("ERROR");
  return NULL;
}

static cairo_status_t cairo_read_func(void *closure, unsigned char *data,
                                      unsigned int length) {
  fread(data, 1, length, closure);

  return CAIRO_STATUS_SUCCESS;
}

static const unsigned char *book_module_pdf_book_get_thumbnail(book_t book,
                                                               int x, int y) {
  pdf_book_t pdf_book = book->private;
  if (pdf_book->thumbnail) {
    return cairo_image_surface_get_data(pdf_book->thumbnail);
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

  pdf_book->thumbnail =
      cairo_image_surface_create_from_png_stream(cairo_read_func, pdfinfo);

  unsigned char *thumbnail = cairo_image_surface_get_data(pdf_book->thumbnail);

  pclose(pdfinfo);

  return thumbnail;

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
  if (pdf_book->thumbnail) {
    cairo_surface_destroy(pdf_book->thumbnail);
  }
  mem_free(pdf_book->title);
  mem_free(pdf_book);

  book->private = NULL;
};
