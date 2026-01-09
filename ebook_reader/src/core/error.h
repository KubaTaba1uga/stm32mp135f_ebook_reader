#ifndef EBOOK_READER_CORE_ERROR_H
#define EBOOK_READER_CORE_ERROR_H

#include "core/core_internal.h"

ebk_error_t ebk_corem_error_init(ebk_core_module_t, ebk_core_t);
void ebk_corem_error_open(ebk_core_module_t, ebk_core_ctx_t, void*);
#endif // EBOOK_READER_CORE_ERROR_H
