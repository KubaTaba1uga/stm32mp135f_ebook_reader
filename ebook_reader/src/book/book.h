#ifndef EBOOK_READER_BOOK_H
#define EBOOK_READER_BOOK_H

#include "utils/err.h"

typedef struct BooksList *books_list_t;
typedef struct BookApi *book_api_t;
typedef struct Book *book_t;

/**
   To use book_t you need to first initialize API
   resposnible for handling all objects from book
   subsystem.
*/
err_t book_api_init(book_api_t *);
void book_api_destroy(book_api_t *);
books_list_t book_api_find_books(book_api_t);
void books_list_destroy(books_list_t);

#endif // EBOOK_READER_BOOK_H
