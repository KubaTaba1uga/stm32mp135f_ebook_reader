#ifndef EBOOK_READER_MENU_SCREEN_H
#define EBOOK_READER_MENU_SCREEN_H

#include "event_bus/event_bus.h"
#include "utils/err.h"
#include "display/display.h"

typedef struct MenuScreen *menu_screen_t;

err_t menu_screen_init(menu_screen_t *out, display_t display, bus_t bus);
void menu_screen_destroy(menu_screen_t *out);

#endif // EBOOK_READER_MENU_SCREEN_H
