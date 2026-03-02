#include "Raspberry/lib/Config/dev_hardware_SPI.h"
#include "unistd.h"

int main(void) {
  DEV_HARDWARE_SPI_begin("/dev/spidev0.0");
  DEV_HARDWARE_SPI_setSpeed(1000000);
  DEV_HARDWARE_SPI_SetBitOrder(SPI_BIT_ORDER_LSBFIRST);
  /* DEV_HARDWARE_SPI_CSEN(DISABLE); */
  
  /* DEV_HARDWARE_SPI_TransferByte(0x60); */
  /* DEV_HARDWARE_SPI_TransferByte(0x0); */

  
  DEV_HARDWARE_SPI_TransferByte(0x60);
  DEV_HARDWARE_SPI_TransferByte(0x00);

  sleep(1);
  
  DEV_HARDWARE_SPI_TransferByte(0x00);
  DEV_HARDWARE_SPI_TransferByte(0x01);

  sleep(1);

  DEV_HARDWARE_SPI_TransferByte(0x60);
  DEV_HARDWARE_SPI_TransferByte(0x00);

  sleep(1);
  
  DEV_HARDWARE_SPI_TransferByte(0x03);
  DEV_HARDWARE_SPI_TransferByte(0x02);
  
  DEV_HARDWARE_SPI_TransferByte(0x10);
  DEV_HARDWARE_SPI_TransferByte(0x00);

  sleep(1);
  
  DEV_HARDWARE_SPI_TransferByte(0x00);
  DEV_HARDWARE_SPI_TransferByte(0x00);
  
  DEV_HARDWARE_SPI_end();
}
