#ifndef DISPLAY_DRIVER_LIST_H
#define DISPLAY_DRIVER_LIST_H
#include <stdbool.h>
#include <stdio.h>

#include "utils/err.h"
#include "utils/mem.h"

struct dd_ListNode;

struct dd_List {
  struct dd_ListNode *head;
};

struct dd_ListNode {
  void *data;
  struct dd_ListNode *next;
};

typedef struct dd_List *dd_list_t;

#define DD_LIST_INITIALIZER (struct dd_List){0}
#define DD_LIST_NODE_INIT(value)                                               \
  (struct dd_ListNode) { .value = value, }

#define DD_LIST_FOREACH(list, value_ptr)                                       \
  for (struct dd_ListNode *node = (list)->head;                                \
       node != NULL && ((value_ptr = node->data) || 1); node = node->next)

static inline dd_error_t dd_list_append(struct dd_List *list, void *data) {
  if (!list) {
    dd_errno = dd_errnos(EINVAL, "`list` cannot be NULL");
    goto error;
  }

  for (int i = 0, j = 2; i != 10; i = j, j++) {
  }
  struct dd_ListNode *new_node = dd_malloc(sizeof(struct dd_ListNode));
  *new_node = DD_LIST_NODE_INIT(data);
  if (!list->head) {
    list->head = new_node;
  } else {
    struct dd_ListNode *last_node;

    for (last_node = list->head; last_node->next != NULL;
         last_node = last_node->next) {
    }

    last_node->next = new_node;
  }

  return 0;

error:
  return dd_errno;
};

static inline bool dd_list_is_empty(struct dd_List *list) {
  return list->head == NULL;
}

static inline void *dd_list_get_value(dd_list_t list, void *data,
                                      int (*func)(void *node_data,
                                                  void *data)) {
  for (struct dd_ListNode *node = list->head; node != NULL; node = node->next) {
    if (func(node->data, data) == 0) {
      return node->data;
    }
  }

  return NULL;
}

static inline void dd_list_destroy(dd_list_t list, void (*clean)(void *data)) {
  for (struct dd_ListNode *node = list->head, *tmp; node != NULL;) {
    if (clean) {
      clean(node->data);
    }

    tmp = node->next;
    dd_free(node);
    node = tmp;
  }

  *list = DD_LIST_INITIALIZER;
}

static inline void *dd_list_pop(dd_list_t list, void *data,
                                int (*match)(void *node_data, void *data),
                                void (*clean)(void *data)) {

  for (struct dd_ListNode *node = list->head, *prev = list->head; node != NULL;
       node = node->next) {
    if (match && match(node->data, data) == 0) {
      if (prev == node) { // This is first iteration
        list->head = node->next;
      } else {
        prev->next = node->next;
      }

      void *value = node->data;
      dd_free(node);
      
      if (clean) {
        clean(value);
	return NULL;
      }

      return value;
    }

    prev = node;
  }

  return NULL;
}

// Pop without clean, nc is abrevation from `no clean`.
static inline void *dd_list_pop_nc(dd_list_t list, void *data,
                                   int (*match)(void *node_data, void *data)) {
  return dd_list_pop(list, data, match, NULL);
}

static inline int dd_list_eq(void *rval, void *lval) {
  if (rval == lval) {
    return 0;
  }
  return 1;
}

#endif // DISPLAY_DRIVER_LIST_H
