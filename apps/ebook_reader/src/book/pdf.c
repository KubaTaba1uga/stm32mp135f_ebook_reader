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
#include "utils/log.h"
#include "utils/mem.h"

typedef struct Pdf *pdf_t;
typedef struct PdfBook *pdf_book_t;

struct Pdf {
  book_api_t owner;
};

struct PdfBook {
  cairo_surface_t *thumbnail;
  cairo_surface_t *page;
};

static err_t book_module_pdf_book_init(book_t);
static void book_module_pdf_book_destroy(book_t);
static const unsigned char *book_module_pdf_book_get_thumbnail(book_t, int,
                                                               int);
static const unsigned char *book_module_pdf_get_page(book_t book, int x, int y,
                                                     int *buf_len);
static bool book_module_pdf_is_extension(const char *);
static void book_module_pdf_destroy(book_module_t);

err_t book_module_pdf_init(book_module_t module, book_api_t api) {
  pdf_t pdf = mem_malloc(sizeof(struct Pdf));
  pdf->owner = api;
  module->book_init = book_module_pdf_book_init;
  module->book_destroy = book_module_pdf_book_destroy;
  module->book_get_thumbnail = book_module_pdf_book_get_thumbnail;
  module->book_get_page = book_module_pdf_get_page;
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

static char *pdfinfo_find_field(char *, const char *);
static err_t book_module_pdf_book_init(book_t book) {
  pdf_book_t pdf_book = book->private = mem_malloc(sizeof(struct PdfBook));
  *pdf_book = (struct PdfBook){0};

  char cmd_buf[4096] = {0};
  snprintf(cmd_buf, sizeof(cmd_buf), "/usr/bin/pdfinfo %s", book->file_path);
  FILE *pdfinfo = popen(cmd_buf, "r");
  if (!pdfinfo) {
    goto error_out;
  }

  fread(cmd_buf, 1, sizeof(cmd_buf), pdfinfo);

  char *title = pdfinfo_find_field(cmd_buf, "Title");
  if (!title) {
    err_o = err_errnof(ENODATA, "No title in: %s", book->file_path);
    goto error_popen_cleanup;
  }

  char *pages = pdfinfo_find_field(cmd_buf, "Pages");
  if (!pages) {
    err_o = err_errnof(ENODATA, "No pages in: %s", book->file_path);
    goto error_title_cleanup;
  }

  book->title = title;
  book->max_page_number = atoi(pages);
  mem_free(pages);
  pclose(pdfinfo);

  return 0;

error_title_cleanup:
  mem_free(title);
error_popen_cleanup:
  pclose(pdfinfo);
error_out:
  mem_free(pdf_book);
  return err_o;
};


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

static char *pdfinfo_find_field(char *stdout, const char *field_name) {
  char field_name_deli[strlen(field_name) + 1];
  snprintf(field_name_deli, sizeof(field_name_deli), "%s:", field_name);

  const char *field_start = strstr(stdout, field_name_deli);
  if (!field_start) {
    goto error_out;
  }

  const char *field_end = strstr(field_start, "\n");
  if (!field_end) {
    goto error_out;
  }

  field_start += sizeof(field_name_deli);
  while (isspace(*field_start) && field_start < field_end) {
    field_start++;
  }

  char *field_value = mem_malloc(field_end - field_start + 1);
  memset(field_value, 0, field_end - field_start + 1);
  memcpy(field_value, field_start, field_end - field_start);

  return field_value;

error_out:
  return NULL;
}

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
  
  if (pdf_book->page) {
    cairo_surface_destroy(pdf_book->page);
  }

  mem_free((void *)book->title);
  mem_free(pdf_book);
  book->private = NULL;
};

static const unsigned char *book_module_pdf_get_page(book_t book, int x, int y,
                                                     int *buf_len) {
  puts(__func__);
  pdf_book_t pdf_book = book->private;
  if (pdf_book->page) {
    cairo_surface_destroy(pdf_book->page);
    pdf_book->page = NULL;
  }

  char cmd_buf[4096] = {0};
  snprintf(cmd_buf, sizeof(cmd_buf),
           "/usr/bin/pdftoppm -f %d -l %d -scale-to-x %d -scale-to-y %d -aa "
           "yes -aaVector yes -png -mono %s",
           book->page_number, book->page_number, (int)(x * book->scale),
           (int)(y * book->scale), book->file_path);
  FILE *pdfinfo = popen(cmd_buf, "r");
  if (!pdfinfo) {
    err_o = err_errnof(errno, "Cannot execute cmd: %s", cmd_buf);
    goto error_out;
  }

  cairo_surface_t *surface =
      cairo_image_surface_create_from_png_stream(cairo_read_func, pdfinfo);

  pdf_book->page = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x, y);
  cairo_t *cr = cairo_create(pdf_book->page);
  cairo_set_source_surface(cr, surface, book->x_off * book->scale,
                           book->y_off * book->scale);
  cairo_paint(cr);

  unsigned char *page = cairo_image_surface_get_data(pdf_book->page);
  *buf_len = x * y * 4;

  pclose(pdfinfo);
  cairo_surface_destroy(surface);
  cairo_destroy(cr);

  return page;

error_out:
  return NULL;
}
