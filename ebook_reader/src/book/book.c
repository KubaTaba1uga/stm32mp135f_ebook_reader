#include "book/book.h"
#include "book/book_internal.h"
#include "book/pdf.h"
#include "utils/error.h"
#include "utils/mem.h"
#include "utils/settings.h"
#include "utils/zero_list.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define CAST_BOOK_PRIV(node) container_of(node, struct ebk_Book, list_node);

struct ebk_BooksCore {
  struct ebk_BookModule modules[ebk_BookExtensionEnum_PDF + 1];
};

static enum ebk_BookExtensionEnum ebk_books_get_extension(ebk_books_t core,
                                                          const char *path);

ebk_error_t ebk_books_init(ebk_books_t *out) {
  ebk_books_t core = *out = ebk_mem_malloc(sizeof(struct ebk_BooksCore));

  ebk_errno =
      ebk_book_pdf_init(&core->modules[ebk_BookExtensionEnum_PDF], core);
  EBK_TRY(ebk_errno);

  return 0;

error_out:
  return ebk_errno;
};

void ebk_books_destroy(ebk_books_t *out) {
  if (!out || !*out) {
    return;
  }

  ebk_books_t core = *out;
  ebk_book_module_t module = &core->modules[ebk_BookExtensionEnum_PDF];
  module->destroy(module);

  ebk_mem_free(core);
  *out = NULL;
};

/**
   @brief Search for books in the system.
   @param core Books subsystem instance.
   @param out List of books to populate.
   @return 0 on success and ebk_errno on failure.
*/
ebk_error_t ebk_books_list_init(ebk_books_t core, ebk_books_list_t *out) {
  enum ebk_BookExtensionEnum book_ext;
  struct dirent *dirent;
  ebk_book_t book;
  DIR *books_dir;
  int i;

  ebk_books_list_t list = *out = ebk_mem_malloc(sizeof(struct ebk_BooksList));
  *list = (struct ebk_BooksList){
      .owner = core,
  };

  books_dir = opendir(ebk_settings_books_dir);

  while ((dirent = readdir(books_dir)) != NULL) {
    if (strcmp(".", dirent->d_name) == 0 || strcmp("..", dirent->d_name) == 0) {
      continue;
    }

    book_ext = ebk_books_get_extension(core, dirent->d_name);
    if (book_ext == -1) {
      continue;
    };

    int bytes =
        snprintf(NULL, 0, "%s/%s", ebk_settings_books_dir, dirent->d_name) + 1;
    char *file_path = ebk_mem_malloc(bytes);
    snprintf(file_path, bytes, "%s/%s", ebk_settings_books_dir, dirent->d_name);

    book = ebk_mem_malloc(sizeof(struct ebk_Book));

    *book = (struct ebk_Book){
        .ext = book_ext,
        .file_path = file_path,
    };

    i = ebk_zlist_append(&list->books, &book->list_node);

    if (i == 0) {
      list->current_book = list->books.head;
    }

    if (core->modules[book_ext].book_init) {
      ebk_errno = core->modules[book_ext].book_init(book);
      EBK_TRY(ebk_errno);
    }
  }

  closedir(books_dir);

  return 0;

error_out:
  ebk_books_list_destroy(&list);
  closedir(books_dir);
  return ebk_errno;
};

/**
   @brief Get book from books list.
   @param list List of books to get book from.
   @return Pointer to book on success, on error NULL and set ebk_errno.

   To get all books from list, you need to execute ebk_books_get until
   it return NULL. It return NULL and set ebk_errno with ENOENT code.
*/
ebk_book_t ebk_books_list_get(ebk_books_list_t list) {
  ebk_book_t book = NULL;

  if (list->current_book) {
    ebk_zlist_node_t next_node = list->current_book->next;
    book = CAST_BOOK_PRIV(&list->current_book);
    list->current_book = next_node;
  }

  return book;
}

void ebk_books_list_destroy(ebk_books_list_t *out) {
  if (!out || !*out) {
    return;
  }

  for (ebk_zlist_node_t node = (*out)->books.head; node != NULL;) {
    ebk_book_t book = CAST_BOOK_PRIV(node);
    struct ebk_BooksCore *core = (*out)->owner;
    if (core->modules[book->ext].book_destroy) {
      core->modules[book->ext].book_destroy(book);
    }

    node = node->next;

    ebk_mem_free((void *)book->file_path);

    ebk_mem_free(book);
  }

  ebk_mem_free(*out);
  *out = NULL;
}

static enum ebk_BookExtensionEnum ebk_books_get_extension(ebk_books_t core,
                                                          const char *path) {
  ebk_book_module_t module = &core->modules[ebk_BookExtensionEnum_PDF];
  bool is_ext = module->is_extension(path);
  if (is_ext) {
    return ebk_BookExtensionEnum_PDF;
  }

  return -1;
}
