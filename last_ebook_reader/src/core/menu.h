#ifndef EBOOK_READER_CORE_MENU_H
#define EBOOK_READER_CORE_MENU_H

#include "core/core_internal.h"
#include "utils/error.h"

ebk_error_t ebk_corem_menu_init(ebk_core_module_t, ebk_core_t);
void ebk_corem_menu_select_book(ebk_core_module_t module, ebk_core_ctx_t ctx,
				void *data);

#endif // EBOOK_READER_CORE_MENU_H
