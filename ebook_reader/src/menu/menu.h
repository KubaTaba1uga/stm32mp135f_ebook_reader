#ifndef EBOOK_READER_MENU_H
#define EBOOK_READER_MENU_H

#include "display/display.h"
#include "event_queue/event_queue.h"
#include "library/library.h"
#include "utils/err.h"

typedef struct Menu *menu_t;

err_t menu_init(menu_t *out, display_t display, event_queue_t queue,
                library_t library);
void menu_destroy(menu_t *out);

#endif // EBOOK_READER_MENU_H
