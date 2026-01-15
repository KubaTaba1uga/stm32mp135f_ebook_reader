#ifndef LOG_H
#define LOG_H
#include <stdio.h>

#include "err.h"

// `##` in `##__VA_ARGS__` deletes `,` if no args provided
#ifdef DEBUG
#define log_info(fmt, ...)                                                     \
  fprintf(stdout, "(%s:%s:%d)[INFO]: " fmt "\n", __FILE__, __func__, __LINE__, \
          ##__VA_ARGS__)
#define log_debug(fmt, ...)                                                    \
  fprintf(stdout, "(%s:%s:%d)[DEBUG]: " fmt "\n", __FILE__, __func__,          \
          __LINE__, ##__VA_ARGS__)
#define log_warn(fmt, ...)                                                     \
  fprintf(stdout, "(%s:%s:%d)[WARN]: " fmt "\n", __FILE__, __func__, __LINE__, \
          ##__VA_ARGS__)
#else
#define log_info(fmt, ...) fprintf(stdout, "[INFO]: " fmt "\n", ##__VA_ARGS__)
#define log_debug(fmt, ...) fprintf(stdout, "[DEBUG]: " fmt "\n", ##__VA_ARGS__)
#define log_warn(fmt, ...) fprintf(stdout, "[WARN]: " fmt "\n", ##__VA_ARGS__)
#endif
#define log_error(err)                                                         \
  {                                                                            \
    char buf[4096];                                                            \
    err_error_dumps(err, sizeof(buf), buf);                                    \
    fprintf(stdout, "[ERROR]: %s\n", buf);                                     \
  }

#endif // MEM_H
