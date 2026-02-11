#ifndef EBOOK_READER_EVENT_BUS_CORE_H
#define EBOOK_READER_EVENT_BUS_CORE_H

#include "event_bus/event_bus.h"
#include "utils/zlist.h"

struct EventQueue {
  struct ZList ev_queue;
};

struct EventQueueNode {
  enum BusEnum bus;
  struct Event event;
  struct ZListNode znode;
};

void event_queue_init(struct EventQueue *out);
void event_queue_destroy(struct EventQueue *out);
void event_queue_push(struct EventQueue *queue, struct EventQueueNode *node);
struct EventQueueNode *event_queue_pull(struct EventQueue *queue);

#endif // EBOOK_READER_EVENT_BUS_CORE_H
