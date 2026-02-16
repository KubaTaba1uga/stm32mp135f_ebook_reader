#ifndef EBOOK_READER_EVENT_QUEUE_H
#define EBOOK_READER_EVENT_QUEUE_H

#include "utils/mem.h"

enum Events {
  Events_NONE,
  Events_BOOT_DONE,
  Events_BOOK_OPENED,
  Events_BOOK_CLOSED,
  Events_BOOK_UPDATED,
  Events_BTN_NEXT_PAGE_CLICKED,
  Events_BTN_PREV_PAGE_CLICKED,
  Events_BTN_MENU_CLICKED,
  Events_BOOK_SETTINGS_OPENED,
  Events_BOOK_SETTINGS_CLOSED,  
  // Add more events here
  Events_MAX,
};

enum EventSubscribers {
  EventSubscribers_NONE,
  EventSubscribers_MENU,
  EventSubscribers_READER,
  EventSubscribers_BOOK_SETTINGS,
  // Add more subscribers here
  EventSubscribers_MAX,
};

typedef struct EventQueue *event_queue_t;
typedef void (*post_event_func_t)(enum Events event, ref_t event_data,
                                  void *sub_data);

void event_queue_init(event_queue_t *out);
void event_queue_destroy(event_queue_t *out);
void event_queue_push(event_queue_t queue, enum Events event, ref_t event_data);
void event_queue_step(event_queue_t queue);
void event_queue_register(event_queue_t queue, enum EventSubscribers subscriber,
                          post_event_func_t subscriber_func,
                          void *subscriber_data);
void event_queue_deregister(event_queue_t queue, enum EventSubscribers subscriber);

#endif // EBOOK_READER_EVENT_QUEUE_H
