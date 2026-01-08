#ifndef EBOOK_READER_CORE
#define EBOOK_READER_CORE

#include "utils/error.h"

typedef struct ebk_Core *ebk_core_t;

/**
   @brief Initialize whole app.
   @param[out] out Pointer to new instance of core.
   @return 0 on success and ebk_errno on error.

   Once core init is done all modules are initialized and we
   can start looking on app as one big state machine.

   GUI intercept user input and deliver it to core, core chooses
   module to perform interactions on input based on it's state,
   and module perform actual work.

   Role of core is to ensure system is properly intialized and
   to dispatch input properly on it's state.

   All app logic should be performed by one of the modules.
*/
ebk_error_t ebk_core_init(ebk_core_t *out);

/**
  @brief Start app's main event loop.
  @param[in] core Pointer to initialized instance of core.
  @return 0 on success and ebk_errno on error.
 */
ebk_error_t ebk_core_main(ebk_core_t core);


/**
  @brief Destroy whole app.
  @param[out] out Instance of core to be destroyed.
 */
void ebk_core_destroy(ebk_core_t *out);

#endif // EBOOK_READER_CORE
