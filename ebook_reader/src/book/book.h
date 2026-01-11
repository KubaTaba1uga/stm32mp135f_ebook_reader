#ifndef EBOOK_READER_BOOK_H
#define EBOOK_READER_BOOK_H

#include "utils/error.h"
#include "utils/zero_list.h"

struct ebk_Book {
  const char *title;
  struct ebk_ZListNode list_node;
  void *private;
};

typedef struct ebk_Book *ebk_book_t;
typedef ebk_zlist_t ebk_books_t;

/**
   @brief Search for books in the system.
   @param out List of books to populate.
   @return 0 on success and ebk_errno on failure.
*/
ebk_error_t ebk_books_find(ebk_books_t *out);
/**
   @brief Get book from books list.
   @param list List of books to get book from.
   @return Pointer to book on success, on error NULL and set ebk_errno.

   To get all books from list, you need to execute ebk_books_get until
   it return NULL. It return NULL and set ebk_errno with ENOENT code.
*/
ebk_book_t ebk_books_get(ebk_books_t list);
void ebk_books_destroy(ebk_books_t out);

#endif // EBOOK_READER_CORE_MENU
