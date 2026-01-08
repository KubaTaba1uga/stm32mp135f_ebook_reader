#ifndef EBOOK_READER_CORE_START_H
#define EBOOK_READER_CORE_START_H
/**
  Start sequence is basically the entrypoint of the device.
  Here we do stuff like initialise drivers, read books from
  sdcard to db and generate pictures for them.

  Goal of start sequence is to initialize system to state when
  it is usable by user and core can move between menu, reader
  settings etc.

*/
#include "core/core.h"
#include "utils/error.h"

struct ebk_Start {};

typedef struct ebk_Start *ebk_start_t;

/**
  @brief Initialize start module.
  @param module Module struct wich start populates.
  @return 0 on success ebk_errno on error.
*/
ebk_error_t ebk_start_init(ebk_start_t module);

struct ebk_StartOpenConfig {
  ebk_core_t core;
};

/**
  @brief Open start module.
  @param module Module to open.
  @param config Pointer to `struct ebk_StartOpenConfig`.
  @return 0 on success ebk_errno on error.

  Start module when opened first renders the gui into loading screen.
  And once loading screen is shown it initializes all modules in repo
  and libs.
*/
ebk_error_t ebk_start_open(ebk_start_t module, void *config);

/**
  @brief Close start module.
  @param module Module to close.
*/
void ebk_start_close(ebk_start_t module);

void ebk_start_destroy(ebk_start_t module);

#endif // EBOOK_READER_CORE_START_H
