#ifndef DD_BOARD_RASPBERRYPI_5_H
#define DD_BOARD_RASPBERRYPI_5_H

#include "board.h"

struct dd_board_raspberrypi_5;
typedef struct dd_board_raspberrypi_5 *dd_board_raspberrypi_5_t;
int dd_board_raspberrypi_5_init(dd_board_raspberrypi_5_t *out);
void dd_board_raspberrypi_5_destroy(dd_board_raspberrypi_5_t *out);
dd_board_t dd_board_raspberrypi_5_get_board(dd_board_raspberrypi_5_t board);

#endif // DD_GPIO_RASPBERRYPI_5_H
