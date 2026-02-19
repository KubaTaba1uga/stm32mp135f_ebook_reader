#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

#include "db/db.h"
#include "utils/err.h"
#include "utils/mem.h"

struct Db {
  sqlite3 *db;
};

err_t db_init(db_t *out) {
  db_t db = *out = mem_malloc(sizeof(struct Db));
  *db = (struct Db){0};

  int err = sqlite3_open("ebk.db", &db->db);
  if (err) {
    err_o = err_errnof(err, "Cannot open connection to ebk.db: %s",
                       sqlite3_errmsg(db->db));
    goto error_out;
  }

  err = sqlite3_exec(db->db,
                     "CREATE TABLE if NOT EXISTS library ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                     "title TEXT NOT NULL,"
                     "path TEXT NOT NULL,"
                     "max_page_number INTEGER,"
                     "page_number INTEGER,"
                     "thumbnail_buf BLOB,"
                     ");",
                     NULL, NULL, NULL);
  if (err) {
    puts(strerror(err));
    err_o = err_errnof(err, "Cannot create 'library' table: %s",
                       sqlite3_errmsg(db->db));
    goto error_db_cleanup;
  }

  return 0;

error_db_cleanup:
  sqlite3_close(db->db);
error_out:
  mem_free(db);
  *out = NULL;
  return err_o;
}

void db_destroy(db_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }

  sqlite3_close((*out)->db);
  mem_free(*out);
  *out = NULL;
}

err_t db_book_insert(db_t db, struct DbBook book) {
  sqlite3_stmt *st = NULL;
  sqlite3_prepare_v2(
      db->db,
      "INSERT INTO library(title, path, max_page_number, page_number, "
      "thumbnail_buf) "
      "VALUES(?, ?, ?, ?, ?);",
      -1, &st, NULL);

  sqlite3_bind_text(st, 1, book.title, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(st, 2, book.path, -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(st, 3, book.max_page_number);
  sqlite3_bind_int(st, 4, book.page_number);
  sqlite3_bind_blob(st, 5, book.thumbnail.buf, book.thumbnail.len,
                    SQLITE_TRANSIENT);

  sqlite3_step(st);
  sqlite3_finalize(st);

  return 0;
};

err_t db_book_get(db_t db, const char *path, struct DbBook *book,
                  bool *is_found) {
  int err;
  sqlite3_stmt *st = NULL;

  err = sqlite3_prepare_v2(db->db,
                           "SELECT id, title, path, max_page_number, "
                           "page_number, thumbnail_buf "
                           "FROM library WHERE path = ? LIMIT 1;",
                           -1, &st, NULL);
  if (err) {
    err_o =
        err_errnof(err, "Cannot get book by path: %s", sqlite3_errmsg(db->db));
    goto error_out;
  }

  err = sqlite3_bind_text(st, 1, path, -1, SQLITE_TRANSIENT);
  if (err) {
    err_o = err_errnof(err, "Cannot bind path to query: %s",
                       sqlite3_errmsg(db->db));
    goto error_out;
  }

  err = sqlite3_step(st);
  if (err == SQLITE_ROW) {
    const char *title = (const char *)sqlite3_column_text(st, 1);
    const char *path = (const char *)sqlite3_column_text(st, 2);
    int max_page_number = sqlite3_column_int(st, 3);
    int page_number = sqlite3_column_int(st, 4);
    const unsigned char *blob = sqlite3_column_blob(st, 5);
    int blob_len = sqlite3_column_bytes(st, 5);

    *is_found = true;
    *book = (struct DbBook){
        .title = title,
        .path = path,
        .max_page_number = max_page_number,
        .page_number = page_number,
        .thumbnail =
            {
                .buf = blob,
                .len = blob_len,
            },
        .priv = st,
    };
  } else if (err == SQLITE_DONE) {
    *is_found = false;
    sqlite3_finalize(st);
  } else {
    err_o =
        err_errnof(err, "Cannot find book by path: %s", sqlite3_errmsg(db->db));
    goto error_out;
  }

  return 0;

error_out:
  *book = (struct DbBook){0};
  sqlite3_finalize(st);
  return err_o;
}

void db_book_destroy(db_t db, struct DbBook *book) {
  if (!book->priv) {
    return;
  }

  sqlite3_finalize((void *)book->priv);
  *book = (struct DbBook){0};
};
