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
#include <string.h>

typedef struct UiScreenReader *ui_screen_reader_t;

struct UiScreenReader {
  void (*event_cb)(lv_event_t *e, void *book, ui_t ui);
  ui_wx_reader_settings_field_t *fields;
  ui_wx_reader_set_hor_num_t hor_num;
  ui_wx_reader_set_ver_num_t ver_num;
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
  lv_obj_add_event_cb(screen->reader, ui_screen_reader_book_event_cb, event,
                      screen);

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
  ui_screen_reader_t reader = screen->screen_data;
  reader->settings = settings;
  assert(reader != NULL);
  assert(reader->reader != NULL);

  ui_screen_reader_cleanup(reader);

  uint32_t n = lv_group_get_obj_count(group);
  printf("group members: %u\n", (unsigned)n);

  lv_group_add_obj(group, settings);

  reader->fields =
      mem_malloc(sizeof(ui_wx_reader_settings_field_t) * fields_len);
  reader->fields_len = fields_len;
  
  for (int i = 0; i < reader->fields_len; i++) {
    ui_wx_reader_settings_field_t f =
        ui_wx_reader_settings_add_field(settings, fields[i], i, reader);
    lv_obj_add_event_cb(f, ui_screen_reader_settings_event_cb,
                        reader->event, f);
    reader->fields[i] = f;
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
  for (int i = 0; i < reader->fields_len; i++) {
    ui_wx_reader_settings_field_destroy(reader->fields[i]);
  }
  ui_wx_reader_settings_destroy(reader->settings);  
  mem_free(reader->fields);
}

static void ui_screen_reader_fields_event_cb(lv_event_t *e) {
  puts(__func__);
  ui_screen_reader_t reader = lv_event_get_user_data(e);
  reader->event_cb(e, reader, reader->owner);
}

err_t ui_screen_reader_set_scale_init(ui_screen_t screen, book_t book,
                                      lv_group_t *group) {
  puts(__func__);
  double scale_value = book_get_scale(book);
  ui_wx_reader_set_hor_num_t scale =
      ui_wx_reader_set_hor_num_create(scale_value);
  ui_screen_reader_t reader_screen = screen->screen_data;
  reader_screen->hor_num = scale;

  lv_obj_t *key_catcher = lv_obj_create(scale);
  lv_obj_set_size(key_catcher, 1, 1);
  lv_obj_add_flag(key_catcher, LV_OBJ_FLAG_HIDDEN);
  lv_group_add_obj(group, key_catcher);
  lv_group_focus_obj(key_catcher);
  lv_group_focus_obj(key_catcher);
  lv_obj_set_user_data(scale, key_catcher);
  lv_obj_add_event_cb(key_catcher, ui_screen_reader_fields_event_cb,
                      LV_EVENT_KEY, reader_screen);

  return 0;
}

void ui_screen_reader_set_scale_destroy(ui_screen_t screen) {
  puts(__func__);
  ui_screen_reader_t reader_screen = screen->screen_data;
  lv_obj_t *scale = lv_obj_get_user_data(reader_screen->hor_num);
  lv_obj_del(scale);
  ui_wx_reader_set_hor_num_destroy(reader_screen->hor_num);
}

err_t ui_screen_reader_set_x_off_init(ui_screen_t screen, book_t book,
                                      lv_group_t *group) {
  puts(__func__);
  double x_off_value = book_get_x_off(book);
  ui_wx_reader_set_ver_num_t x_off =
      ui_wx_reader_set_ver_num_create(x_off_value);
  ui_screen_reader_t reader_screen = screen->screen_data;
  reader_screen->ver_num = x_off;

  lv_obj_t *key_catcher = lv_obj_create(x_off);
  lv_obj_set_size(key_catcher, 1, 1);
  lv_obj_add_flag(key_catcher, LV_OBJ_FLAG_HIDDEN);
  lv_group_add_obj(group, key_catcher);
  lv_group_focus_obj(key_catcher);
  lv_obj_set_user_data(x_off, key_catcher);
  lv_obj_add_event_cb(key_catcher, ui_screen_reader_fields_event_cb,
                      LV_EVENT_KEY, reader_screen);

  return 0;
}

void ui_screen_reader_set_x_off_destroy(ui_screen_t screen) {
  puts(__func__);
  ui_screen_reader_t reader_screen = screen->screen_data;
  lv_obj_t *x_off = lv_obj_get_user_data(reader_screen->ver_num);
  lv_obj_del(x_off);
  ui_wx_reader_set_ver_num_destroy(reader_screen->ver_num);
}

err_t ui_screen_reader_set_y_off_init(ui_screen_t screen, book_t book,
                                      lv_group_t *group) {
  puts(__func__);
  double y_off_value = book_get_y_off(book);
  ui_wx_reader_set_hor_num_t y_off =
      ui_wx_reader_set_hor_num_create(y_off_value);
  ui_screen_reader_t reader_screen = screen->screen_data;
  reader_screen->hor_num = y_off;

  lv_obj_t *key_catcher = lv_obj_create(y_off);
  lv_obj_set_size(key_catcher, 1, 1);
  lv_obj_add_flag(key_catcher, LV_OBJ_FLAG_HIDDEN);
  lv_group_add_obj(group, key_catcher);
  lv_group_focus_obj(key_catcher);
  lv_group_focus_obj(key_catcher);
  lv_obj_set_user_data(y_off, key_catcher);
  lv_obj_add_event_cb(key_catcher, ui_screen_reader_fields_event_cb,
                      LV_EVENT_KEY, reader_screen);

  return 0;
}

void ui_screen_reader_set_y_off_destroy(ui_screen_t screen) {
  puts(__func__);
  ui_screen_reader_t reader_screen = screen->screen_data;
  lv_obj_t *y_off = lv_obj_get_user_data(reader_screen->hor_num);
  lv_obj_del(y_off);
  ui_wx_reader_set_hor_num_destroy(reader_screen->hor_num);
}
