#ifndef SETTINGS_H
#define SETTINGS_H

enum DisplayModelEnum {
  DisplayModelEnum_X11 = 0,
  DisplayModelEnum_WVS7IN5V2B,
  DisplayModelEnum_WVS7IN5V2,
  DisplayModelEnum_MAX,
};

extern const enum DisplayModelEnum settings_display_model;
extern const char *settings_boot_screen_path;
extern const char *settings_input_path;
extern const char *settings_books_dir;


#endif // SETTINGS_H
