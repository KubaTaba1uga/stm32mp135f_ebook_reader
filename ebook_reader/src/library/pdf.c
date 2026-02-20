#include <assert.h>
#include <ctype.h>
#include <libgen.h>
#include <lvgl.h>
#include <poppler.h>
#include <stdio.h>
#include <string.h>

#include "cairo.h"
#include "db/db.h"
#include "library/core.h"
#include "library/library.h"
#include "utils/err.h"
#include "utils/mem.h"

typedef struct Pdf *pdf_t;
typedef struct PdfBook *pdf_book_t;

struct Pdf {
  library_t owner;
  db_t db;
};

struct PdfBook {
  cairo_surface_t *thumbnail;
  cairo_surface_t *page;
};

static err_t book_interface_pdf_book_init(void *private, void **interface_data,
                                          const char *path);
static void book_interface_pdf_book_destroy(void *private, book_t book);
static const unsigned char *
book_interface_pdf_book_get_thumbnail(void *private, pdf_book_t pdf_book,
                                      const char *path, int x, int y);
/* static const unsigned char *book_interface_pdf_get_page(void *private, */
/*                                                         book_t book, int x, */
/*                                                         int y, int *buf_len); */
static bool book_interface_pdf_is_extension(void *private, const char *);
static void book_interface_pdf_destroy(book_interface_t);

err_t book_interface_pdf_init(book_interface_t interface, library_t lib,
                              db_t db) {
  pdf_t pdf = mem_malloc(sizeof(struct Pdf));
  *pdf = (struct Pdf){
      .owner = lib,
      .db = db,
  };

  interface->book_init = book_interface_pdf_book_init;
  interface->book_destroy = book_interface_pdf_book_destroy;
  /* interface->book_get_page = book_interface_pdf_get_page; */
  interface->is_extension = book_interface_pdf_is_extension;
  interface->destroy = book_interface_pdf_destroy;
  interface->private = pdf;

  return 0;
}

void book_interface_pdf_destroy(book_interface_t interface) {
  if (!interface->private) {
    return;
  }

  mem_free(interface->private);
  interface->private = NULL;
};

static char *pdfinfo_find_field(void *, char *, const char *);
static err_t book_interface_pdf_book_init(void *private, void **interface_data,
                                          const char *path) {
  pdf_t pdf = private;
  pdf_book_t pdf_book = *interface_data = mem_malloc(sizeof(struct PdfBook));
  *pdf_book = (struct PdfBook){0};

  char cmd_buf[4096] = {0};
  snprintf(cmd_buf, sizeof(cmd_buf), "/usr/bin/pdfinfo %s", path);
  FILE *pdfinfo = popen(cmd_buf, "r");
  if (!pdfinfo) {
    goto error_out;
  }

  fread(cmd_buf, 1, sizeof(cmd_buf), pdfinfo);

  char *title = pdfinfo_find_field(pdf, cmd_buf, "Title");
  if (!title) {
    err_o = err_errnof(ENODATA, "No title in: %s", path);
    goto error_popen_cleanup;
  }

  char *pages = pdfinfo_find_field(pdf, cmd_buf, "Pages");
  if (!pages) {
    err_o = err_errnof(ENODATA, "No pages in: %s", path);
    goto error_title_cleanup;
  }

  err_o = db_book_insert(
      pdf->db,
      (struct DbBook){
          .extension = BookExtensionEnum_PDF,
          .title = title,
          .max_page_number = atoi(pages),
          .page_number = 0,
          .path = path,
          .thumbnail =
              {
                  .buf = book_interface_pdf_book_get_thumbnail(
                      pdf, pdf_book, path, book_thumbnail_x, book_thumbnail_y),
                  .len = book_thumbnail_x * book_thumbnail_y * 4 // ARGB
              },
      });
  ERR_TRY_CATCH(err_o, error_pages_cleanup);

  cairo_surface_destroy(pdf_book->thumbnail);
  pdf_book->thumbnail = NULL;
  mem_free(title);
  mem_free(pages);
  pclose(pdfinfo);

  return 0;

error_pages_cleanup:
  mem_free(pages);
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

static const unsigned char *
book_interface_pdf_book_get_thumbnail(void *private, pdf_book_t pdf_book,
                                      const char *path, int x, int y) {
  char cmd_buf[4096] = {0};
  snprintf(cmd_buf, sizeof(cmd_buf),
           "/usr/bin/pdftoppm -f 0 -l 0 -scale-to-x %d -scale-to-y %d -png "
           "-mono %s",
           x, y, path);
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

static char *pdfinfo_find_field(void *private, char *stdout,
                                const char *field_name) {
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

static bool book_interface_pdf_is_extension(void *private,
                                            const char *file_path) {
  return strstr(file_path, ".pdf") != NULL;
}

static void book_interface_pdf_book_destroy(void *private, book_t book) {
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

  mem_free(pdf_book);
  book->private = NULL;
};

/* static const unsigned char *book_interface_pdf_get_page(void *private, */
/*                                                         book_t book, int x,
 */
/*                                                         int y, int *buf_len)
 * { */
/*   pdf_book_t pdf_book = book->private; */
/*   if (pdf_book->page) { */
/*     cairo_surface_destroy(pdf_book->page); */
/*     pdf_book->page = NULL; */
/*   } */

/*   char cmd_buf[4096] = {0}; */
/*   snprintf(cmd_buf, sizeof(cmd_buf), */
/*            "/usr/bin/pdftoppm -f %d -l %d -scale-to-x %d -scale-to-y %d " */
/*            "-png -mono %s", */
/*            book->page_number, book->page_number, (int)(x * book->scale), */
/*            (int)(y * book->scale), book->file_path); */
/*   FILE *pdfinfo = popen(cmd_buf, "r"); */
/*   if (!pdfinfo) { */
/*     err_o = err_errnof(errno, "Cannot execute cmd: %s", cmd_buf); */
/*     goto error_out; */
/*   } */

/*   cairo_surface_t *surface = */
/*       cairo_image_surface_create_from_png_stream(cairo_read_func, pdfinfo);
 */

/*   pdf_book->page = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x, y); */
/*   cairo_t *cr = cairo_create(pdf_book->page); */
/*   cairo_set_source_surface(cr, surface, book->x_off * book->scale, */
/*                            book->y_off * book->scale); */
/*   cairo_paint(cr); */

/*   unsigned char *page = cairo_image_surface_get_data(pdf_book->page); */
/*   *buf_len = x * y * 4; // ARGB pixel size is 4 bytes */

/*   pclose(pdfinfo); */
/*   cairo_surface_destroy(surface); */
/*   cairo_destroy(cr); */

/*   return page; */

/* error_out: */
/*   return NULL; */
/* } */
