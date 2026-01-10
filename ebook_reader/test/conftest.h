#include <stdbool.h>

extern bool enable_ebk_display_init_mock;
extern int ebk_display_init_mock_called;

extern bool enable_ebk_display_show_boot_img_mock;
extern int ebk_display_show_boot_img_mock_called;

extern bool enable_ebk_display_destroy_mock;
extern int ebk_display_destroy_mock_called;

extern bool enable_ebk_gui_init_mock;
extern int ebk_gui_init_mock_called;

extern bool enable_ebk_gui_tick_mock;
extern int ebk_gui_tick_mock_called;
extern int ebk_gui_tick_mock_return_value;

extern bool enable_ebk_gui_destroy_mock;
extern int ebk_gui_destroy_mock_called;
