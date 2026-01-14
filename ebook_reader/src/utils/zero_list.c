#include "utils/zero_list.h"
#include "utils/error.h"

int ebk_zlist_append(ebk_zlist_t head, ebk_zlist_node_t node) {
  if (!head || !node) {
    ebk_errno = ebk_errnos(EINVAL, "`head` and `node` cannot be NULL");
    return -1;
  }

  ebk_zlist_node_t *p = &head->head;
  while (*p) {
    p = &(*p)->next;
  }

  head->len++;
  node->next = NULL;
  *p = node;
  return 0;
}

/* int ebk_zlist_append(ebk_zlist_t head, ebk_zlist_node_t node) { */
/*   if (!head || !node) { */
/*     ebk_errno = ebk_errnos(EINVAL, "`head` and `node` cannot be NULL"); */
/*     goto error_out; */
/*   } */

/*   ebk_zlist_node_t *last_node = &head->head; */
/*   while (*last_node && (*last_node)->next) { */
/*     *last_node = (*last_node)->next; */
/*   } */

/*   *last_node = node; */

/*   return 0; */

/* error_out: */
/*   return -1; */
/* } */
