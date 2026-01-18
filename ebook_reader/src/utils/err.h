#ifndef ERR_H
#define ERR_H

/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
   MIT License

   Copyright (c) [2025] [Jakub Buczynski <KubaTaba1uga>]

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 */

#ifdef __STDC_NO_THREADS__
#error "Threads extension is required to compile this library"
#endif

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>

/******************************************************************************
 *                             Config / Limits                                *
 ******************************************************************************/
#ifndef ERR_FSTR_MAX
#define ERR_FSTR_MAX 255
#endif

#ifndef ERR_BTRACE_MAX
#define ERR_BTRACE_MAX 32
#endif

#ifndef ERR_DISABLE_ERRNO_API
#endif

#ifdef ERR_OPTIMIZE
#undef ERR_BTRACE_MAX
#define ERR_BTRACE_MAX 1
#endif

/******************************************************************************
 *                             Data types *
 ******************************************************************************/
/**
 * Error type.
 */
enum err_ErrorType {
  err_ErrorType_INT,
  err_ErrorType_STR,
#ifndef ERR_OPTIMIZE
  err_ErrorType_FSTR,
#endif
};

/**
 * Error frame object.
 */
struct err_EFrame {
  const char *file;
  const char *func;
  int line;
};

/**
 * Common error object.
 */
struct err_Error {
  enum err_ErrorType type;                         // Error type
  uint16_t code;                                   // Status code
  const char *msg;                                 // String msg, can be NULL
  struct err_EFrame eframes[ERR_BTRACE_MAX]; // Backtrace frames
  size_t eframes_len;                              // Backtrace frames length

#ifndef ERR_OPTIMIZE
  char _msg_buf[ERR_FSTR_MAX]; // Internal storage for formatted string
#endif
};

typedef struct err_Error *err_t;

/******************************************************************************
 *                                 Generic API                                *
 ******************************************************************************/
int err_error_dumps(err_t err, size_t buf_size, char *buf);

/**
 * Create struct err_Error of type err_ErrorType_INT.
 */
static inline err_t err_error_int(struct err_Error *err, uint16_t code,
                                        const char *file, const char *func,
                                        int line) {
  *err = (struct err_Error){
      .type = err_ErrorType_INT,
      .code = code,
      .eframes = {{.file = file, .func = func, .line = line}},
      .eframes_len = 1,
  };

  return err;
};

/**
 * Create struct err_Error of type err_ErrorType_STR.
 */
static inline err_t err_error_lstr(struct err_Error *err, uint16_t code,
                                         const char *file, const char *func,
                                         int line, const char *msg) {
  *err = (struct err_Error){
      .type = err_ErrorType_STR,
      .code = code,
      .msg = msg,
      .eframes = {{.file = file, .func = func, .line = line}},
      .eframes_len = 1,
  };

  return err;
};

#ifndef ERR_OPTIMIZE
/**
 * Create struct err_Error of type err_ErrorType_FSTR.
 */
static inline err_t err_error_fstr(struct err_Error *err, uint16_t code,
                                         const char *file, const char *func,
                                         int line, const char *fmt, ...) {

  *err = (struct err_Error){
      .type = err_ErrorType_FSTR,
      .code = code,
      .eframes = {{.file = file, .func = func, .line = line}},
      .eframes_len = 1,
  };

  va_list args;
  va_start(args, fmt);
  int written_bytes =
      vsnprintf(err->_msg_buf, sizeof(err->_msg_buf), fmt, args);
  va_end(args);

  assert(written_bytes >= 0);

  err->msg = err->_msg_buf;

  return err;
};
#endif

static inline void err_error_add_frame(err_t err,
                                       struct err_EFrame *frame) {
  if (err->eframes_len >= ERR_BTRACE_MAX) {
    return;
  }
  err->eframes[err->eframes_len++] = *frame;
}

#ifndef ERR_OPTIMIZE
#define err_error_wrap(err)                                                    \
  ({                                                                           \
    err_error_add_frame(err, &(struct err_EFrame){.file = __FILE_NAME__,       \
                                                  .func = __func__,            \
                                                  .line = __LINE__});          \
    err;                                                                       \
  })
#else
#define err_error_wrap(err)
#endif

#define err_error_return(ret, err)                                             \
  ({                                                                           \
    err_error_wrap(err);                                                       \
    ret;                                                                       \
  })

#define err_errori(err, code)                                                  \
  err_error_int((err), (code), __FILE_NAME__, __func__, __LINE__);

#define err_errors(err, code, msg)                                             \
  err_error_lstr((err), (code), __FILE_NAME__, __func__, __LINE__, (msg))

#define err_errorf(err, code, fmt, ...)                                        \
  err_error_fstr((err), (code), __FILE_NAME__, __func__, __LINE__, (fmt),      \
                 ##__VA_ARGS__)

/******************************************************************************
 *                                Errno API                                   *
 ******************************************************************************/
_Thread_local extern err_t err_o;
_Thread_local extern struct err_Error err_hidden_errno;

#define err_errnoi(code) err_errori(&err_hidden_errno, code)

#define err_errnos(code, msg) err_errors(&err_hidden_errno, code, msg)

#ifndef ERR_ERROR_OPTIMIZE
#define err_errnof(code, fmt, ...)                                             \
  err_errorf(&err_hidden_errno, code, fmt, ##__VA_ARGS__)
#endif

#define err_ewrap() err_error_wrap(&err_hidden_errno)

#define err_ereturn(ret) err_error_return((ret), &err_hidden_errno)

#define err_edumps(buf_size, buf)                                              \
  err_error_dumps(&err_hidden_errno, buf_size, buf)

#define ERR_TRY_CATCH(err, golabel)                                            \
  do {                                                                         \
    if ((err)) {                                                               \
      err_ewrap();                                                             \
      goto golabel;                                                            \
    }                                                                          \
  } while (0)

#define ERR_TRY(err) ERR_TRY_CATCH(err, error_out)

#endif // ERR_H
