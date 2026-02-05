#include "book/book.h"
#include "display/lv_display.h"
#include "ui/screen.h"
#include "ui/widgets.h"
#include "utils/err.h"
#include "utils/mem.h"
#include <stdio.h>

typedef struct UiScreenReader *ui_screen_reader_t;

struct UiScreenReader {
  ui_wx_reader_t reader;
  lv_group_t *group;
  ui_t owner;
};

static void ui_screen_reader_destroy(void *);

err_t ui_screen_reader_init(ui_screen_t out, ui_t ui, book_t book, int event,
                            void (*event_cb)(lv_event_t *e),
                            lv_group_t *group) {
  puts(__func__);
  assert(event_cb != NULL);
  assert(group != NULL);
  assert(book != NULL);
  assert(out != NULL);
  assert(ui != NULL);

  int page_size = 0;
  const unsigned char *page_data =
      book_get_page(book, lv_display_get_horizontal_resolution(NULL),
                    lv_display_get_vertical_resolution(NULL), 7, &page_size);
  if (!page_data) {
    ERR_TRY(err_o);
  }

  assert(page_size != 0);
  
  ui_wx_reader_t reader = ui_wx_reader_create(page_size, page_data);
  if (!reader) {
    err_o = err_errnos(EINVAL, "Cannot create reader widget");
    goto error_out;
  }

  
  lv_group_add_obj(group, reader);
  lv_obj_add_event_cb(reader, event_cb, event, book);  
  ui_screen_reader_t screen = mem_malloc(sizeof(struct UiScreenReader));
  *screen = (struct UiScreenReader){
      .reader = reader,
      .group = group,
      .owner = ui,
  };

  *out = (struct UiScreen){
      .destroy = ui_screen_reader_destroy,
      .screen_data = screen,
  };

  return 0;

error_out:
  return err_o;
};

static void ui_screen_reader_destroy(void *screen) {
  puts(__func__);  
  ui_screen_reader_t reader_screen = screen;
  ui_wx_reader_destroy(reader_screen->reader);
  mem_free(screen);
}
