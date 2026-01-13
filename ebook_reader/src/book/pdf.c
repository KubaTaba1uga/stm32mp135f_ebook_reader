#include <poppler/glib/poppler-document.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "book/book.h"
#include "book/pdf.h"
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

ebk_error_t ebk_book_pdf_init(ebk_book_module_t module, ebk_books_t core) {
  puts(__func__)  ;  
  struct ebk_Pdf *pdf = ebk_mem_malloc(sizeof(struct ebk_Pdf));
  *pdf = (struct ebk_Pdf){
      .core = core,
  };
  *module = (struct ebk_BookModule){
      .is_extension = pdf_is_extension,
      .book_init = pdf_book_init,
      .book_destroy = pdf_book_destroy,
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
  puts(__func__)  ;  
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
  if (!book->title){
    book->title = "Unknown";
  }
  
  puts(book->title)  ;  
  return 0;

error_out:
  ebk_mem_free(uri);
  return ebk_errno;
};
static void pdf_book_destroy(ebk_book_t book) {
  puts(__func__)  ;
  if (!book->private) {
    return;
  }

  g_object_unref(book->private);
  book->private = NULL;
};
