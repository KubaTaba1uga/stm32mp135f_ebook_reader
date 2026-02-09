#ifndef EBOOK_READER_EVENT_BUS_H
#define EBOOK_READER_EVENT_BUS_H

/*******************************************************
                           BUS
 *******************************************************/
enum BusEnum {
  BusEnum_NONE = 0,
  BusEnum_USER,
  BusEnum_MENU,
  BusEnum_MENU_SCREEN,
  BusEnum_MAX,
};

enum EventEnum {
  EventEnum_NONE = 0,
  EventEnum_BOOT_COMPLETED,  
  EventEnum_BTN_ENTER,
  EventEnum_BTN_UP,
  EventEnum_BTN_DOWN,
  EventEnum_BTN_LEFT,
  EventEnum_BTN_RIGHT,
  EventEnum_MENU_ACTIVATED,
  EventEnum_MENU_DEACTIVATED,
  EventEnum_MAX,
};

struct Event {
  enum EventEnum event;
  void *data;
};

typedef void (*post_event_t)(struct Event, void *);

void event_bus_init(void);
void event_bus_post_event(enum BusEnum bus, struct Event event);
void event_bus_register(enum BusEnum bus, post_event_t post_func, void *data);
void event_bus_unregister(enum BusEnum bus, post_event_t post_func, void *data);
const char *event_dump(enum EventEnum event);
const char *bus_dump(enum BusEnum event);

#endif // EBOOK_READER_EVENT_BUS_H
