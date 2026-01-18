#include "zlist.h"
#include "err.h"

int zlist_append(zlist_t head, zlist_node_t node) {
  if (!head || !node) {
    err_o = err_errnos(EINVAL, "`head` and `node` cannot be NULL");
    return -1;
  }

  zlist_node_t *p = &head->head;
  while (*p) {
    p = &(*p)->next;
  }

  head->len++;
  node->next = NULL;
  *p = node;
  return 0;
}

zlist_node_t zlist_get(zlist_t list, int idx) {
  int i = 0;
  for (zlist_node_t node = list->head; node != NULL; node = node->next) {
    if (idx == i++) {
      return node;
    }
  }

  return NULL;
};
