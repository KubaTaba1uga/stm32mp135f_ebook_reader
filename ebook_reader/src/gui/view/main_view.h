#ifndef EBOOK_READER_VIEW_MAIN_H
#define EBOOK_READER_VIEW_MAIN_H

#include "utils/error.h"

struct ViewMain {
  
};

typedef struct ViewMain *view_main_t;

cdk_error_t view_main_init(view_main_t);
void view_main_destroy(view_main_t);

#endif // EBOOK_READER_VIEW_MAIN_H
