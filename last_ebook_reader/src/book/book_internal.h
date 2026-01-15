#ifndef EBOOK_READER_BOOK_INTERNAL_H
#define EBOOK_READER_BOOK_INTERNAL_H
#include <stdbool.h>

#include "book/book.h"
#include "utils/error.h"

typedef struct ebk_BookModule *ebk_book_module_t;
typedef struct ebk_Book *ebk_book_t;
typedef struct ebk_BookPrivate *ebk_book_private_t;

enum ebk_BookExtensionEnum {
  ebk_BookExtensionEnum_PDF,
};

/**
  Other subsystem than book does not need to care about book's extension.
  This way we can treat all books exactly the same.
*/
struct ebk_Book {
  enum ebk_BookExtensionEnum ext;
  struct ebk_ZListNode list_node;
  const char *file_path;
  const char *title;
  void *private;
  void *owner;  
};

/**
   We are gonna support multiple books extensions like PDF, MOBI, EPUB etc.
   Book module provide generic abstracion over all formats.
*/
struct ebk_BookModule {
  void (*destroy)(ebk_book_module_t);
  bool (*is_extension)(const char *);
  unsigned char * (*create_thumbnail)(ebk_book_t book, int w, int h);
  ebk_error_t (*book_init)(ebk_book_t);
  void (*book_destroy)(ebk_book_t);  
  void *private;
};

struct ebk_BooksList {
  ebk_zlist_node_t current_book;
  struct ebk_ZList books;
  void *owner;
};

#endif // EBOOK_READER_BOOK_INTERNAL_H
#ifndef EBOOK_READER_BOOK_H
#define EBOOK_READER_BOOK_H

#include "utils/error.h"
#include "utils/zero_list.h"

enum ebk_BookExtensionEnum {
  ebk_BookExtensionEnum_PDF,
};

/**
   @brief Search for books in the system.
   @param out List of books to populate.
   @return 0 on success and ebk_errno on failure.
*/
ebk_error_t ebk_books_find(ebk_books_t out);
void ebk_books_destroy(ebk_books_t out);

#endif // EBOOK_READER_CORE_MENU
