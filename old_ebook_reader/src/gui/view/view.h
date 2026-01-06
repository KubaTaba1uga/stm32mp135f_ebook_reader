#ifndef EBOOK_READER_VIEW_H
#define EBOOK_READER_VIEW_H

#include "utils/error.h"
#include "gui/lv/lv.h"

struct ViewOps {
  cdk_error_t (*init)(void *data);
  cdk_error_t (*callback)(enum LvglEvent event, void *data);
  void (*destroy)(void *data);
};

struct GenericView {
  void *data;
  struct ViewOps ops;
};

/**
  View provide generic subsystem for all views.
*/
struct View {
  struct GenericView current_view;
};

typedef struct View *view_t;

cdk_error_t view_init(view_t);
void view_destroy(view_t);

#endif // EBOOK_READER_VIEW_H
