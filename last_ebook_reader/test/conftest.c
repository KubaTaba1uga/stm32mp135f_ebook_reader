#include "display/display.h"
#include "display/display_internal.h"
#include "gui/gui.h"
#include "gui/gui_internal.h"
#include "utils/mem.h"

bool enable_ebk_display_init_mock = false;
int ebk_display_init_mock_called = 0;
ebk_error_t __real_ebk_display_init(ebk_display_t *, enum dd_DisplayDriverEnum);
ebk_error_t __wrap_ebk_display_init(ebk_display_t *display,
                                    enum dd_DisplayDriverEnum model) {
  if (!enable_ebk_display_init_mock) {
    return __real_ebk_display_init(display, model);
  }

  ebk_display_init_mock_called++;
  printf("%s mocked\n", __func__);

  *display = ebk_mem_malloc(sizeof(struct ebk_Display));
  **display = (struct ebk_Display){0};

  return 0;
}

bool enable_ebk_display_show_boot_img_mock = false;
int ebk_display_show_boot_img_mock_called = 0;
ebk_error_t __real_ebk_display_show_boot_img(ebk_display_t);
ebk_error_t __wrap_ebk_display_show_boot_img(ebk_display_t disp) {
  if (!enable_ebk_display_show_boot_img_mock) {
    return __real_ebk_display_show_boot_img(disp);
  }

  ebk_display_show_boot_img_mock_called++;
  printf("%s mocked\n", __func__);

  return 0;
};

bool enable_ebk_display_destroy_mock = false;
int ebk_display_destroy_mock_called = 0;
void __real_ebk_display_destroy(ebk_display_t *);
void __wrap_ebk_display_destroy(ebk_display_t *disp) {
  if (!enable_ebk_display_destroy_mock) {
    return __real_ebk_display_destroy(disp);
  };

  printf("%s mocked\n", __func__);

  ebk_mem_free(*disp);
};

bool enable_ebk_gui_init_mock = false;
int ebk_gui_init_mock_called = 0;
ebk_error_t __real_ebk_gui_init(ebk_gui_t *out);
ebk_error_t __wrap_ebk_gui_init(ebk_gui_t *out) {
  if (!enable_ebk_gui_init_mock) {
    return __real_ebk_gui_init(out);
  }

  ebk_gui_init_mock_called++;
  printf("%s mocked\n", __func__);

  *out = ebk_mem_malloc(sizeof(struct ebk_Gui));
  **out = (struct ebk_Gui){0};

  return 0;
}

bool enable_ebk_gui_tick_mock = false;
int ebk_gui_tick_mock_called = 0;
int ebk_gui_tick_mock_return_value = 1000;
int __real_ebk_gui_tick(ebk_gui_t gui);
int __wrap_ebk_gui_tick(ebk_gui_t gui) {
  if (!enable_ebk_gui_tick_mock) {
    return __real_ebk_gui_tick(gui);
  }

  ebk_gui_tick_mock_called++;
  printf("%s mocked\n", __func__);

  return ebk_gui_tick_mock_return_value;
}

bool enable_ebk_gui_destroy_mock = false;
int ebk_gui_destroy_mock_called = 0;
void __real_ebk_gui_destroy(ebk_gui_t *out);
void __wrap_ebk_gui_destroy(ebk_gui_t *out) {
  if (!enable_ebk_gui_destroy_mock) {
    return __real_ebk_gui_destroy(out);
  }

  ebk_gui_destroy_mock_called++;
  printf("%s mocked\n", __func__);

  ebk_mem_free(*out);
}
