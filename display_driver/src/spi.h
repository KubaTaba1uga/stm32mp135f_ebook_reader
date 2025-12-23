#ifndef DD_SPI_H
#define DD_SPI_H

#include <stdint.h>

/* enum dd_spi_mode { */
/*   dd_spi_mode_0, /\*!< CPOL = 0, CPHA = 0 *\/ */
/*   dd_spi_mode_1, /\*!< CPOL = 0, CPHA = 1 *\/ */
/*   dd_spi_mode_2, /\*!< CPOL = 1, CPHA = 0 *\/ */
/*   dd_spi_mode_3, /\*!< CPOL = 1, CPHA = 1 *\/ */
/* }; */

/* enum dd_spi_csen { // chip select enabled/disabled */
/*   dd_spi_csen_enabled, */
/*   dd_spi_csen_disabled, */
/* }; */

/* enum dd_spi_bit_order { */
/*   dd_spi_bit_order_lsb, */
/*   dd_spi_bit_order_msb, */
/* }  ; */

/* enum dd_spi_bus_mode { */
/*   dd_spi_bus_mode_3wire, */
/*   dd_spi_bus_mode_4wire,   */
/* }; */

struct dd_spi;
typedef struct dd_spi *dd_spi_t;
int dd_spi_init(const char *path, dd_spi_t *out);
void dd_spi_destroy(dd_spi_t *out);
int dd_spi_send_byte(uint8_t byte, dd_spi_t spi);
int dd_spi_send_bytes(uint8_t *bytes, uint32_t len, dd_spi_t spi);

#endif // DD_SPI_H
