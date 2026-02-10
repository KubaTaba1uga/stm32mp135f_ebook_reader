#ifndef EBOOK_READER_LIBRARY_H
#define EBOOK_READER_LIBRARY_H

#include "utils/err.h"

typedef struct Library *library_t;
typedef struct Book *book_t;
typedef struct BooksList *books_list_t;

err_t library_init(library_t *out);
void library_destroy(library_t *out);
books_list_t library_list_books(library_t lib);
book_t books_list_get(books_list_t);
int books_list_len(books_list_t);
void books_list_reset(books_list_t);
void books_list_destroy(books_list_t);
book_t books_list_pop(books_list_t, int);

#endif // EBOOK_READER_LIBRARY_H
