/**
  Settings are specified at compilation time. Each setting has corresponding
  definition, if definition is not set default value is used.

  Some settings depend on each other, for example boot screen image depends
  on the display model, because different displays support different resolutions
  and colors so we need different pictures to make every possible configuration
  look beautiful. If you need such dependent setting use preprocessor directives
  to set appropriate value.

  EBK_DISPLAY_MODEL display model used with a device instance.
 */

#include <display_driver.h>

#ifndef EBK_DISPLAY_MODEL
#define EBK_DISPLAY_MODEL dd_DisplayDriverEnum_Wvs7in5V2b
#endif

#if EBK_DISPLAY_MODEL == dd_DisplayDriverEnum_Wvs7in5V2b
#define EBK_DISPLAY_BOOT_SCREEN_PATH "data/480x800_1bit_boot_screen"
#else
#error "Unsupported display model"
#endif

const enum dd_DisplayDriverEnum ebk_settings_display_model = EBK_DISPLAY_MODEL;
const char *ebk_settings_display_boot_screen_path =
    EBK_DISPLAY_BOOT_SCREEN_PATH;
