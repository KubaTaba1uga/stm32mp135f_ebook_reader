#ifndef DISPLAY_DRIVER_SPI_H
#define DISPLAY_DRIVER_SPI_H

#include <stdint.h>

#include "display_driver.h"
struct dd_Spi {
  const char *path;
  void *private;
};

dd_error_t dd_spi_init(const char *path, struct dd_Spi *spi);
void dd_spi_destroy(struct dd_Spi *spi);
dd_error_t dd_spi_send_byte(uint8_t byte, struct dd_Spi *spi);
dd_error_t dd_spi_send_bytes(uint8_t *bytes, uint32_t len, struct dd_Spi *spi);

#endif // DISPLAY_DRIVER_SPI_H
