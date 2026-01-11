#ifndef EBOOK_READER_ZLIST_H
#define EBOOK_READER_ZLIST_H

#include <stdint.h>
#include <stdlib.h>

/**
   Idea of zero list is that, we can create a list with zero memore allocations
   in meantime, just embed zlist node into required struct and point to another
   embedded zlist node struct in runtime.

   @note To receive node's payload use offsetof.
*/

struct ebk_ZListNode {
  struct ebk_ZListNode *next;
};

struct ebk_ZList {
  uint32_t len;
  struct ebk_ZListNode *head;
};

typedef struct ebk_ZListNode *ebk_zlist_node_t;
typedef struct ebk_ZList *ebk_zlist_t;

/**
   @brief Append node to zero list.
   @param head Head of the zero list.
   @param node Node that will be appended.
   @return On success index of appended node, on error return -1 and set ebk_errno.
*/
int ebk_zlist_append(ebk_zlist_t head, ebk_zlist_node_t node);

/**
   @brief Get node from zero list.
   @param head Head of the zero list.
   @param idx Index of node to get.
   @return On success pointer to node, on error NULL and set ebk_errno.
*/
ebk_zlist_node_t ebk_zlist_get(ebk_zlist_t head, int idx);


#endif // EBOOK_READER_ZLIST_H
