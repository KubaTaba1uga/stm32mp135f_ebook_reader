#ifndef EBOOK_READER_SETTINGS_H
#define EBOOK_READER_SETTINGS_H

#include <display_driver.h>

enum ebk_DisplayModelEnum {
  ebk_DisplayModelEnum_Wvs7in5V2b,
  ebk_DisplayModelEnum_X11,
};

extern const enum ebk_DisplayModelEnum ebk_settings_display_model;
extern const char *ebk_settings_books_dir;

#endif // EBOOK_READER_SETTINGS_H
