#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "library/core.h"
#include "library/library.h"
#include "utils/err.h"
#include "utils/log.h"
#include "utils/mem.h"
#include "utils/settings.h"
#include "utils/zlist.h"

#define CAST_BOOK_PRIV(node) mem_container_of(node, struct Book, next)

struct Library {
  struct BookModule modules[BookExtensionEnum_MAX];
};

struct BooksList {
  zlist_node_t current_book;
  struct ZList books;
  library_t owner;
};

static int book_get_extension(library_t lib, const char *path);
static void books_list_destroy(void *data);
static void book_destroy(void *data);

err_t library_init(library_t *out) {
  library_t lib = *out = mem_malloc(sizeof(struct Library));

  err_t (*module_inits[BookExtensionEnum_MAX])(book_module_t, library_t) = {
      [BookExtensionEnum_PDF] = book_module_pdf_init,
  };
  int inits_status;
  for (inits_status = BookExtensionEnum_PDF;
       inits_status < BookExtensionEnum_MAX; inits_status++) {
    if (!module_inits[inits_status]) {
      continue;
    }

    err_o = module_inits[inits_status](&lib->modules[inits_status], lib);
    ERR_TRY(err_o);
  }

  return 0;

error_out:
  for (; inits_status >= BookExtensionEnum_PDF; inits_status--) {
    if (!lib->modules[inits_status].destroy) {
      continue;
    }
    lib->modules[inits_status].destroy(&lib->modules[inits_status]);
  }

  mem_free(*out);
  *out = NULL;
  return err_o;
};

void library_destroy(library_t *out) {
  puts(__func__);
  if (!out || !*out) {
    return;
  }

  library_t lib = *out;
  for (int inits_status = BookExtensionEnum_MAX - 1;
       inits_status >= BookExtensionEnum_PDF; inits_status--) {
    if (!lib->modules[inits_status].destroy) {
      continue;
    }
    lib->modules[inits_status].destroy(&lib->modules[inits_status]);
  }

  mem_free(*out);
  *out = NULL;
};

books_list_t library_list_books(library_t lib) {
  struct dirent *dirent;
  DIR *books_dir;
  int book_ext;
  book_t book;

  books_list_t list =
      mem_refalloc(sizeof(struct BooksList), books_list_destroy);
  *list = (struct BooksList){
      .owner = lib,
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

    book_ext = book_get_extension(lib, dirent->d_name);
    if (book_ext == -1) {
      continue;
    };

    int bytes =
        snprintf(NULL, 0, "%s/%s", settings_books_dir, dirent->d_name) + 1;
    char *file_path = mem_malloc(bytes);
    snprintf(file_path, bytes, "%s/%s", settings_books_dir, dirent->d_name);

    log_debug("Creating book: %s", file_path);

    book = mem_refalloc(sizeof(struct Book), book_destroy);

    *book = (struct Book){
        .extension = book_ext,
        .file_path = file_path,
        .owner = lib,
        .scale = 1,
        .page_number = 1,
    };

    zlist_append(&list->books, &book->next);

    if (!list->current_book) {
      list->current_book = list->books.head;
    }

    if (lib->modules[book_ext].book_init) {
      err_o = lib->modules[book_ext].book_init(book);
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

static void books_list_destroy(void *data) {
  puts(__func__);

  books_list_t blist = data;
  if (!blist) {
    return;
  }

  for (zlist_node_t node = blist->books.head; node != NULL;) {
    book_t book = CAST_BOOK_PRIV(node);
    node = node->next;
    mem_deref(book);
  }
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

static int book_get_extension(library_t lib, const char *path) {

  for (int i = BookExtensionEnum_PDF; i < BookExtensionEnum_MAX; i++) {
    if (!lib->modules[i].is_extension) {
      continue;
    }

    if (lib->modules[i].is_extension(path)) {
      return i;
    };
  }

  return -1;
}

const char *book_get_title(book_t book) { return book->title; }

const unsigned char *book_get_thumbnail(book_t book, int x, int y) {
  return book->owner->modules[book->extension].book_get_thumbnail(book, x, y);
}

int books_list_len(books_list_t list) { return list->books.len; }

book_t books_list_pop(books_list_t list, int idx) {
  assert(list != NULL);
  zlist_node_t book_node = zlist_pop(&list->books, idx);
  assert(book_node != NULL);
  book_t book = CAST_BOOK_PRIV(book_node);
  assert(book != NULL);
  return book;
}

static void book_destroy(void *data) {
  book_t book = data;

  if (!book) {
    return;
  }

  book->owner->modules[book->extension].book_destroy(book);
  mem_free((void *)book->file_path);
};

void books_list_remove(books_list_t list, book_t book) {
  int i = 0;
  for (zlist_node_t node = list->books.head; node != NULL; node = node->next) {
    i++;
  }

  (void)books_list_pop(list, i);
}

const unsigned char *book_get_page(book_t book, int x, int y, int *buf_len) {
  puts(__func__);
  return book->owner->modules[book->extension].book_get_page(book, x, y,
                                                             buf_len);
}

int book_get_page_no(book_t book) { return book->page_number; }

void book_set_page_no(book_t book, int page_no) {
  if (page_no >= book->max_page_number) {
    page_no = book->max_page_number;
  } else if (page_no < 1) {
    page_no = 1;
  }

  book->page_number = page_no;
}

void book_set_scale(book_t book, double value) { book->scale = value; }

int book_get_max_page_no(book_t book) { return book->max_page_number; }

double book_get_scale(book_t book) { return book->scale; }

int book_get_x_off(book_t book) { return book->x_off; }

int book_get_y_off(book_t book) { return book->y_off; }

void book_set_x_off(book_t book, int value) { book->x_off = value; }

void book_set_y_off(book_t book, int value) { book->y_off = value; }
