/**
  Settings are specified at compilation time. Each setting has corresponding
  definition, if definition is not set default value is used.

  Some settings depend on each other, for example boot screen image depends
  on the display model, because different displays support different resolutions
  and colors so we need different pictures to make every possible configuration
  look beautiful. If you need such dependent setting use preprocessor directives
  to set appropriate value.

  DISPLAY_MODEL display model used with a device instance.
  DISPLAY_BOOT_SCREEN_PATH path to image displayed during boot.
 */

#include "settings.h"

#if !defined(EBK_DISPLAY_WVS7IN5V2B) && !defined(EBK_DISPLAY_X11) &&           \
    !defined(EBK_DISPLAY_WVS7IN5V2)
#define EBK_DISPLAY_WVS7IN5V2B 1
#endif

#if EBK_DISPLAY_WVS7IN5V2B
#define EBK_DISPLAY_MODEL DisplayModelEnum_WVS7IN5V2B
#define EBK_DISPLAY_BOOT_SCREEN_PATH "data/480x800_img_boot_screen_adjusted"
#elif EBK_DISPLAY_WVS7IN5V2
#define EBK_DISPLAY_MODEL DisplayModelEnum_WVS7IN5V2
#define EBK_DISPLAY_BOOT_SCREEN_PATH "data/480x800_img_boot_screen_adjusted"
#elif EBK_DISPLAY_X11
#define EBK_DISPLAY_MODEL DisplayModelEnum_X11
#define EBK_DISPLAY_BOOT_SCREEN_PATH                                           \
  "/home/taba1uga/Github/stm32mp135f_ebook_reader/ebook_reader/data/"          \
  "480x800_img_boot_screen_normal"
#else
#error "Unsupported display model"
#endif

const enum DisplayModelEnum settings_display_model = EBK_DISPLAY_MODEL;
const char *settings_boot_screen_path = EBK_DISPLAY_BOOT_SCREEN_PATH;
const char *settings_books_dir = "/mnt/sdcard";
const char *settings_input_path = "/dev/input/event0";
