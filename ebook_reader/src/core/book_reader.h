#ifndef EBOOK_READER_CORE_READER_H
#define EBOOK_READER_CORE_READER_H

#include "core/core_internal.h"

ebk_error_t ebk_corem_reader_init(ebk_core_module_t, ebk_core_t);
void ebk_corem_reader_open(ebk_core_module_t, ebk_core_ctx_t, void*);
void ebk_corem_reader_next_page(ebk_core_module_t, ebk_core_ctx_t, void*);
void ebk_corem_reader_prev_page(ebk_core_module_t, ebk_core_ctx_t, void*);
void ebk_corem_reader_destroy(ebk_core_module_t);
#endif // EBOOK_READER_CORE_READER_H
