#include "book/book.h"
#include "book/book_internal.h"
#include "book/pdf.h"
#include "utils/error.h"
#include "utils/mem.h"

struct ebk_BooksCore {
  struct ebk_BookModule modules[ebk_BookExtensionEnum_PDF + 1];
};

ebk_error_t ebk_books_init(ebk_books_t *out) {
  ebk_books_t core = *out = ebk_mem_malloc(sizeof(struct ebk_BooksCore));

  ebk_errno = ebk_book_pdf_init(&core->modules[ebk_BookExtensionEnum_PDF]);
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
  ebk_books_list_t list = *out = ebk_mem_malloc(sizeof(struct ebk_BooksList));
  *list = (struct ebk_BooksList){
      .owner = core,
  };

  
  
  return 0;
};

/**
   @brief Get book from books list.
   @param list List of books to get book from.
   @return Pointer to book on success, on error NULL and set ebk_errno.

   To get all books from list, you need to execute ebk_books_get until
   it return NULL. It return NULL and set ebk_errno with ENOENT code.
*/
ebk_book_t ebk_books_list_get(ebk_books_list_t list);
void ebk_books_list_destroy(ebk_books_list_t *out);
