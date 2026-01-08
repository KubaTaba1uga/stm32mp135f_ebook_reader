#ifndef EBOOK_READER_CORE_INTERNAL_H
#define EBOOK_READER_CORE_INTERNAL_H

/**
   This is interface for all interactions between subsystem and core, where the
   interaction is started by subsystem.

   Example of such interaction is opening menu from the IN_READER state.
   Reader get ebk_Core using offsetof and then do ebk_core_open_menu on the
   core.

   Each module in this subsystem provide generic interface.
 */
#include <stdbool.h>

#include "core/book_reader.h"
#include "core/menu.h"
#include "core/start.h"
#include "gui/gui.h"
#include "utils/error.h"

typedef struct ebk_Core *ebk_core_t;
typedef struct ebk_CoreModule *ebk_core_module_t;

#define EBK_CORE_STATE_ENUM_INIT -1

enum ebk_CoreStateEnum {
  ebk_CoreStateEnum_STARTING = 0,
  ebk_CoreStateEnum_IN_MENU,
  ebk_CoreStateEnum_IN_READER,
  ebk_CoreStateEnum_IN_ERROR,
};

struct ebk_Core {
  enum ebk_CoreStateEnum state;
  struct ebk_Start start;
  struct ebk_Menu menu;
  struct ebk_Reader reader;
  ebk_gui_t gui;
  bool on;
};

/**
   @brief Initiate appropriate core state.
   @param core Core instance to initiate state in.
   @param state State to initiate.
   @return 0 on success and ebk_error_t on error.

   Initializing a state means only that module repsponsible for handling a state
   is initialized, it does not mean that core state changes.
*/
ebk_error_t ebk_core_sinit(ebk_core_t core, enum ebk_CoreStateEnum state);

/**
   @brief Open appropriate core state.
   @param core Core instance to initiate state in.
   @param state State to initiate.
   @return 0 on success and ebk_error_t on error.

   Open change state of core and close the old state.
*/
ebk_error_t ebk_core_sopen(ebk_core_t core, enum ebk_CoreStateEnum state,
                           void *config);

void ebk_core_sclose(ebk_core_t core, enum ebk_CoreStateEnum state);
const char *ebk_core_sdump(enum ebk_CoreStateEnum state);

#endif // EBOOK_READER_CORE_INTERNAL_H
