#ifndef EBOOK_READER_READER_SCREEN_CORE_H
#define EBOOK_READER_READER_SCREEN_CORE_H
#include <lvgl.h>
#include <stdint.h>

typedef lv_obj_t *wx_reader_t;
wx_reader_t wx_reader_create(int page_len, const unsigned char *page_buf);
void wx_reader_destroy(wx_reader_t reader);

#endif // EBOOK_READER_READER_SCREEN_CORE_H
