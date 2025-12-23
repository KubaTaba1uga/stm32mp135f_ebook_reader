#ifndef DISPLAY_DRIVER_LIST_H
#define DISPLAY_DRIVER_LIST_H
#include <stdbool.h>

#include "utils/err.h"
#include "utils/mem.h"

struct dd_List {
  void *data;
  struct dd_List *next;
};
typedef struct dd_List *dd_list_t;

#define DD_LIST_INITIALIZER (struct dd_List){0}
#define DD_LIST_INIT(value)                                                    \
  (struct dd_List) { .data = data }

#define DD_LIST_GET_TAIL(list, last_node)                                      \
  for (last_node = list; last_node->next != NULL;                              \
       last_node = last_node->next) {                                          \
  }

static inline dd_error_t dd_list_append(struct dd_List *list, void *data) {
  if (!list) {
    dd_errno = dd_errnos(EINVAL, "`list` cannot be NULL");
    goto error;
  }

  struct dd_List *last_node;
  DD_LIST_GET_TAIL(list, last_node);

  last_node->next = dd_malloc(sizeof(struct dd_List));
  *last_node->next = DD_LIST_INIT(data);

error:
  return dd_errno;
};

static inline bool dd_list_is_empty(struct dd_List *list) {
  return list->next == NULL && list->data == NULL;
}

static inline void *dd_list_get_value(dd_list_t list, void *data,
                                      int (*func)(void *node_data,
                                                  void *data)) {
  if (!dd_list_is_empty(list)) {
    for (struct dd_List *node = list; node != NULL; node = node->next) {
      if (func(node->data, data) == 0) {
        return node->data;
      }
    }
  }

  return NULL;
}

static inline void dd_list_destroy(dd_list_t list, void (*func)(void *data)) {
  if (!dd_list_is_empty(list)) {
    for (struct dd_List *node = list, *tmp; node != NULL; node = node->next) {
      func(node->data);
      tmp = node->next;
      if (node != list) {
        dd_free(node);
      }
      node = tmp;
    }
    *list = DD_LIST_INITIALIZER;
  }
}

#endif // DISPLAY_DRIVER_LIST_H
