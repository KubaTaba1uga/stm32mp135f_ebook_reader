#ifndef EBOOK_READER_BOOK_H
#define EBOOK_READER_BOOK_H

#include "utils/error.h"
#include "utils/zero_list.h"
#include <stdint.h>

typedef struct ebk_Book *ebk_book_t;
typedef struct ebk_BooksList *ebk_books_list_t;
typedef struct ebk_BooksCore *ebk_books_t;

ebk_error_t ebk_books_init(ebk_books_t *);
void ebk_books_destroy(ebk_books_t *);

/**
   @brief Search for books in the system.
   @param core Books subsystem instance.
   @param out List of books to populate.
   @return 0 on success and ebk_errno on failure.
*/
ebk_error_t ebk_books_list_init(ebk_books_t core, ebk_books_list_t *out);

/**
   @brief Get book from books list.
   @param list List of books to get book from.
   @return Pointer to book on success, on error NULL and set ebk_errno.

   To get all books from list, you need to execute ebk_books_get until
   it return NULL. It return NULL and set ebk_errno with ENOENT code.
*/
ebk_book_t ebk_books_list_get(ebk_books_list_t list);
uint32_t ebk_books_list_len(ebk_books_list_t list);

void ebk_books_list_destroy(ebk_books_list_t* out);

const char *ebk_book_get_title(ebk_book_t book);

unsigned char *ebk_book_create_thumbnail(ebk_book_t book, int w, int h);
#endif // EBOOK_READER_CORE_MENU
