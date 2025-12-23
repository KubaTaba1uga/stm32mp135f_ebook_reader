#define _DEFAULT_SOURCE
#include <stdint.h>
#include <unistd.h>

void dd_delay_ms(uint32_t ms) {
  for (uint32_t i = 0; i < ms; i++) {
    usleep(1000);
  }
}
