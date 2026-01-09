#include "core/error.h"

ebk_error_t ebk_corem_error_init(ebk_core_module_t module, ebk_core_t core) {
  puts(__func__);
  return 0;
}

void ebk_corem_error_open(ebk_core_module_t module, ebk_core_ctx_t ctx,
                          void *data) {
  puts(__func__);
}
