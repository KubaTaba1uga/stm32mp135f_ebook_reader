#ifndef EBOOK_READER_READER_SCREEN_H
#define EBOOK_READER_READER_SCREEN_H

#include "event_bus/event_bus.h"
#include "utils/err.h"
#include "display/display.h"

typedef struct ReaderScreen *reader_screen_t;

err_t reader_screen_init(reader_screen_t *out, display_t display, bus_t bus);
void reader_screen_destroy(reader_screen_t *out);

#endif // EBOOK_READER_READER_SCREEN_H
