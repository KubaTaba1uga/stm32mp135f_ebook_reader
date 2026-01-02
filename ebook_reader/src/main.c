#include <lvgl.h>
#include <stdio.h>
bool terminated = false;
static void on_close_cb(lv_event_t *e) {
  puts("Exiting");

  terminated = true;
}
int main(void) {
  puts("Hello world!");

  /* initialize X11 display driver */
  lv_display_t *disp = lv_x11_window_create("LVGL X11 Simulation", 480, 800);
  lv_display_add_event_cb(disp, on_close_cb, LV_EVENT_DELETE, disp);

  /* initialize X11 input drivers (for keyboard, mouse & mousewheel) */
  LV_IMAGE_DECLARE(my_mouse_cursor_icon);
  lv_x11_inputs_create(disp, &my_mouse_cursor_icon);

#if !LV_X11_DIRECT_EXIT
  /* set optional window close callback to enable application cleanup and exit
   */
  lv_x11_window_set_close_cb(disp, on_close_cb, disp);
#endif

  while (!terminated) {
    /* Periodically call the lv_timer handler */
    lv_timer_handler();
    lv_sleep_ms(100);
  }

  return 0;
}
