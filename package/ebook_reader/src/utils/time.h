#ifndef TIME_H
#define TIME_H

#include <stdint.h>

struct Trace {
  const char *name;
  long long start;
};

void time_sleep_ms(int ms);
uint32_t time_now(void);
char *time_now_dump(char *buf, uint32_t buf_len);

struct Trace trace_start(const char *name);
void trace_end(struct Trace *trce);


#endif // MEM_H
