/**
  Settings are specified at compilation time. Each setting has corresponding
  definition, if definition is not set default value is used.

  EBK_DISPLAY_MODEL display model used with a device instance.
  EBK_BOOT_SCREEN_PATH path to a picture displayed during boot.
 */

#include <display_driver.h>

#ifndef EBK_DISPLAY_MODEL
#define EBK_DISPLAY_MODEL dd_DisplayDriverEnum_Wvs7in5V2b
#endif

#ifndef EBK_BOOT_SCREEN_PATH
#define EBK_BOOT_SCREEN_PATH "data/480x800_1bit_boot_screen"
#endif

const enum dd_DisplayDriverEnum ebk_settings_display_model = EBK_DISPLAY_MODEL;
const char *ebk_settings_display_boot_screen_path = "/opt/data";
