#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "spi.h"
#include "utils/err.h"
#include "utils/mem.h"

struct dd_spi {
  int fd;
  struct spi_ioc_transfer transfer;
  uint32_t bus_mode;
  uint8_t bit_order;
};

int dd_spi_init(const char *path, dd_spi_t *out) {
  *out = dd_malloc(sizeof(struct dd_spi));
  memset(*out, 0, sizeof(struct dd_spi));

  (*out)->fd = open(path, O_RDWR);
  if ((*out)->fd < 0) {
    dd_errno = dd_errnof(errno, "Cannot open: %s", path);
    goto error;
  }

  (*out)->transfer.bits_per_word = 8;
  int ret = ioctl((*out)->fd, SPI_IOC_WR_BITS_PER_WORD,
                  &(*out)->transfer.bits_per_word);
  if (ret == -1) {
    dd_errno = dd_errnof(errno, "Cannot set bits per word for %s", path);
    goto error;
  }

  uint8_t bits_per_word;
  ret = ioctl((*out)->fd, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word);
  if (ret == -1) {
    dd_errno = dd_errnof(errno, "Cannot get bits per word for %s", path);
    goto error;
  }

  assert((*out)->transfer.bits_per_word == bits_per_word);

  (*out)->transfer.speed_hz = 10000000; // 10MHz
  if (ioctl((*out)->fd, SPI_IOC_WR_MAX_SPEED_HZ, &(*out)->transfer.speed_hz) ==
      -1) {
    dd_errno = dd_errnof(errno, "Cannot set speed for %s", path);
    goto error;
  }

  uint32_t transfer_speed;
  if (ioctl((*out)->fd, SPI_IOC_RD_MAX_SPEED_HZ, &transfer_speed) == -1) {
    dd_errno = dd_errnof(errno, "Cannot get speed for %s", path);
    goto error;
  }

  assert((*out)->transfer.speed_hz == transfer_speed);

  (*out)->bus_mode = SPI_MODE_0 | SPI_NO_CS;
  if (ioctl((*out)->fd, SPI_IOC_WR_MODE32, &(*out)->bus_mode) == -1) {
    dd_errno = dd_errnof(errno, "Cannot set spi mode for %s", path);
    goto error;
  }

  uint32_t bus_mode;
  if (ioctl((*out)->fd, SPI_IOC_RD_MODE32, &bus_mode) == -1) {
    dd_errno = dd_errnof(errno, "Cannot get spi mode for %s", path);
    goto error;
  }

  assert((*out)->bus_mode == bus_mode);

  (*out)->bit_order = 0; // Set MSB first
  if (ioctl((*out)->fd, SPI_IOC_WR_LSB_FIRST, &(*out)->bit_order) == -1) {
    dd_errno = dd_errnof(errno, "Cannot set LSB for %s", path);
    goto error;
  }

  uint8_t bit_order;
  if (ioctl((*out)->fd, SPI_IOC_RD_LSB_FIRST, &bit_order) == -1) {
    dd_errno = dd_errnof(errno, "Cannot get LSB for %s", path);
    goto error;
  }

  assert((*out)->bit_order == bit_order);

  (*out)->transfer.delay_usecs = 5;

  return 0;

error:
  dd_spi_destroy(out);
  return dd_ereturn(-1);
}

void dd_spi_destroy(dd_spi_t *out) {
  if (!out || !*out) {
    return;
  }

  close((*out)->fd);
  free(*out);
  *out = NULL;
}

int dd_spi_send_byte(uint8_t byte, dd_spi_t spi) {
  uint8_t rbuf;
  spi->transfer.len = 1;
  spi->transfer.tx_buf = (unsigned long)&byte;
  spi->transfer.rx_buf = (unsigned long)&rbuf;

  // ioctl Operation, transmission of data
  if (ioctl(spi->fd, SPI_IOC_MESSAGE(1), &spi->transfer) < 1) {
    dd_errno = dd_errnos(errno, "Cannot send SPI byte");
    goto error;
  }

  return 0;

error:
  return dd_ereturn(-1);
}

int dd_spi_send_bytes(uint8_t *bytes, uint32_t len, dd_spi_t spi) {
  uint8_t rbuf[len];  
  spi->transfer.len = len;
  spi->transfer.tx_buf = (unsigned long)bytes;
  spi->transfer.rx_buf = (unsigned long)rbuf;
  
  // ioctl Operation, transmission of data
  if (ioctl(spi->fd, SPI_IOC_MESSAGE(1), &spi->transfer) < 1) {
    dd_errno = dd_errnos(errno, "Cannot send SPI byte");
    goto error;
  }

  return 0;
  
error:
  return dd_ereturn(-1);
  
}
