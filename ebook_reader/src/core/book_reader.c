#include <stdio.h>

#include "core/book_reader.h"

ebk_error_t ebk_corem_reader_init(ebk_core_module_t module, ebk_core_t core) {
  puts(__func__);
  return 0;
}
void ebk_corem_reader_open(ebk_core_module_t module, ebk_core_ctx_t ctx,
                           void *data) {
  puts(__func__);
}
void ebk_corem_reader_next_page(ebk_core_module_t module, ebk_core_ctx_t ctx,
                                void *data) {
  puts(__func__);
}
void ebk_corem_reader_prev_page(ebk_core_module_t module, ebk_core_ctx_t ctx,
                                void *data) {
  puts(__func__);
}
void ebk_corem_reader_destroy(ebk_core_module_t module) { puts(__func__); }
