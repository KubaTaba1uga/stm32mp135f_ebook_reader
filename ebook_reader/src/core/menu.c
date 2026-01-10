#include <stdio.h>
#include <string.h>

#include "core/core_internal.h"
#include "core/menu.h"
#include "utils/mem.h"

typedef struct ebk_Menu *ebk_menu_t;

struct ebk_Menu {
  ebk_core_t core;
};

static void ebk_corem_menu_open(ebk_core_module_t, ebk_core_ctx_t, void *);
static void ebk_corem_menu_close(ebk_core_module_t);
static void ebk_corem_menu_destroy(ebk_core_module_t);

ebk_error_t ebk_corem_menu_init(ebk_core_module_t module, ebk_core_t core) {
  puts(__func__);

  ebk_menu_t menu = ebk_mem_malloc(sizeof(struct ebk_Menu));
  *menu = (struct ebk_Menu){
      .core = core,
  };

  *module = (struct ebk_CoreModule){
      .open = ebk_corem_menu_open,
      .close = ebk_corem_menu_close,
      .destroy = ebk_corem_menu_destroy,
      .private = menu,
  };

  return 0;
}

static void ebk_corem_menu_open(ebk_core_module_t module, ebk_core_ctx_t ctx,
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

static void ebk_corem_menu_destroy(ebk_core_module_t module) {
  puts(__func__);
  if (!module->private) {
    return;
  }

  ebk_mem_free(module->private);
  memset(module, 0, sizeof(struct ebk_CoreModule));
}

static void ebk_corem_menu_close(ebk_core_module_t module) {
  puts(__func__);
  }
