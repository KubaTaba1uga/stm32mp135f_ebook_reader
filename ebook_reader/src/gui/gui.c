#include "gui/gui.h"
#include <lvgl.h>
#include <stdio.h>

#include "gui/gui_internal.h"
#include "utils/error.h"
#include "utils/mem.h"
#include "utils/time.h"
#include "utils/lvgl.h"

ebk_error_t ebk_gui_init(
    ebk_gui_t *out,
void (*input_callback)(enum ebk_GuiInputEventEnum event, void *data),
    void *input_data) {
  puts(__func__);

  ebk_lvgl_init();
  lv_tick_set_cb(ebk_time_now);

  ebk_gui_t gui = *out = ebk_mem_malloc(sizeof(struct ebk_Gui));
  *gui = (struct ebk_Gui){
      .inputh =
          {
              .callback = input_callback,
              .data = input_data,
          },
  };

  return 0;
}

int ebk_gui_tick(ebk_gui_t gui) {
  puts(__func__);
  return lv_timer_handler();
}

void ebk_gui_destroy(ebk_gui_t *out) {
  puts(__func__);
  if (!out || !*out) {
    return;
  }

  ebk_lvgl_deinit();  
  ebk_mem_free(*out);
  *out = NULL;
}

ebk_error_t ebk_gui_show_menu(ebk_gui_t gui, ebk_books_list_t books,
                              uint32_t width, uint32_t height) {

  /**

     Menu area is splitted like this:

     Heigth: 800

      Bar:
        5   px border
        90  px bar
        5   px border

      Bookshelf:
        50  px space
        100 px photo
        50  px space
        100 px photo
        50  px space
        100 px photo
        50  px space
        100 px photo
        50  px space
        50  px of space


     Width: 480

      Bar:
        5   px border
        120 px battery
        35  px space
        140 px hour
        35  px space
        140 px date
        5   px border

      Bookshelf:
        40 px space
        80 px thumbnail
        25 px space
        80 px thumbnail
        25 px space
        80 px thumbnail
        25 px space
        80 px thumbnail
        40 px space

*/
  static uint32_t btn_cnt = 1;
  static lv_obj_t * main_page;
  static lv_obj_t *menu;

  menu = lv_menu_create(lv_screen_active());
  lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
  lv_obj_center(menu);  
};
