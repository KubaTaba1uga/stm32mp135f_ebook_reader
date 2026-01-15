#include <poppler/glib/poppler-document.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "book/book.h"
#include "book/pdf.h"
#include "cairo.h"
#include "glib.h"
#include "glib/poppler.h"
#include "utils/error.h"
#include "utils/mem.h"

struct ebk_Pdf {
  ebk_books_t core;
};

static bool pdf_is_extension(const char *);
static ebk_error_t pdf_book_init(ebk_book_t);
static void pdf_book_destroy(ebk_book_t);
static unsigned char *pdf_book_create_thumbnail(ebk_book_t book, int w, int h);

ebk_error_t ebk_book_pdf_init(ebk_book_module_t module, ebk_books_t core) {
  puts(__func__);
  struct ebk_Pdf *pdf = ebk_mem_malloc(sizeof(struct ebk_Pdf));
  *pdf = (struct ebk_Pdf){
      .core = core,
  };
  *module = (struct ebk_BookModule){
      .is_extension = pdf_is_extension,
      .book_init = pdf_book_init,
      .book_destroy = pdf_book_destroy,
      .create_thumbnail = pdf_book_create_thumbnail,
      .private = pdf,
  };

  return 0;
}

static bool pdf_is_extension(const char *path) {
  puts(__func__);
  /* puts(path); */
  const char *ext = ".pdf";
  const char *result = strstr(path, ext);
  return result != NULL;
  ;
};

static ebk_error_t pdf_book_init(ebk_book_t book) {
  puts(__func__);
  int bytes = strlen(book->file_path) + strlen("///file:/") + 1;
  char *uri = ebk_mem_malloc(bytes);
  snprintf(uri, bytes, "file:///%s", book->file_path);

  PopplerDocument *pdf = poppler_document_new_from_file(uri, NULL, NULL);
  if (!pdf) {
    ebk_errno = ebk_errnof(ENODATA, "Cannot open pdf: %s", uri);
    goto error_out;
  }

  ebk_mem_free(uri);

  book->private = pdf;
  book->title = poppler_document_get_title(pdf);
  if (!book->title) {
    book->title = "Unknown";
  }

  puts(book->title);
  return 0;

error_out:
  ebk_mem_free(uri);
  return ebk_errno;
};
static void pdf_book_destroy(ebk_book_t book) {
  puts(__func__);
  if (!book->private) {
    return;
  }

  g_object_unref(book->private);
  book->private = NULL;
};

static void argb32_to_a1(uint8_t *dst, int w, int h,
                         const uint8_t *src, int stride)
{
    int dst_stride = (w + 7) / 8;
    memset(dst, 0x00, dst_stride * h);   // 0 = transparent everywhere

    for(int y = 0; y < h; y++) {
        const uint32_t *row = (const uint32_t *)(src + y * stride);

        for(int x = 0; x < w; x++) {
            uint32_t p = row[x];                 // 0xAARRGGBB (little-endian)
            uint8_t a = (p >> 24) & 0xFF;
            uint8_t r = (p >> 16) & 0xFF;
            uint8_t g = (p >>  8) & 0xFF;
            uint8_t b = (p >>  0) & 0xFF;

            /* decide "ink" (opaque) */
            uint16_t lum = (uint16_t)(r*30 + g*59 + b*11) / 100;
            bool is_ink = (a > 0) && (lum < 160);  // dark -> opaque

            int byte_i = y * dst_stride + (x >> 3);
            int bit    = 7 - (x & 7);              // MSB first

            if(is_ink) dst[byte_i] |= (1u << bit);  // 1 = opaque
        }
    }
}

// dst: size = ((w + 7) / 8) * h bytes
static void argb32_to_i1(uint8_t *dst, int w, int h,
                         const uint8_t *src, int stride)
{
    int dst_stride = (w + 7) / 8;
    memset(dst, 0x00, dst_stride * h); // 0 = black (you can invert if needed)

    for(int y=0; y<h; y++) {
        const uint32_t *row = (const uint32_t *)(src + y * stride);
        for(int x=0; x<w; x++) {
            uint32_t p = row[x];          // 0xAARRGGBB on little-endian
            uint8_t r = (p >> 16) & 0xFF;
            uint8_t g = (p >> 8)  & 0xFF;
            uint8_t b = (p >> 0)  & 0xFF;

            // luminance (cheap)
            uint16_t lum = (uint16_t)(r*30 + g*59 + b*11) / 100;

            // threshold: choose what looks good for e-ink
            bool white = lum > 160;

            int byte_i = y * dst_stride + (x >> 3);
            int bit    = 7 - (x & 7);     // MSB first
            if(white) dst[byte_i] |= (1u << bit);
        }
    }
}


static unsigned char * pdf_book_create_thumbnail(ebk_book_t book, int w, int h) {
  PopplerDocument *doc;
  PopplerPage *page;
  cairo_surface_t *surface;
  cairo_t *cr;

  doc = book->private;
  page = poppler_document_get_page(doc, 0);

  double pw, ph; // page size in points
  poppler_page_get_size(page, &pw, &ph);

  int out_w = w; // thumbnail width
  int out_h = h;

  surface =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, out_w, out_h);
  cr = cairo_create(surface);

  /* white background */
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);

  /* scale page -> output */
  double sx = (double)out_w / pw;
  double sy = (double)out_h / ph;
  cairo_scale(cr, sx, sy);

  /* render at scaled size */
  poppler_page_render(page, cr);

  /* now surface contains the first page image */

  unsigned char *sdata = cairo_image_surface_get_data(surface);
  int sw      = cairo_image_surface_get_width(surface);
  int sh      = cairo_image_surface_get_height(surface);
  int stride = cairo_image_surface_get_stride(surface);
  (void)(argb32_to_i1);
  unsigned char *buf = ebk_mem_malloc(w*h);
  argb32_to_a1(buf, sw, sh, sdata, stride);
  return buf;
  
  /* cairo_destroy(cr); */
  /* g_object_unref(page); */
  /* g_object_unref(doc); */
}
