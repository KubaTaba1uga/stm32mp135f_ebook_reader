#include "stm32board.h"
#include "board.h"
#include "error.h"
#include "gpio.h"

int stm32board_init(struct stm32board *out) {
  gpio_bank gpioa;
  gpio_bank gpioc;
  gpio_bank gpiog;
  gpio_bank gpioh;
  gpio_bank gpioi;
  struct gpio din;
  struct gpio clk;
  struct gpio cs;
  struct gpio dc;
  struct gpio rst;
  struct gpio bsy;
  struct gpio pwr;

  int err = gpio_bank_init('A', &gpioa);
  if (err) {
    cdk_ewrap();
    goto error;
  }

  err = gpio_bank_init('C', &gpioc);
  if (err) {
    cdk_ewrap();
    goto error_gpioa_cleanup;
  }

  err = gpio_bank_init('G', &gpiog);
  if (err) {
    cdk_ewrap();
    goto error_gpioc_cleanup;
  }

  err = gpio_bank_init('H', &gpioh);
  if (err) {
    cdk_ewrap();
    goto error_gpiog_cleanup;
  }

  err = gpio_bank_init('I', &gpioi);
  if (err) {
    cdk_ewrap();
    goto error_gpioh_cleanup;
  }

  err = gpio_init(gpioh, 3, &din);
  if (err) {
    cdk_ewrap();
    goto error_gpioi_cleanup;
  }

  err = gpio_init(gpioh, 7, &clk);
  if (err) {
    cdk_ewrap();
    goto error_din_cleanup;
  }

  err = gpio_init(gpioh, 11, &cs);
  if (err) {
    cdk_ewrap();
    goto error_clk_cleanup;
  }

  err = gpio_init(gpioi, 0, &dc);
  if (err) {
    cdk_ewrap();
    goto error_cs_cleanup;
  }

  err = gpio_init(gpioc, 11, &rst);
  if (err) {
    cdk_ewrap();
    goto error_dc_cleanup;
  }

  err = gpio_init(gpiog, 3, &bsy);
  if (err) {
    cdk_ewrap();
    goto error_rst_cleanup;
  }

  err = gpio_init(gpioa, 4, &pwr);
  if (err) {
    cdk_ewrap();
    goto error_bsy_cleanup;
  }

  err = board_init(din, clk, cs, dc, rst, bsy, pwr, &out->board);
  if (err) {
    cdk_ewrap();
    goto error_pwr_cleanup;    
  }

  return 0;
  
error_pwr_cleanup:
  gpio_destroy(&pwr);  
error_bsy_cleanup:
  gpio_destroy(&bsy);
error_rst_cleanup:
  gpio_destroy(&rst);
error_dc_cleanup:
  gpio_destroy(&dc);
error_cs_cleanup:
  gpio_destroy(&cs);
error_clk_cleanup:
  gpio_destroy(&clk);
error_din_cleanup:
  gpio_destroy(&din);
error_gpioa_cleanup:
  gpio_bank_destroy(&gpioa);
error_gpioc_cleanup:
  gpio_bank_destroy(&gpioc);
error_gpiog_cleanup:
  gpio_bank_destroy(&gpiog);
error_gpioh_cleanup:
  gpio_bank_destroy(&gpioh);
error_gpioi_cleanup:
  gpio_bank_destroy(&gpioi);
error:
  return err;
}

void stm32board_destroy(struct stm32board *out) {
  if (!out) {
    return;
  }

  gpio_bank_destroy(&out->board.din.bank);
  gpio_bank_destroy(&out->board.dc.bank);
  gpio_bank_destroy(&out->board.rst.bank);
  gpio_bank_destroy(&out->board.busy.bank);
  gpio_bank_destroy(&out->board.pwr.bank);

  board_destroy(&out->board);
};
