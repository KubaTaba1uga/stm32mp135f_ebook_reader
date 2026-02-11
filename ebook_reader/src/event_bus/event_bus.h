#ifndef EBOOK_READER_EVENT_BUS_H
#define EBOOK_READER_EVENT_BUS_H

#include "utils/mem.h"
struct Event;
typedef void (*post_event_t)(struct Event, void *);
typedef struct Bus *bus_t;

enum BusEnum {
  BusEnum_NONE = 0,
  BusEnum_ALL,
  BusEnum_MENU,
  BusEnum_MENU_SCREEN,
  BusEnum_READER,    
  BusEnum_READER_SCREEN,  
  BusEnum_MAX,
};

enum BusConnectorEnum {
  BusConnectorEnum_NONE = 0,
  BusConnectorEnum_MENU,
  BusConnectorEnum_MENU_SCREEN,
  BusConnectorEnum_READER,
  BusConnectorEnum_READER_SCREEN,  
  BusConnectorEnum_MAX,
};

enum EventEnum {
  EventEnum_NONE = 0,
  EventEnum_BOOT_COMPLETED,
  EventEnum_BTN_ENTER,
  EventEnum_BTN_UP,
  EventEnum_BTN_DOWN,
  EventEnum_BTN_LEFT,
  EventEnum_BTN_RIGHT,
  EventEnum_BTN_MENU,
  EventEnum_MENU_ACTIVATED,
  EventEnum_MENU_DEACTIVATED,
  EventEnum_BOOK_OPENED,
  EventEnum_BOOK_CLOSED,  
  EventEnum_MAX,
};

struct Event {
  enum EventEnum event;
  ref_t data;
};

void event_bus_init(bus_t *out);
void event_bus_destroy(bus_t *out);
void event_bus_register(bus_t bus, enum BusConnectorEnum ep,
                        post_event_t post_func, void *data);
void event_bus_unregister(bus_t bus, enum BusConnectorEnum ep,
                          post_event_t post_func, void *data);
void event_bus_post_event(bus_t bus, enum BusEnum dst, struct Event event);
void event_bus_step(bus_t bus);
const char *bus_dump(enum BusEnum bus);
const char *event_dump(enum EventEnum ev);

#endif // EBOOK_READER_EVENT_BUS_H
