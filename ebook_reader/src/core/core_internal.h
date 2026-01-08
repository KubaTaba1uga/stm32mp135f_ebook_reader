#ifndef EBOOK_READER_CORE_INTERNAL_H
#define EBOOK_READER_CORE_INTERNAL_H
#include <stdbool.h>

#include "gui/gui.h"
#include "utils/error.h"

typedef struct ebk_Core *ebk_core_t;
typedef struct ebk_CoreModule *ebk_core_module_t;

#define EBK_CORE_STATE_ENUM_INIT -1

enum ebk_CoreStateEnum {
  ebk_CoreStateEnum_IN_MENU = 0,
  ebk_CoreStateEnum_IN_READER,
  ebk_CoreStateEnum_IN_ERROR,
  // Add more states here
  ebk_CoreStateEnum_MAX,
};

struct ebk_CoreModule {
  ebk_error_t (*init)(struct ebk_CoreModule *module, struct ebk_Core *core);
  ebk_error_t (*open)(struct ebk_CoreModule *module, void *config);
  ebk_error_t (*handle_input)(struct ebk_CoreModule *module, int input_event);  
  void (*destroy)(struct ebk_CoreModule *module);
  void (*close)(struct ebk_CoreModule *module);
  void *private;
};

/**
   @brief Open appropriate core state.
   @param core Core instance to set state in.
   @param state State to open.
   @return 0 on success and ebk_error_t on error.

   Open changes state of the core and close the old state.
*/
ebk_error_t ebk_core_sopen(ebk_core_t core, enum ebk_CoreStateEnum state,
                           void *config);

const char *ebk_core_sdump(enum ebk_CoreStateEnum state);

#endif // EBOOK_READER_CORE_INTERNAL_H
