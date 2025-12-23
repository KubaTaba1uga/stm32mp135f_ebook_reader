#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "gpio.h"
#include "spi.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/time.h"

struct dd_board {
  dd_gpio_pin_t cs;
  dd_gpio_pin_t dc;
  dd_gpio_pin_t rst;
  dd_gpio_pin_t bsy;
  dd_gpio_pin_t pwr;
  dd_spi_t spidev;
};

// dd_board takes ownership over pins memory.
int dd_board_init(dd_gpio_pin_t cs, dd_gpio_pin_t dc, dd_gpio_pin_t rst,
                  dd_gpio_pin_t bsy, dd_gpio_pin_t pwr, dd_spi_t spi,
                  dd_board_t *out) {
  *out = dd_malloc(sizeof(struct dd_board));

  **out = (struct dd_board){
      .cs = cs,
      .dc = dc,
      .rst = rst,
      .bsy = bsy,
      .pwr = pwr,
      .spidev = spi,
  };

  /* dd_gpio_pin_write((*out)->cs, 1); */
  dd_gpio_pin_write((*out)->dc, 0); // 0=command, 1=data
  dd_gpio_pin_write((*out)->rst, 1);
  /* dd_gpio_pin_write((*out)->bsy, 1); */ // 0=busy, 1=idle
  dd_gpio_pin_write((*out)->pwr, 1);

  return 0;
}

void dd_board_destroy(dd_board_t *out) {
  if (!out || !*out) {
    return;
  }

  dd_gpio_pin_destroy(&(*out)->cs);
  dd_gpio_pin_destroy(&(*out)->dc);
  dd_gpio_pin_destroy(&(*out)->rst);
  dd_gpio_pin_destroy(&(*out)->bsy);
  dd_gpio_pin_destroy(&(*out)->pwr);

  dd_spi_destroy(&(*out)->spidev);

  dd_free(*out);

  *out = NULL;
};

static int dd_board_reset(dd_board_t board) {
  if (dd_gpio_pin_write(board->rst, 1) == -1) {
    dd_ewrap();
    goto error;
  };
  dd_delay_ms(200);

  if (dd_gpio_pin_write(board->rst, 0) == -1) {
    dd_ewrap();
    goto error;
  };
  dd_delay_ms(100);

  if (dd_gpio_pin_write(board->rst, 1) == -1) {
    dd_ewrap();
    goto error;
  };
  dd_delay_ms(200);

  return 0;

error:
  return dd_ereturn(-1);
}

static int dd_board_send_command(uint8_t cmd, dd_board_t board) {
  // Choose sending command
  if (dd_gpio_pin_write(board->dc, 0) == -1) {
    dd_ewrap();
    goto error;
  };

  // Activate display as current slave
  if (dd_gpio_pin_write(board->cs, 0) == -1) {
    dd_ewrap();
    goto error;
  };

  if (dd_spi_send_byte(cmd, board->spidev) == -1) {
    dd_ewrap();
    goto error;
  };

  // Deactivate display as current slave
  if (dd_gpio_pin_write(board->cs, 1) == -1) {
    dd_ewrap();
    goto error;
  };

  return 0;

error:
  return dd_ereturn(-1);
}

int dd_board_send_data_bytes(uint8_t *bytes, uint32_t len, dd_board_t board) {
  // Choose sending data
  if (dd_gpio_pin_write(board->dc, 1) == -1) {
    dd_ewrap();
    goto error;
  };

  // Activate display as current slave
  if (dd_gpio_pin_write(board->cs, 0) == -1) {
    dd_ewrap();
    goto error;
  };

  if (dd_spi_send_bytes(bytes, len, board->spidev) == -1) {
    dd_ewrap();
    goto error;
  };

  // Deactivate display as current slave
  if (dd_gpio_pin_write(board->cs, 1) == -1) {
    dd_ewrap();
    goto error;
  };
  return 0;

error:
  return dd_ereturn(-1);
}

static int dd_board_send_data_byte(uint8_t byte, dd_board_t board) {
  // Choose sending data
  if (dd_gpio_pin_write(board->dc, 1) == -1) {
    dd_ewrap();
    goto error;
  };

  if (dd_gpio_pin_write(board->cs, 0) == -1) {
    dd_ewrap();
    goto error;
  };

  // Activate display as current slave
  if (dd_spi_send_byte(byte, board->spidev) == -1) {
    dd_ewrap();
    goto error;
  };

  // Deactivate display as current slave
  if (dd_gpio_pin_write(board->cs, 1) == -1) {
    dd_ewrap();
    goto error;
  };
  return 0;

error:
  return dd_ereturn(-1);
}

static void dd_board_wait(dd_board_t board) {
  puts("e-Paper busy\r\n");
  do {
    dd_delay_ms(5);
  } while (!(dd_gpio_pin_read(board->bsy)));
  dd_delay_ms(5);
  puts("e-Paper busy release\r\n");
}

int dd_board_power_on(dd_board_t board) {
  printf("Before rst: %d", dd_gpio_pin_read(board->bsy));
  DD_TRY(dd_board_reset(board));
  printf("After rst: %d", dd_gpio_pin_read(board->bsy));
  
  if (1) {
    return 0;
  }
  
  DD_TRY(dd_board_send_command(0x01, board)); // POWER SETTING
  DD_TRY(dd_board_send_data_byte(0x07, board));
  DD_TRY(dd_board_send_data_byte(0x07, board)); // VGH=20V,VGL=-20V
  DD_TRY(dd_board_send_data_byte(0x3f, board)); // VDH=15V
  DD_TRY(dd_board_send_data_byte(0x3f, board)); // VDL=-15V

  // Enhanced display drive(Add 0x06 command)
  DD_TRY(dd_board_send_command(0x06, board)); // Booster Soft Start
  DD_TRY(dd_board_send_data_byte(0x17, board));
  DD_TRY(dd_board_send_data_byte(0x17, board));
  DD_TRY(dd_board_send_data_byte(0x28, board));
  DD_TRY(dd_board_send_data_byte(0x17, board));

  DD_TRY(dd_board_send_command(0x04, board)); // POWER ON
  dd_delay_ms(100);
  dd_board_wait(board); // waiting for IC to release idle signal

  DD_TRY(dd_board_send_command(0X00, board)); // PANNEL SETTING
  DD_TRY(dd_board_send_data_byte(0x1F, board)); // KW-3f KWR-2F BWROTP 0f BWOTP 1f

  DD_TRY(dd_board_send_command(0x61, board));   // tres
  DD_TRY(dd_board_send_data_byte(0x03, board)); // source 800
  DD_TRY(dd_board_send_data_byte(0x20, board));
  DD_TRY(dd_board_send_data_byte(0x01, board)); // gate 480
  DD_TRY(dd_board_send_data_byte(0xE0, board));

  DD_TRY(dd_board_send_command(0X15, board));
  DD_TRY(dd_board_send_data_byte(0x00, board));

  /*
      If the screen appears gray, use the annotated initialization command
  */
  DD_TRY(dd_board_send_command(0X50, board));
  DD_TRY(dd_board_send_data_byte(0x10, board));
  DD_TRY(dd_board_send_data_byte(0x07, board));
  // dd_board_send_command(0X50);
  // dd_board_send_data_byte(0x10);
  // dd_board_send_data_byte(0x17);
  // dd_board_send_command(0X52);
  // dd_board_send_data_byte(0x03);

  DD_TRY(dd_board_send_command(0X60, board)); // TCON SETTING
  DD_TRY(dd_board_send_data_byte(0x22, board));

  return 0;

error:
  return dd_ereturn(-1);
}
