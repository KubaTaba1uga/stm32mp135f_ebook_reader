#include "event_queue/event_queue.h"
#include "utils/mem.h"
#include "utils/zlist.h"

typedef struct Event *event_t;
typedef struct EventQueue *event_queue_t;

struct Event {
  ref_t data;
  enum Events event;
  struct ZListNode next;
};

struct Subscriber {
  post_event_func_t func;
  void *data;
};

struct EventQueue {
  struct ZList queue;
  struct Subscriber subscribers[EventSubscribers_MAX];
};

enum EventSubscribers route_table[Events_MAX][EventSubscribers_MAX] = {
    [Events_BOOT_DONE] =
        {
            EventSubscribers_MENU,
        },
    [Events_BOOK_OPENED] =
        {
            EventSubscribers_MENU,
            EventSubscribers_READER,
        },
    [Events_BTN_NEXT_PAGE_CLICKED] =
        {
            EventSubscribers_READER,
        },
    [Events_BTN_PREV_PAGE_CLICKED] =
        {
            EventSubscribers_READER,
        },
    [Events_BOOK_UPDATED] =
        {
            EventSubscribers_READER,
        },
    [Events_BTN_MENU_CLICKED] = {
        EventSubscribers_READER,
            EventSubscribers_MENU,        
        
        },
};

static void event_bus_route_event(event_queue_t queue, event_t event);
static event_t event_queue_pull(event_queue_t queue);

void event_queue_init(event_queue_t *out) {
  event_queue_t queue = *out = mem_malloc(sizeof(struct EventQueue));
  *queue = (struct EventQueue){0};
};

void event_queue_destroy(event_queue_t *out) {
  if (mem_is_null_ptr(out)) {
    return;
  }
  event_queue_t queue = *out;
  event_t event;
  while ((event = event_queue_pull(queue)) != NULL) {
    mem_deref(event->data);
    mem_free(event);
  }
  mem_free(queue);
  *out = NULL;
}

void event_queue_push(event_queue_t queue, enum Events event,
                      ref_t event_data) {
  event_t ev = mem_malloc(sizeof(struct Event));
  *ev = (struct Event){.data = event_data == NULL ? NULL : mem_ref(event_data),
                       .event = event};

  zlist_append(&queue->queue, &ev->next);
}

void event_queue_step(event_queue_t queue) {
  event_t event;
  while ((event = event_queue_pull(queue)) != NULL) {
    /* log_debug("Handling: (%s)->(%s)", bus_dump(node->bus), */
    /*           event_dump(node->event.event)); */

    event_bus_route_event(queue, event);

    mem_deref(event->data);
    mem_free(event);
  }
}

static void event_bus_route_event(event_queue_t queue, event_t event) {
  const struct Subscriber *sub;
  int i = 0;
  for (enum EventSubscribers dst = route_table[event->event][i];
       dst != EventSubscribers_NONE; dst = route_table[event->event][++i]) {
    sub = &queue->subscribers[dst];
    if (!sub || !sub->func) {
      continue;
    }

    sub->func(event->event, event->data, sub->data);
  }
}

static event_t event_queue_pull(event_queue_t queue) {
  zlist_node_t node = zlist_pop(&queue->queue, 0);
  if (!node) {
    return NULL;
  }

  return mem_container_of(node, struct Event, next);
};

void event_queue_register(event_queue_t queue, enum EventSubscribers subscriber,
                          post_event_func_t subscriber_func,
                          void *subscriber_data) {
  queue->subscribers[subscriber] = (struct Subscriber){
      .func = subscriber_func,
      .data = subscriber_data,
  };
}

void event_queue_deregister(event_queue_t queue,
                            enum EventSubscribers subscriber) {
  queue->subscribers[subscriber] = (struct Subscriber){0};
}
