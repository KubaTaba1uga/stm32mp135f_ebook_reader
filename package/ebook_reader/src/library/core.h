#ifndef EBOOK_READER_BOOK_CORE_H
#define EBOOK_READER_BOOK_CORE_H
#include <stdbool.h>

#include "db/db.h"
#include "library/library.h"
#include "utils/err.h"
#include "utils/zlist.h"

typedef struct BookInterface *book_interface_t;

enum BookExtensionEnum {
  BookExtensionEnum_PDF,
  // Add more extensions here
  BookExtensionEnum_MAX,
};

struct Book {
  enum BookExtensionEnum ext;
  struct ZListNode next;
  struct DbBook db_data;
  library_t owner;
  void *private;
};

struct BookInterface {
  err_t (*book_create)(void *private, const char *path, book_t book);
  void (*book_destroy)(void *private, book_t book);
  const unsigned char *(*book_get_page)(void *private, book_t book, int x,
                                        int y, int *buf_len);
  bool (*is_extension)(void *private, const char *);
  void (*destroy)(book_interface_t);
  void *private;
};

err_t book_interface_pdf_init(book_interface_t, library_t, db_t);

#endif // EBOOK_READER_BOOK_CORE_H
