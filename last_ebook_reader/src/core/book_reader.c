#include <stdio.h>
#include <string.h>

#include "core/book_reader.h"
#include "core/core_internal.h"
#include "utils/mem.h"

typedef struct ebk_Reader *ebk_reader_t;

struct ebk_Reader {
  ebk_core_t core;
};

static void ebk_corem_reader_open(ebk_core_module_t, ebk_core_ctx_t, void *);
static void ebk_corem_reader_close(ebk_core_module_t);
static void ebk_corem_reader_destroy(ebk_core_module_t);

ebk_error_t ebk_corem_reader_init(ebk_core_module_t module, ebk_core_t core) {
  puts(__func__);

  ebk_reader_t reader = ebk_mem_malloc(sizeof(struct ebk_Reader));
  *reader = (struct ebk_Reader){
      .core = core,
  };

  *module = (struct ebk_CoreModule){
      .open = ebk_corem_reader_open,
      .close = ebk_corem_reader_close,
      .destroy = ebk_corem_reader_destroy,
      .private = reader,
  };

  return 0;
}

void ebk_corem_reader_next_page(ebk_core_module_t module, ebk_core_ctx_t ctx,
                                void *data) {
  puts(__func__);
}
void ebk_corem_reader_prev_page(ebk_core_module_t module, ebk_core_ctx_t ctx,
                                void *data) {
  puts(__func__);
}

static void ebk_corem_reader_open(ebk_core_module_t module, ebk_core_ctx_t ctx,
                                  void *data) {
  puts(__func__);
}

static void ebk_corem_reader_close(ebk_core_module_t module) { puts(__func__); }

static void ebk_corem_reader_destroy(ebk_core_module_t module) {
  puts(__func__);
  if (!module->private) {
    return;
  }

  ebk_mem_free(module->private);
  memset(module, 0, sizeof(struct ebk_CoreModule));
}
