#ifndef EBOOK_READER_LOG_H
#define EBOOK_READER_LOG_H
#include "utils/error.h"
#include <stdio.h>

// `##` in `##__VA_ARGS__` deletes `,` if no args provided
#ifdef DEBUG
#define log_info(fmt, ...)                                                     \
  fprintf(stdout, "(%16.16s:%16.16s:%4.4d)[INFOR]: " fmt "\n", __FILE__, __func__, __LINE__,   \
          ##__VA_ARGS__)
#define log_debug(fmt, ...)                                                    \
  fprintf(stdout, "(%16.16s:%16.16s:%4.4d)[DEBUG]: " fmt "\n", __FILE__, __func__, __LINE__,   \
          ##__VA_ARGS__)
#define log_warn(fmt, ...)                                                     \
  fprintf(stdout, "(%16.16s:%16.16s:%4.4d)[WARNI]: " fmt "\n", __FILE__, __func__, __LINE__,   \
          ##__VA_ARGS__)
#else
#define log_info(fmt, ...) fprintf(stdout, "[INFOR]: " fmt "\n", ##__VA_ARGS__)
#define log_debug(fmt, ...) fprintf(stdout, "[DEBUG]: " fmt "\n", ##__VA_ARGS__)
#define log_warn(fmt, ...) fprintf(stdout, "[WARNI]: " fmt "\n", ##__VA_ARGS__)
#endif
#define log_error(err)                                                         \
  {                                                                            \
    char buf[4096];                                                            \
    cdk_error_dumps(err, sizeof(buf), buf);                                    \
    fprintf(stdout, "[ERROR]: %s\n", buf);                                     \
  }



#endif // EBOOK_READER_MEM_H
