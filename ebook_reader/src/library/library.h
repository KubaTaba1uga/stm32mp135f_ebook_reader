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
book_t books_list_pop(books_list_t, int);
void books_list_remove(books_list_t list, book_t book);
const unsigned char *book_get_page(book_t book, int x, int y, int *buf_len);
const char *book_get_title(book_t);
void book_set_scale(book_t, double);
double book_get_scale(book_t);
void book_set_x_off(book_t, int);
int book_get_x_off(book_t);
void book_set_y_off(book_t, int);
int book_get_y_off(book_t);
int book_get_page_no(book_t);
void book_set_page_no(book_t, int);
int book_get_max_page_no(book_t);
const unsigned char *book_get_thumbnail(book_t, int x, int y);

#endif // EBOOK_READER_LIBRARY_H
