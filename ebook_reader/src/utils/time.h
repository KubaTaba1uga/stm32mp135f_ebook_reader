#ifndef TIME_H
#define TIME_H

#include <stdint.h>

void time_sleep_ms(int ms);
uint32_t time_now(void);
char * time_now_dump(char *buf, uint32_t buf_len);

#endif // MEM_H
