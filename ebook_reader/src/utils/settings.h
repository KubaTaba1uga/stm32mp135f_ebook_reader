#ifndef SETTINGS_H
#define SETTINGS_H

enum DisplayModelEnum {
  DisplayModelEnum_Wvs7in5V2b,
  DisplayModelEnum_X11,
};

extern const enum DisplayModelEnum settings_display_model;
extern const char *settings_boot_screen_path;
extern const char *settings_books_dir;

#endif // SETTINGS_H
