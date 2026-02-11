#ifndef EBOOK_READER_BOOK_SETTINGS_H
#define EBOOK_READER_BOOK_SETTINGS_H

#include "event_bus/event_bus.h"
#include "utils/err.h"

typedef struct BookSettings *book_settings_t;

err_t book_settings_init(book_settings_t *out, bus_t bus);
void book_settings_destroy(book_settings_t *out);

#endif // EBOOK_READER_BOOK_SETTINGS_H
