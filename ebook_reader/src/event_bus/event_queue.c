#include <assert.h>

#include "event_bus/core.h"
#include "utils/mem.h"
#include "utils/zlist.h"

void event_queue_init(struct EventQueue *out) { *out = (struct EventQueue){0}; }

void event_queue_destroy(struct EventQueue *out) {
  assert(out->ev_queue.len == 0);
}

void event_queue_push(struct EventQueue *queue, struct EventQueueNode *node) {
  zlist_append(&queue->ev_queue, &node->znode);
};

struct EventQueueNode *event_queue_pull(struct EventQueue *queue) {
  zlist_node_t znode = zlist_pop(&queue->ev_queue, 0);
  if (!znode) {
    return NULL;
  }

  return mem_container_of(znode, struct EventQueueNode, znode);
}
