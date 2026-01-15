#ifndef EBOOK_READER_CORE_INTERNAL_H
#define EBOOK_READER_CORE_INTERNAL_H
#include <stdbool.h>

#include "book/book.h"
#include "ui/ui.h"
#include "utils/err.h"

typedef struct App *app_t;
typedef struct AppCtx *app_ctx_t;
typedef struct AppModule *app_module_t;

#define EBK_CORE_STATE_ENUM_INIT -1

struct AppModule {
  void (*open)(app_module_t, app_ctx_t, void *);
  void (*close)(app_module_t);
  void (*destroy)(app_module_t);
  void *private;
};

struct AppCtx {
  ui_t ui;
  book_api_t book_api;  
};

/**
   @brief Events occuring in the system.
 */
enum AppEventEnum {
  AppEventEnum_NONE = 0,
  /**
   Once device finish boot we emmit BOOT_DONE.
   This is first event which occur in the system.
   */
  AppEventEnum_BOOT_DONE,
  /**
  Once enter button is pressed we emmit BTN_ENTER.
   */
  AppEventEnum_BTN_ENTER,
  /**
   Once left button is pressed we emmit BTN_LEFT.
   */
  AppEventEnum_BTN_LEFT,
  /**
   Once rigth button is pressed we emmit BTN_RIGTH.
   */
  AppEventEnum_BTN_RIGTH,
  /**
   Once up button is pressed we emmit BTN_UP.
   */
  AppEventEnum_BTN_UP,
  /**
   Once down button is pressed we emmit BTN_DOWN.
   */
  AppEventEnum_BTN_DOWN,
  /**
   Once menu button is pressed we emmit BTN_MENU.
   */
  AppEventEnum_BTN_MENU,
  AppEventEnum_BOOK_SELECTED,  
  /**
   Once any module meet error that cannot be gracefully handled
   so the user can go back to using the device we emmit ERROR_RAISED.
   Once device is in error mode it log info about error and resets itself.
   */
  AppEventEnum_ERROR_RAISED,
  // Add more events here
  AppEventEnum_MAX,  
};

struct AppEventData {
  enum AppEventEnum event;
  void *data;
};


/**
  @brief Post event to the core.
  @param core Pointer to initialized instance of core.
  @param event Event to post.
  @param data Generic data in case event needs payload.
 */
void app_event_post(app_t core, enum AppEventEnum event,
                         void *data);


/**
   @brief Dump event to string.
   @param state Event to dump.
   @return Null terminated string.
*/
const char *app_event_dump(enum AppEventEnum event);

/**
   @brief Post ERROR_RAISED event.
   @param core Core instance to receive the event.
   @param error Error which will be handled by error module.
*/
void app_raise_error(app_t core, err_t error);

err_t app_menu_init(app_module_t menu, app_t app);
err_t app_error_init(app_module_t menu, app_t app);
err_t app_reader_init(app_module_t menu, app_t app);

#endif // EBOOK_READER_CORE_INTERNAL_H
