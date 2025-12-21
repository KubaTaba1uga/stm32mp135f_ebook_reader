#ifndef DD_BOARD_STM32MP135F_DK_H
#define DD_BOARD_STM32MP135F_DK_H

#include "gpio.h"

struct dd_board_stm32mp135f_dk;
typedef struct dd_board_stm32mp135f_dk *dd_board_stm32mp135f_dk_t;
int dd_board_stm32mp135f_dk_init(dd_board_stm32mp135f_dk_t *out);
void dd_board_stm32mp135f_dk_destroy(dd_board_stm32mp135f_dk_t *out);

#endif // DD_GPIO_STM32MP135F_DK_H
