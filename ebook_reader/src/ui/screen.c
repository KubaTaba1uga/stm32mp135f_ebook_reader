#include "ui/screen.h"
#include "ui/ui.h"
#include "utils/err.h"
#include "utils/mem.h"

struct UiScreen {
  void (*destroy)(void *);
  void *private;
  ui_t owner;
};

err_t ui_screen_create(ui_screen_t *out, ui_t ui, void (*destroy)(void *),
                     void *private) {
  ui_screen_t screen = *out = mem_malloc(sizeof(struct UiScreen));
  screen->destroy = destroy;
  screen->private = private;
  screen->owner = ui;

  return 0;
}

void ui_screen_destroy(ui_screen_t *out) {
  if (!out || !*out) {
    return;
  }

  if ((*out)->destroy) {
    (*out)->destroy((*out)->private);
  }
  
  mem_free(*out);
  *out = NULL;
};

ui_t ui_screen_get_ui(ui_screen_t screen) { return screen->owner; }

void * ui_screen_get_private(ui_screen_t screen) { return screen->private;}
