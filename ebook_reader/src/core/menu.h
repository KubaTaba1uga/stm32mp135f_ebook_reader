#ifndef EBOOK_READER_CORE_MENU_H
#define EBOOK_READER_CORE_MENU_H

#include "utils/error.h"

struct ebk_Menu {};

typedef struct ebk_Menu *ebk_menu_t;

/**
  @brief Initialize menu module.
  @param module Module struct wich menu populates.

  Initialization should be done once per lifetime module.
  Once module is initialized it needs to be also destroyed.
*/
ebk_error_t ebk_menu_init(ebk_menu_t module);

/**
  @brief Open menu.
  @param module Menu module to open.
  @return 0 on success and ebk_error_t on error.

  Menu instance can be opened and closed multiple times.
*/
ebk_error_t ebk_menu_open(ebk_menu_t module, void *config);

/**
  @brief Close menu.
  @param module Menu module to close.
  @return 0 on success and ebk_error_t on error.
*/
void ebk_menu_close(ebk_menu_t module);

/**
  @brief Destroy menu.
  @param module Menu module to close.
  @return 0 on success and ebk_error_t on error.

  Once module is initialized it needs to be also destroyed.

*/
void ebk_menu_destroy(ebk_menu_t module);

#endif // EBOOK_READER_CORE_MENU_H
