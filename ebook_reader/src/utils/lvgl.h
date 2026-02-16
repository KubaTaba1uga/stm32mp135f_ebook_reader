#ifndef EBOOK_READER_UTILS_LVGL_H
#define EBOOK_READER_UTILS_LVGL_H

typedef struct _lv_obj_t *lvgl_obj_t;
typedef struct _lv_event_t *lvgl_event_t;

lvgl_obj_t lvgl_obj_create(lvgl_obj_t parent);
lvgl_obj_t lvgl_img_create(lvgl_obj_t parent);

#endif // EBOOK_READER_UTILS_LVGL_H
