#ifndef ZLIST_H
#define ZLIST_H

#include <stdint.h>
#include <stdlib.h>

/**
   Idea of zero list is that, we can create a list with zero memore allocations
   in meantime, just embed zlist node into required struct and point to another
   embedded zlist node struct in runtime.

   @note To receive node's payload use container_of.
*/

typedef struct ZList *zlist_t;
typedef struct ZListNode *zlist_node_t;

struct ZList {
  uint32_t len;
  zlist_node_t head;
};

struct ZListNode {
  zlist_node_t next;
};

/**
   @brief Append node to zero list.
   @param head Head of the zero list.
   @param node Node that will be appended.
   @return On success index of appended node, on error return -1 and set
   ebk_errno.
*/
int zlist_append(zlist_t head, zlist_node_t node);

/**
   @brief Get node from zero list.
   @param head Head of the zero list.
   @param idx Index of node to get.
   @return On success pointer to node, on error NULL.
*/
zlist_node_t zlist_get(zlist_t head, int idx);
zlist_node_t zlist_pop(zlist_t list, int idx);
#endif // ZLIST_H
