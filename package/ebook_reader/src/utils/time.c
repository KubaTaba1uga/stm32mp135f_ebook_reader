/* #define _POSIX_C_SOURCE 200809L */
#include "time.h"
#include <stdio.h>
#include <time.h>

void time_sleep_ms(int ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (long)(ms % 1000) * 1000000L;
  nanosleep(&ts, NULL);
}

uint32_t time_now(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

char *time_now_dump(char *buf, uint32_t buf_len) {
  struct tm *tmp;
  time_t t;

  t = time(NULL);
  tmp = localtime(&t);
  if (tmp == NULL) {
    return NULL;
  }

  if (strftime(buf, buf_len, "%a, %d %b %Y %H:%M", tmp) == 0) {
    return NULL;
  }

  return buf;
}

static inline long long time_now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static inline void print_ms(const char *name, long long dt_ns) {
  printf("%s: %.3f ms\n", name, (double)dt_ns / 1e6);
}

struct Trace trace_start(const char *name) {
  return (struct Trace){
      .name = name,
      .start = time_now_ns(),
  };
}

void trace_end(struct Trace *trce) {
  print_ms(trce->name, time_now_ns() - trce->start);
}
