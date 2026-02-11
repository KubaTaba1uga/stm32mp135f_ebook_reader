#ifndef EBOOK_READER_READER_H
#define EBOOK_READER_READER_H

#include "utils/err.h"

typedef struct Reader *reader_t;

err_t reader_init(reader_t *out, bus_t bus);
void reader_destroy(reader_t *out);

#endif // EBOOK_READER_READER_H
