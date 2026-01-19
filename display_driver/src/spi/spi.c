#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "spi/spi.h"
#include "utils/err.h"
#include "utils/mem.h"
#include "utils/io.h"

struct dd_SpiPrivate {
  int fd;
  struct spi_ioc_transfer transfer;
  uint32_t bus_mode;
  uint8_t bit_order;
};

dd_error_t dd_spi_init(const char *path, struct dd_Spi *spi) {
  int fd = dd_io_open(path, O_RDWR);
  if (fd < 0) {
    dd_errno = dd_errnof(errno, "Cannot open: %s", path);
    goto error_out;
  }

  *spi = (struct dd_Spi){
      .path = strdup(path),
      .private = dd_malloc(sizeof(struct dd_SpiPrivate)),
  };

  struct dd_SpiPrivate *spi_priv = spi->private;
  *spi_priv = (struct dd_SpiPrivate){.fd = fd};

  spi_priv->transfer.bits_per_word = 8;
  int ret = dd_io_ioctl(spi_priv->fd, SPI_IOC_WR_BITS_PER_WORD,
                  &spi_priv->transfer.bits_per_word);
  if (ret == -1) {
    dd_errno = dd_errnof(errno, "Cannot set bits per word for %s", path);
    goto error_spi_cleanup;
  }

  spi_priv->transfer.speed_hz = 10000000; // 10MHz
  if (dd_io_ioctl(spi_priv->fd, SPI_IOC_WR_MAX_SPEED_HZ,
            &spi_priv->transfer.speed_hz) == -1) {
    dd_errno = dd_errnof(errno, "Cannot set speed for %s", path);
    goto error_spi_cleanup;
  }

  spi_priv->bus_mode = SPI_MODE_0;
  if (dd_io_ioctl(spi_priv->fd, SPI_IOC_WR_MODE32, &spi_priv->bus_mode) == -1) {
    dd_errno = dd_errnof(errno, "Cannot set spi mode for %s", path);
    goto error_spi_cleanup;
  }

  spi_priv->bit_order = 0; // Set MSB first
  if (dd_io_ioctl(spi_priv->fd, SPI_IOC_WR_LSB_FIRST, &spi_priv->bit_order) == -1) {
    dd_errno = dd_errnof(errno, "Cannot set LSB for %s", path);
    goto error_spi_cleanup;
  }

  return 0;

error_spi_cleanup:
  dd_spi_destroy(spi);
error_out:
  return dd_errno;
};

void dd_spi_destroy(struct dd_Spi *spi) {
  if (!spi) {
    return;
  }

  if (spi->private) {
    struct dd_SpiPrivate *spi_priv = spi->private;
    if (spi_priv->fd) {
      dd_io_close(spi_priv->fd);
    }
  }

  dd_free((void *)spi->path);
  dd_free((void *)spi->private);
  *spi = (struct dd_Spi){0};
}

dd_error_t dd_spi_send_bytes(uint8_t *bytes, uint32_t len, struct dd_Spi *spi) {
  struct dd_SpiPrivate *spi_priv = spi->private;
  /* uint8_t rbuf; */

  spi_priv->transfer.len = len;
  spi_priv->transfer.tx_buf = (unsigned long)bytes;
  spi_priv->transfer.rx_buf = 0;

  // ioctl Operation, transmission of data
  if (dd_io_ioctl(spi_priv->fd, SPI_IOC_MESSAGE(1), &spi_priv->transfer) < 1) {
    dd_errno = dd_errnos(errno, "Cannot send SPI bytes");
    goto error_out;
  }

  return 0;

error_out:
  return dd_errno;
}

dd_error_t dd_spi_send_byte(uint8_t byte, struct dd_Spi *spi) {
  dd_errno = dd_spi_send_bytes(&byte, 1, spi);
  DD_TRY(dd_errno);

  return 0;

error_out:
  return dd_errno;
}
