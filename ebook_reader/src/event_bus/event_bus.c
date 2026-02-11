#include "event_bus/event_bus.h"
#include "event_bus/core.h"
#include "utils/log.h"
#include "utils/mem.h"
#include <stddef.h>
#include <stdio.h>

/**
  Bus object holds multiple buses like bus for all modules, bus for menu module,
  bus for menu screen etc. An event can be put on a bus, which means that it
  will be delivered to all modules connected to the bus. So if we have Menu
  Screen module and Reader module and they are connected to Menu bus, menu
  screen and reader receive all events that appear on the bus. Connection
  between particular bus and particular module is called BusConnector, you can
  think of it as of wire going out of the module itself that we can plug into
  any bus.

  Bus solves issue of events propagation, imagine you want to create multiple
  layovers for the reader view. So when user reads a book he/she can search for
  a word in dictionary, create a note, save bookmark etc. Using bus and events
  we can easilly decouple modules responsible for each action from the rest of
  the app. Before we used FSM table which described all transitions in global
  state of the app but we soon discovered that it lead to big few modules in
  app which does everything.
*/
// @note: Propably we should minimize size of this struct.
static enum BusConnectorEnum
    route_table[BusEnum_MAX][EventEnum_MAX][BusConnectorEnum_MAX] = {
        [BusEnum_ALL] =
            {
                [EventEnum_BOOT_COMPLETED] = {BusConnectorEnum_MENU, 0},
            },
        [BusEnum_MENU] =
            {
                [EventEnum_MENU_ACTIVATED] = {BusConnectorEnum_MENU_SCREEN, 0},
                [EventEnum_MENU_DEACTIVATED] = {BusConnectorEnum_MENU_SCREEN,
                                                0},
                [EventEnum_BOOK_OPENED] = {BusConnectorEnum_READER,
                                           BusConnectorEnum_READER_SCREEN, 0},
            },
        [BusEnum_MENU_SCREEN] =
            {
                [EventEnum_BTN_ENTER] = {BusConnectorEnum_MENU, 0},
                [EventEnum_BTN_UP] = {BusConnectorEnum_MENU, 0},
                [EventEnum_BTN_DOWN] = {BusConnectorEnum_MENU, 0},
                [EventEnum_BTN_LEFT] = {BusConnectorEnum_MENU, 0},
                [EventEnum_BTN_RIGHT] = {BusConnectorEnum_MENU, 0},
            },
        [BusEnum_READER] = {
            [EventEnum_BOOK_CLOSED] = {BusConnectorEnum_READER_SCREEN, 0},
	}            
        
};

struct Bus {
  post_event_t func_table[BusConnectorEnum_MAX];
  void *data_table[BusConnectorEnum_MAX];
  struct EventQueue event_queue;
};

static void event_bus_route_event(bus_t bus, enum BusEnum dst,
                                  struct Event event);

void event_bus_init(bus_t *out) {
  bus_t bus = *out = mem_malloc(sizeof(struct Bus));
  *bus = (struct Bus){0};
}

void event_bus_destroy(bus_t *out) {
  puts(__func__);
  if (!out || !*out) {
    return;
  }

  struct EventQueueNode *node;
  for (node = event_queue_pull(&(*out)->event_queue); node != NULL;
       node = event_queue_pull(&(*out)->event_queue)) {
    mem_deref(node->event.data);
    mem_free(node);
  }

  mem_free(*out);
  *out = NULL;
}

void event_bus_register(bus_t bus, enum BusConnectorEnum ep,
                        post_event_t post_func, void *data) {
  bus->func_table[ep] = post_func;
  bus->data_table[ep] = data;
}

void event_bus_unregister(bus_t bus, enum BusConnectorEnum ep,
                          post_event_t post_func, void *data) {
  if (bus->func_table[ep] == post_func && bus->data_table[ep] == data) {
    bus->func_table[ep] = 0;
    bus->data_table[ep] = 0;
  }
}

void event_bus_post_event(bus_t bus, enum BusEnum dst, struct Event event) {
  struct EventQueueNode *node = mem_malloc(sizeof(struct EventQueueNode));
  *node = (struct EventQueueNode){.event = event, .bus = dst};
  node->event.data = mem_ref(node->event.data);

  log_debug("Received: (%s)->(%s)", bus_dump(dst), event_dump(event.event));

  event_queue_push(&bus->event_queue, node);
}

void event_bus_step(bus_t bus) {
  struct EventQueueNode *node;
  while ((node = event_queue_pull(&bus->event_queue)) != NULL) {
    log_debug("Handling: (%s)->(%s)", bus_dump(node->bus),
              event_dump(node->event.event));
    event_bus_route_event(bus, node->bus, node->event);

    mem_deref(node->event.data);
    mem_free(node);
  }
}

static void event_bus_route_event(bus_t bus, enum BusEnum dst,
                                  struct Event event) {
  post_event_t conn_func;
  void *conn_data;
  int i = 0;

  for (enum BusConnectorEnum conn = route_table[dst][event.event][i];
       conn != BusConnectorEnum_NONE;
       conn = route_table[dst][event.event][++i]) {
    conn_func = bus->func_table[conn];
    conn_data = bus->data_table[conn];
    if (!conn_func) {
      continue;
    }

    conn_func(event, conn_data);
  }
}

const char *bus_dump(enum BusEnum bus) {
  static const char *map[] = {
      [BusEnum_NONE] = "bus_none",
      [BusEnum_MENU] = "bus_menu",
      [BusEnum_MENU_SCREEN] = "bus_menu_screen",
  };

  if (bus < BusEnum_NONE || bus >= BusEnum_MAX || !map[bus]) {
    return "Unknown";
  }
  return map[bus];
}

const char *event_dump(enum EventEnum ev) {
  static const char *map[EventEnum_MAX] = {
      [EventEnum_NONE] = "ev_none",
      [EventEnum_BOOT_COMPLETED] = "ev_boot_completed",
      [EventEnum_BTN_ENTER] = "ev_btn_enter",
      [EventEnum_BTN_UP] = "ev_btn_up",
      [EventEnum_BTN_DOWN] = "ev_btn_down",
      [EventEnum_BTN_LEFT] = "ev_btn_left",
      [EventEnum_BTN_RIGHT] = "ev_btn_right",
      [EventEnum_MENU_ACTIVATED] = "ev_menu_activated",
      [EventEnum_MENU_DEACTIVATED] = "ev_menu_deactivated",
      [EventEnum_BOOK_OPENED] = "ev_book_opened",
  };

  if (ev < EventEnum_NONE || ev >= EventEnum_MAX || !map[ev]) {
    return "Unknown";
  }

  return map[ev];
}
