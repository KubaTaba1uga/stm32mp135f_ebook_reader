#ifndef EBOOK_READER_DB_H
#define EBOOK_READER_DB_H
#include <stdbool.h>

#include "utils/err.h"

typedef struct Db *db_t;

err_t db_init(db_t *out);
void db_destroy(db_t *out);

struct DbBook {
  int max_page_number;
  const char *title;
  const char *path;
  int page_number;
  
  struct {
    const unsigned char *buf;
    int len;
  } thumbnail;

  struct {
    double scale;
    int x_off;
    int y_off;
  } settings;
  
  const void *priv;
};

err_t db_book_insert(db_t db, struct DbBook book);
err_t db_book_get(db_t db, const char *path, struct DbBook *book, bool *is_found);
void db_book_destroy(db_t db, struct DbBook *book);

#endif // EBOOK_READER_DB_H
