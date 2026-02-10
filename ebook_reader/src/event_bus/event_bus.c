#include "event_bus/event_bus.h"
#include "event_bus/core.h"
#include "utils/mem.h"

// @note: Propably we should minimize size of this struct.
static enum EndpointEnum
    event_bus_route_table[BusEnum_MAX][EventEnum_MAX][EndpointEnum_MAX] = {
        [BusEnum_ALL] =
            {
                [EventEnum_BOOT_COMPLETED] = {EndpointEnum_MENU, 0},
            },
        [BusEnum_MENU] =
            {
                [EventEnum_MENU_ACTIVATED] = {EndpointEnum_MENU_SCREEN, 0},
                [EventEnum_MENU_DEACTIVATED] = {EndpointEnum_MENU_SCREEN, 0},
                [EventEnum_BOOK_OPENED] = {EndpointEnum_READER, 0},
            },
        [BusEnum_MENU_SCREEN] =
            {
                [EventEnum_BTN_ENTER] = {EndpointEnum_MENU, 0},
                [EventEnum_BTN_UP] = {EndpointEnum_MENU, 0},
                [EventEnum_BTN_DOWN] = {EndpointEnum_MENU, 0},
                [EventEnum_BTN_LEFT] = {EndpointEnum_MENU, 0},
                [EventEnum_BTN_RIGHT] = {EndpointEnum_MENU, 0},
            },
};
static post_event_t event_bus_func_table[EndpointEnum_MAX] = {0};
static void *event_bus_data_table[EndpointEnum_MAX] = {0};
struct EventQueue event_queue = {0};

static void event_bus_route_event(enum BusEnum bus, struct Event event);

void event_bus_init(void) { event_queue_init(&event_queue); }

void event_bus_register(enum EndpointEnum ep, post_event_t post_func,
                        void *data) {
  event_bus_func_table[ep] = post_func;
  event_bus_data_table[ep] = data;
}

void event_bus_unregister(enum EndpointEnum ep, post_event_t post_func,
                          void *data) {
  if (event_bus_func_table[ep] == post_func &&
      event_bus_data_table[ep] == data) {
    event_bus_func_table[ep] = 0;
    event_bus_data_table[ep] = 0;
  }
}

void event_bus_post_event(enum BusEnum bus, struct Event event) {
  struct EventQueueNode *node = mem_malloc(sizeof(struct EventQueueNode));
  *node = (struct EventQueueNode){.event = event, .bus = bus};
  event_queue_push(&event_queue, node);
}

void event_bus_step(void) {
  struct EventQueueNode *node;
  while ((node = event_queue_pull(&event_queue)) != NULL) {
    event_bus_route_event(node->bus, node->event);

    mem_free(node);
  }
}

static void event_bus_route_event(enum BusEnum bus, struct Event event) {
  post_event_t dst_func;
  void *dst_data;
  int i = 0;

  for (enum EndpointEnum dst = event_bus_route_table[bus][event.event][i];
       dst != EndpointEnum_NONE;
       dst = event_bus_route_table[bus][event.event][++i]) {
    dst_func = event_bus_func_table[dst];
    dst_data = event_bus_data_table[dst];
    if (!dst_func) {
      continue;
    }

    dst_func(event, dst_data);
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
  static const char *map[] = {
      [EventEnum_NONE] = "ev_none",
      [EventEnum_BOOT_COMPLETED] = "ev_boot_completed",
      [EventEnum_BTN_ENTER] = "ev_btn_enter",
      [EventEnum_BTN_UP] = "ev_btn_up",
      [EventEnum_BTN_DOWN] = "ev_btn_down",
      [EventEnum_BTN_LEFT] = "ev_btn_left",
      [EventEnum_BTN_RIGHT] = "ev_btn_right",
      [EventEnum_MENU_ACTIVATED] = "ev_menu_activated",
      [EventEnum_MENU_DEACTIVATED] = "ev_menu_deactivated",
  };

  if (ev < EventEnum_NONE || ev >= EventEnum_MAX || !map[ev]) {
    return "Unknown";
  }

  return map[ev];
}
