#ifndef DISP_DRVER_STM32BOARD_H
#define DISP_DRVER_STM32BOARD_H
#include "board.h"

struct stm32board {
  struct board board;
};

int stm32board_init(struct stm32board *out);
void stm32board_destroy(struct stm32board *out);

#endif // DISP_DRVER_GPIO_H

