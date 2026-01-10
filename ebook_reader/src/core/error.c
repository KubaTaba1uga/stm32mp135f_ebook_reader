#include "core/error.h"

#include <stdio.h>
#include <string.h>

#include "core/core_internal.h"
#include "core/error.h"
#include "utils/mem.h"

typedef struct ebk_CoremError *ebk_corem_error_t;

struct ebk_CoremError {
  ebk_core_t core;
};

static void ebk_corem_error_open(ebk_core_module_t, ebk_core_ctx_t, void *);
static void ebk_corem_error_close(ebk_core_module_t);
static void ebk_corem_error_destroy(ebk_core_module_t);

ebk_error_t ebk_corem_error_init(ebk_core_module_t module, ebk_core_t core) {
  puts(__func__);

  ebk_corem_error_t error = ebk_mem_malloc(sizeof(struct ebk_CoremError));
  *error = (struct ebk_CoremError){
      .core = core,
  };

  *module = (struct ebk_CoreModule){
      .open = ebk_corem_error_open,
      .close = ebk_corem_error_close,
      .destroy = ebk_corem_error_destroy,
      .private = error,
  };

  return 0;
}

static void ebk_corem_error_open(ebk_core_module_t module, ebk_core_ctx_t ctx,
                         void *data) {
  puts(__func__);
}

static void ebk_corem_error_destroy(ebk_core_module_t module) {
  puts(__func__);
  if (!module->private) {
    return;
  }

  ebk_mem_free(module->private);
  memset(module, 0, sizeof(struct ebk_CoreModule));
}

static void ebk_corem_error_close(ebk_core_module_t module) {
  puts(__func__);
  }
