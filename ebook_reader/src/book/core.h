#ifndef EBOOK_READER_BOOK_CORE_H
#define EBOOK_READER_BOOK_CORE_H
#include <stdbool.h>

#include "book/book.h"
#include "utils/err.h"
#include "utils/zlist.h"

typedef struct BookModule *book_module_t;

enum BookExtensionEnum {
  BookExtensionEnum_PDF,
  // Add more extensions here
  BookExtensionEnum_MAX,
};

struct Book {
  enum BookExtensionEnum extension;
  struct ZListNode next;
  const char *file_path;
  book_api_t owner;  
  void *private;
};

struct BookModule {
err_t (*book_create)(book_t);
  void (*book_destroy)(book_t);
  const char *(*book_get_title)(book_t);
  const unsigned char *(*book_get_thumbnail)(book_t, int x, int y);
  bool (*is_extension)(const char *);
  void (*destroy)(book_module_t);
  void *private;
};

err_t book_module_pdf_init(book_module_t, book_api_t);

/* err_t book_init(); */

#endif // EBOOK_READER_BOOK_CORE_H
