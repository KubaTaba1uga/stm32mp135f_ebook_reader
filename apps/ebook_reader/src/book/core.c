#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "book/book.h"
#include "book/core.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"
#include "utils/settings.h"
#include "utils/zlist.h"

#define CAST_BOOK_PRIV(node) mem_container_of(node, struct Book, next)

struct BookApi {
  struct BookModule modules[BookExtensionEnum_MAX];
};

struct BooksList {
  zlist_node_t current_book;
  struct ZList books;
  book_api_t owner;
};

static int book_get_extension(book_api_t api, const char *path);

err_t book_api_init(book_api_t *out) {
  book_api_t api = *out = mem_malloc(sizeof(struct BookApi));

  err_t (*module_inits[BookExtensionEnum_MAX])(book_module_t, book_api_t) = {
      [BookExtensionEnum_PDF] = book_module_pdf_init,
  };
  int inits_status;
  for (inits_status = BookExtensionEnum_PDF;
       inits_status < BookExtensionEnum_MAX; inits_status++) {
    if (!module_inits[inits_status]) {
      continue;
    }

    err_o = module_inits[inits_status](&api->modules[inits_status], api);
    ERR_TRY(err_o);
  }

  return 0;

error_out:
  for (; inits_status >= BookExtensionEnum_PDF; inits_status--) {
    if (!api->modules[inits_status].destroy) {
      continue;
    }
    api->modules[inits_status].destroy(&api->modules[inits_status]);
  }

  mem_free(*out);
  *out = NULL;
  return err_o;
};

void book_api_destroy(book_api_t *out) {
  if (!out || !*out) {
    return;
  }

  book_api_t api = *out;
  for (int inits_status = BookExtensionEnum_MAX - 1;
       inits_status >= BookExtensionEnum_PDF; inits_status--) {
    if (!api->modules[inits_status].destroy) {
      continue;
    }
    api->modules[inits_status].destroy(&api->modules[inits_status]);
  }

  mem_free(*out);
  *out = NULL;
};

books_list_t book_api_find_books(book_api_t api) {
  struct dirent *dirent;
  DIR *books_dir;
  int book_ext;
  book_t book;

  books_list_t list = mem_malloc(sizeof(struct BooksList));
  *list = (struct BooksList){
      .owner = api,
  };

  books_dir = opendir(settings_books_dir);
  if (!books_dir) {
    err_o = err_errnof(errno, "Cannot open directory: %s", books_dir);
    goto error_out;
  }

  while ((dirent = readdir(books_dir)) != NULL) {
    if (strcmp(".", dirent->d_name) == 0 || strcmp("..", dirent->d_name) == 0) {
      continue;
    }

    book_ext = book_get_extension(api, dirent->d_name);
    if (book_ext == -1) {
      continue;
    };

    int bytes =
        snprintf(NULL, 0, "%s/%s", settings_books_dir, dirent->d_name) + 1;
    char *file_path = mem_malloc(bytes);
    snprintf(file_path, bytes, "%s/%s", settings_books_dir, dirent->d_name);

    log_debug("Creating book: %s", file_path);

    book = mem_malloc(sizeof(struct Book));

    *book = (struct Book){
        .extension = book_ext,
        .file_path = file_path,
        .owner = api,
        .scale = 1,
    };

    zlist_append(&list->books, &book->next);

    if (!list->current_book) {
      list->current_book = list->books.head;
    }

    if (api->modules[book_ext].book_init) {
      err_o = api->modules[book_ext].book_init(book);
      ERR_TRY_CATCH(err_o, error_list_cleanup);
    }
  }

  closedir(books_dir);

  return list;

error_list_cleanup:
  books_list_destroy(list);
  closedir(books_dir);
error_out:
  mem_free(list);
  return NULL;
};

void books_list_destroy(books_list_t blist) {
  if (!blist) {
    return;
  }

  book_api_t api = blist->owner;
  for (zlist_node_t node = blist->books.head; node != NULL;) {
    book_t book = CAST_BOOK_PRIV(node);

    if (api->modules[book->extension].book_destroy) {
      api->modules[book->extension].book_destroy(book);
    }

    node = node->next;
    mem_free((void *)book->file_path);
    mem_free(book);
  }

  mem_free(blist);
};

book_t books_list_get(books_list_t list) {
  if (!list->current_book) {
    return NULL;
  }

  book_t current_book = CAST_BOOK_PRIV(list->current_book);
  list->current_book = list->current_book->next;

  return current_book;
}

void books_list_reset(books_list_t list) {
  list->current_book = list->books.head;
}

static int book_get_extension(book_api_t api, const char *path) {

  for (int i = BookExtensionEnum_PDF; i < BookExtensionEnum_MAX; i++) {
    if (!api->modules[i].is_extension) {
      continue;
    }

    if (api->modules[i].is_extension(path)) {
      return i;
    };
  }

  return -1;
}

const char *book_get_title(book_t book) {
  return book->title;
}

const unsigned char *book_get_thumbnail(book_t book, int x, int y) {
  return book->owner->modules[book->extension].book_get_thumbnail(book, x, y);
}

int books_list_len(books_list_t list) { return list->books.len; }

book_t books_list_pop(books_list_t list, int idx) {
  zlist_node_t book_node = zlist_pop(&list->books, idx);
  assert(book_node != NULL);
  book_t book = CAST_BOOK_PRIV(book_node);
  assert(book != NULL);
  return book;
}

const unsigned char *book_get_page(book_t book, int x, int y, int *buf_len) {
  puts(__func__);
  return book->owner->modules[book->extension].book_get_page(book, x, y,
                                                             buf_len);
}

void book_set_x_offset(book_t book, int value) { book->x_off = value; }

void book_set_y_offset(book_t book, int value) { book->y_off = value; }

void book_set_scale(book_t book, double value) { book->scale = value; }

void book_destroy(book_t *book) {
  if (!book || !*book) {
    return;
  }

  (*book)->owner->modules[(*book)->extension].book_destroy(*book);
  mem_free((void *)(*book)->file_path);
  mem_free(*book);
  *book = NULL;
};

int book_get_max_page_no(book_t book) { return book->max_page_number; }

int book_get_page_no(book_t book) { return book->page_number; }

void book_set_page_no(book_t book, int page_no) {
  book->page_number =
      page_no < book->max_page_number ? page_no : book->max_page_number;
}

