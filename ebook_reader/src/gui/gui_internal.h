#ifndef EBOOK_READER_CORE_INTERNAL_H
#define EBOOK_READER_CORE_INTERNAL_H
#include <stdbool.h>

#include "gui/gui.h"
#include "utils/error.h"

struct ebk_Gui {
  struct {
    void (*callback)(enum ebk_GuiInputEventEnum event, void *data);
    void *data;
  } inputh;
};

#endif // EBOOK_READER_CORE_INTERNAL_H
