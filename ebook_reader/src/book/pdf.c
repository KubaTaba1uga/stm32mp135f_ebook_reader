#include <libgen.h>
#include <poppler.h>

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
  PopplerDocument *document;
  unsigned char *thumbnail;
  char *title;
};

static err_t book_module_pdf_book_create(book_t);
static void book_module_pdf_book_destroy(book_t);
static const char *book_module_pdf_book_get_title(book_t);
static const unsigned char *book_module_pdf_book_get_thumbnail(book_t, int,
                                                               int);
static bool book_module_pdf_is_extension(const char *);
static void book_module_pdf_destroy(book_module_t);

err_t book_module_pdf_init(book_module_t module, book_api_t api) {
  pdf_t pdf = mem_malloc(sizeof(struct Pdf));
  pdf->owner = api;
  module->book_create = book_module_pdf_book_create;
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

static err_t book_module_pdf_book_create(book_t book) {
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

  /* scale page -> output */
  double sx = (double)x / pw;
  double sy = (double)y / ph;
  cairo_scale(cr, sx, sy);

  /* render at scaled size */
  poppler_page_render(page, cr);

  /* now surface contains the first page image */

  unsigned char *sdata = cairo_image_surface_get_data(surface);
  int sw = cairo_image_surface_get_width(surface);
  int sh = cairo_image_surface_get_height(surface);
  int stride = cairo_image_surface_get_stride(surface);
  unsigned char *buf = mem_malloc(x * y);
  graphic_argb32_to_a1(buf, sw, sh, sdata, stride);
  cairo_destroy(cr);
  g_object_unref(page);

  return buf;
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
  mem_free(pdf_book);

  book->private = NULL;
};
