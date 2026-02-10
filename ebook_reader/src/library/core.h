#ifndef EBOOK_READER_BOOK_CORE_H
#define EBOOK_READER_BOOK_CORE_H
#include <stdbool.h>

#include "library/library.h"
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
  int max_page_number;
  const char *title;    
  library_t owner;
  int page_number;
  void *private;
  double scale;
  int x_off;
  int y_off;

};

struct BookModule {
  err_t (*book_init)(book_t);
  void (*book_destroy)(book_t);
  const unsigned char *(*book_get_thumbnail)(book_t, int x, int y);
  const unsigned char *(*book_get_page)(book_t book, int x, int y,
                                        int *buf_len);
  bool (*is_extension)(const char *);
  void (*destroy)(book_module_t);

  void *private;
};

err_t book_module_pdf_init(book_module_t, library_t);

#endif // EBOOK_READER_BOOK_CORE_H
