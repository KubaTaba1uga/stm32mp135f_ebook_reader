#ifndef EBOOK_READER_GUI_H
#define EBOOK_READER_GUI_H
#include "utils/error.h"

typedef struct Gui *gui_t;

cdk_error_t gui_init(gui_t *);
cdk_error_t gui_start(gui_t);
void gui_stop(gui_t);
void gui_destroy(gui_t *);

#endif // EBOOK_READER_GUI_H
