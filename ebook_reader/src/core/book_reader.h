#ifndef EBOOK_READER_CORE_READER_H
#define EBOOK_READER_CORE_READER_H

#include "utils/error.h"

struct ebk_Reader {};

typedef struct ebk_Reader *ebk_reader_t;

/**
  @brief Initialize reader module.
  @param module Module struct wich reader populates.

  Initialization should be done once per lifetime module.
  Once module is initialized it needs to be also destroyed.
*/
ebk_error_t ebk_reader_init(ebk_reader_t module);

/**
  @brief Open reader.
  @param module Reader module to open.
  @return 0 on success and ebk_error_t on error.

  Reader instance can be opened and closed multiple times.
*/
ebk_error_t ebk_reader_open(ebk_reader_t module, void *config);

/**
  @brief Close reader.
  @param module Reader module to close.
  @return 0 on success and ebk_error_t on error.
*/
void ebk_reader_close(ebk_reader_t module);

/**
  @brief Destroy reader.
  @param module Reader module to close.
  @return 0 on success and ebk_error_t on error.

  Once module is initialized it needs to be also destroyed.

*/
void ebk_reader_destroy(ebk_reader_t module);

#endif // EBOOK_READER_CORE_READER_H
