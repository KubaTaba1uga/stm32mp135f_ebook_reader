#include <db/db.h>

struct ebk_Db {
  void *db_connection;
};

ebk_error_t ebk_db_init(ebk_db_t *out) { return 0; }

void ebk_db_destroy(ebk_db_t *out) {
  
};
