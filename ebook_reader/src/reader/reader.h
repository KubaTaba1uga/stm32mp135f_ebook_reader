#ifndef EBOOK_READER_READER_H
#define EBOOK_READER_READER_H

#include "display/display.h"
#include "event_queue/event_queue.h"
#include "library/library.h"
#include "utils/err.h"

typedef struct Reader *reader_t;

err_t reader_init(reader_t *out, display_t display, event_queue_t queue,
                library_t library);
void reader_destroy(reader_t *out);

#endif // EBOOK_READER_READER_H
