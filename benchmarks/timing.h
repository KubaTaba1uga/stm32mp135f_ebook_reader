#include <time.h>
#include <stdio.h>
#include <unistd.h>

static inline long long now_ns(void);
static inline void print_ms(const char *name, long long dt_ns);

#define TIME_CALL(label, expr)                                                 \
  do {                                                                         \
    long long t0 = now_ns();                                                   \
    (expr);                                                                    \
    long long t1 = now_ns();                                                   \
    print_ms((label), (t1 - t0));                                              \
  } while (0)

static inline long long now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static inline void print_ms(const char *name, long long dt_ns) {
  printf("%s: %.3f ms\n", name, (double)dt_ns / 1e6);
}

