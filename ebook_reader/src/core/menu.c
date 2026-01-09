#include <stdio.h>

#include "core/menu.h"

ebk_error_t ebk_corem_menu_init(ebk_core_module_t module, ebk_core_t core) {
  puts(__func__);
  return 0;
}

void ebk_corem_menu_open(ebk_core_module_t module, ebk_core_ctx_t ctx,
                         void *data) {
  puts(__func__);
}

void ebk_corem_menu_up(ebk_core_module_t module, ebk_core_ctx_t ctx,
                       void *data) {
  puts(__func__);
}

void ebk_corem_menu_down(ebk_core_module_t module, ebk_core_ctx_t ctx,
                         void *data) {
  puts(__func__);
}

void ebk_corem_menu_left(ebk_core_module_t module, ebk_core_ctx_t ctx,
                         void *data) {
  puts(__func__);
}

void ebk_corem_menu_rigth(ebk_core_module_t module, ebk_core_ctx_t ctx,
                          void *data) {
  puts(__func__);
}

void ebk_corem_menu_destroy(ebk_core_module_t module) { puts(__func__); }
