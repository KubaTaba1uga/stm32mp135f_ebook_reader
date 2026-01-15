#ifndef EBOOK_READER_DB_H
#define EBOOK_READER_DB_H

#include "utils/error.h"

typedef struct ebk_Db *ebk_db_t;

ebk_error_t ebk_db_init(ebk_db_t *out);
void ebk_db_destroy(ebk_db_t *out);

#endif // EBOOK_READER_CORE_MENU
