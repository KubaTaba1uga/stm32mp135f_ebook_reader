Application
===========

Application is located in ebook_reader directory, the architecture of the application
is based on the finite state machine with states like BOOT, MENU,  READER, ERROR and potentially more.

Application beside state has concept of events, each event describe something that happen to the application
like button press or it can be somthing that happen inside the app like boot is completed.

The transitions of the state machine are described in `src/app/core.c`. Each transition is done based on input event
and current state and lead to the next state. For example sake let's analyze boot trasnition, boot tranisitons from
boot state to menu state, once we initialize app we display to the user menu with the bookshelf. So when app broadcast
BOOT_DONE event we move to the MENU state and fire up app_module_menu_open function which is responsible for displaying
menu.

So application is composed of states, each state has corresponding module and each application module has corresponding screen.
So we have ``app state`` -> ``app module`` -> ``ui screen``. App module handles logic behind the state, and screen handle
presentation of the state. Screen itself is composed of multiple widgets wich are lously gatherd in ``ui/widgets.c``.

Directory layout of the application
-------------------------

..   code::
     ebook_reader/
     ├── data
     │   ├── ...
     ├── meson.build
     ├── meson.options
     ├── src
     │   ├── app
     │   │   ├── app.h
     │   │   ├── core.c
     │   │   ├── core.h
     │   │   ├── module.c
     │   │   ├── module.h
     │   │   ├── module_error.c
     │   │   ├── module_error.h
     │   │   ├── module_menu.c
     │   │   ├── module_menu.h
     │   │   ├── module_reader.c
     │   │   ├── module_reader.h
     │   ├── book
     │   │   ├── book.h
     │   │   ├── core.c
     │   │   ├── core.h
     │   │   └── pdf.c
     │   ├── main.c
     │   ├── ui
     │   │   ├── ui.h
     │   │   ├── core.c
     │   │   ├── core.h
     │   │   ├── display.c
     │   │   ├── display.h
     │   │   ├── display_wvs7in5v2b.c
     │   │   ├── display_wvs7in5v2b.h
     │   │   ├── display_x11.c
     │   │   ├── display_x11.h
     │   │   ├── screen.c
     │   │   ├── screen.h
     │   │   ├── screen_menu.c
     │   │   ├── screen_menu.h
     │   │   ├── widgets.c
     │   │   └── widgets.h
     │   └── utils
     │       ├── err.c
     │       ├── err.h
     │       ├── graphic.c
     │       ├── graphic.h
     │       ├── log.h
     │       ├── mem.c
     │       ├── mem.h
     │       ├── settings.c
     │       ├── settings.h
     │       ├── time.c
     │       ├── time.h
     │       ├── zlist.c
     │       └── zlist.h
     ├── subprojects
     │   └── ...     
     └── test
	 ├── meson.build
	 ├── test_list.c
	 ├── test_unity.c


The root of the repository is meant mostly for meson configuration files.

``data`` directory is for all assets which are not meant to be in app's memory.
For example various photos like photo we display during boot screen.

``src`` directory is meant for all app's code. Central point of the src directory is ``src/app`` where reside ``struct App``,
this struct represent our app hence it is composed of other subsystems implemented in our app. Each subsystem is located
in ``src`` in it's own directory and contain public interface in form of ``src/<subsystem name>/<subsystem name>.h``.
Each subsystem creates it's own namespace, that's why all funcs or types in app directory uses app prefix and similiarly
in ui directory all funcs and structs uses ui prefix. We create one namespace per submodule to keep things clean.
``src/utils`` directory is meant for small code helpers, each creating it's own namespace.
``src/main.c`` uses app subsystem and sets up OS for it, that's why you will find signal hendlers, exit handlers etc. in main.c.

In ``subprojects`` we use placed only lvgl all other deps are managed by buildroot. We decided to do it this way because
lvgl is often customized during development and rebuilduing it with buildroot all the time would be very annoying.

In ``test`` we place all tests the project. Beside tests we are havily using asserts in our code. We have signal handler for
SIGABRT so we can safely ensure that stuff like display drivers are not left in broken state. We encourage you to use asserts
in your code as well.




