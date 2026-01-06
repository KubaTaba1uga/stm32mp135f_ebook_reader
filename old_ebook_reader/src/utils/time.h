#ifndef EBOOK_READER_TIME_H
#define EBOOK_READER_TIME_H

#include "utils/error.h"
#include <stdint.h>

cdk_error_t time_sleep_ms(uint32_t ms);
uint32_t time_now(void);

#endif // EBOOK_READER_MEM_H
