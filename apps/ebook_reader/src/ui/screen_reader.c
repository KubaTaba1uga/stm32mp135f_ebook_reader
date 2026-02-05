#include "book/book.h"
#include "core/lv_group.h"
#include "core/lv_obj.h"
#include "display/lv_display.h"
#include "misc/lv_types.h"
#include "ui/screen.h"
#include "ui/widgets.h"
#include "utils/err.h"
#include "utils/mem.h"
#include <stdio.h>

typedef struct UiScreenReader *ui_screen_reader_t;

struct UiScreenReader {
  void (*event_cb)(lv_event_t *e, void *book, ui_t ui);
  ui_wx_reader_settings_field_t *fields;
  ui_wx_reader_settings_t settings;
  ui_wx_reader_t reader;
  lv_group_t *group;
  int fields_len;
  ui_t owner;
  int event;
};

static void ui_screen_reader_destroy(void *);
static void ui_screen_reader_book_event_cb(lv_event_t *e);
static void ui_screen_reader_cleanup(ui_screen_reader_t reader);

err_t ui_screen_reader_init(ui_screen_t out, ui_t ui, book_t book, int event,
                            void (*event_cb)(lv_event_t *e, void *book,
                                             ui_t ui),
                            lv_group_t *group) {
  puts(__func__);
  assert(event_cb != NULL);
  assert(group != NULL);
  assert(book != NULL);
  assert(out != NULL);
  assert(ui != NULL);
  ui_screen_reader_t screen;
  
  if (out->screen_data) {
    screen = out->screen_data;
    goto out;
    }
  
  int page_size = 0;
  const unsigned char *page_data =
      book_get_page(book, lv_display_get_horizontal_resolution(NULL),
                    lv_display_get_vertical_resolution(NULL), &page_size);
  if (!page_data) {
    ERR_TRY(err_o);
  }

  assert(page_size != 0);

  ui_wx_reader_t reader = ui_wx_reader_create(page_size, page_data);
  if (!reader) {
    err_o = err_errnos(EINVAL, "Cannot create reader widget");
    goto error_out;
  }

 screen = mem_malloc(sizeof(struct UiScreenReader));
  *screen = (struct UiScreenReader){
      .event_cb = event_cb,
      .reader = reader,
      .group = group,
      .event = event,
      .owner = ui,
  };

  *out = (struct UiScreen){
      .destroy = ui_screen_reader_destroy,
      .screen_data = screen,
  };

  lv_obj_set_user_data(reader, book);
  
 out:    
  lv_group_add_obj(group, screen->reader);
  lv_obj_add_event_cb(screen->reader, ui_screen_reader_book_event_cb, event, screen);

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

static void ui_screen_reader_book_event_cb(lv_event_t *e) {
  ui_screen_reader_t reader = lv_event_get_user_data(e);
  ui_wx_reader_t widget = reader->reader;
  book_t book = lv_obj_get_user_data(widget);

  reader->event_cb(e, book, reader->owner);
}

static void ui_screen_reader_settings_event_cb(lv_event_t *e) {
  puts(__func__);
  ui_wx_reader_settings_field_t f = lv_event_get_user_data(e);
  ui_screen_reader_t reader = ui_wx_reader_settings_field_get_data(f);
  reader->event_cb(e, ui_wx_reader_settings_field_get_id(f), reader->owner);
}

err_t ui_screen_reader_settings_init(ui_screen_t screen, const char **fields,
                                     int fields_len, lv_group_t *group) {
  puts(__func__);
  ui_wx_reader_settings_t settings = ui_wx_reader_settings_create();
  ui_screen_reader_t reader_screen = screen->screen_data;
  reader_screen->settings = settings;

  ui_screen_reader_cleanup(reader_screen);

  uint32_t n = lv_group_get_obj_count(group);
  printf("group members: %u\n", (unsigned)n);

  lv_group_add_obj(group, settings);

  reader_screen->fields =
      mem_malloc(sizeof(ui_wx_reader_settings_field_t) * fields_len);
  for (int i = 0; i < fields_len; i++) {
    ui_wx_reader_settings_field_t f =
        ui_wx_reader_settings_add_field(settings, fields[i], i, reader_screen);
    lv_obj_add_event_cb(f, ui_screen_reader_settings_event_cb,
                        reader_screen->event, f);
    reader_screen->fields[i] = f;
  }

  return 0;
}

static void ui_screen_reader_cleanup(ui_screen_reader_t reader) {
  puts(__func__);
  lv_obj_remove_event_cb_with_user_data(reader->reader,
                                        ui_screen_reader_book_event_cb, reader);

  lv_group_remove_obj(reader->reader);
}

void ui_screen_reader_settings_destroy(ui_screen_t screen) {
  ui_screen_reader_t reader = screen->screen_data;
  ui_wx_reader_settings_destroy(reader->settings);
  for (int i = 0; i < reader->fields_len; i++) {
    ui_wx_reader_settings_field_destroy(reader->fields[i]);
  }
  mem_free(reader->fields)  ;
}
