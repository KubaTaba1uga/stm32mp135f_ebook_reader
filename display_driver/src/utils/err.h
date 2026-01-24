#ifndef DD_ERROR_H
#define DD_ERROR_H

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

//
////
//////
/******************************************************************************
 C Development Kit: Error – a modern, fast, errno-like error mechanism.

 The idea behind `errno` is great, but in modern languages errors usually carry
 more than just a status code. `dd_error` brings that spirit to C: a simple,
 lightweight error API with extra context such as string messages and manual
 backtraces.

 By default there are three error kinds:
   - Integer error
   - String error
   - Formatted string error (can be disabled)

 Each type exists for performance reasons. Integer errors are the fastest,
 string errors add only a little overhead, and formatted errors are the slowest
 because of formatting work. Since C is often chosen for performance, you can
 trim down to only what you need. Defining `DD_ERROR_OPTIMIZE` removes the
 formatted string buffer to save bytes.

 Another key feature is backtraces. Every error can store its own trace, making
 debugging far easier. Trace collection is manual, which might feel verbose,
 but it has important advantages:
   - extremely fast
   - works in every build (no special compiler flags)
   - includes only frames from your code, no external noise

******************************************************************************/
//////////

/******************************************************************************
 *                             Config / Limits                                *
 ******************************************************************************/
#ifndef DD_ERROR_FSTR_MAX
#define DD_ERROR_FSTR_MAX 255
#endif

#ifndef DD_ERROR_BTRACE_MAX
#define DD_ERROR_BTRACE_MAX 16
#endif

#ifndef DD_DISABLE_ERRNO_API
#endif

#ifdef DD_ERROR_OPTIMIZE
#undef DD_ERROR_BTRACE_MAX
#define DD_ERROR_BTRACE_MAX 1
#endif

/******************************************************************************
 *                             Data types *
 ******************************************************************************/
/**
 * Error type.
 */
enum dd_ErrorType {
  dd_ErrorType_INT,
  dd_ErrorType_STR,
#ifndef DD_ERROR_OPTIMIZE
  dd_ErrorType_FSTR,
#endif
};

/**
 * Error frame object.
 */
struct dd_EFrame {
  const char *file;
  const char *func;
  uint32_t line;
};

/**
 * Common error object.
 */
struct dd_Error {
  enum dd_ErrorType type;                        // Error type
  uint16_t code;                                 // Status code
  const char *msg;                               // String msg, can be NULL
  struct dd_EFrame eframes[DD_ERROR_BTRACE_MAX]; // Backtrace frames
  size_t eframes_len;                            // Backtrace frames length

#ifndef DD_ERROR_OPTIMIZE
  char _msg_buf[DD_ERROR_FSTR_MAX]; // Internal storage for formatted string
#endif
};

typedef struct dd_Error *dd_error_t;

/******************************************************************************
 *                                 Generic API                                *
 ******************************************************************************/
/**
 * Create struct dd_Error of type dd_ErrorType_INT.
 */
static inline dd_error_t dd_error_int(struct dd_Error *err, uint16_t code,
                                      const char *file, const char *func,
                                      int line) {
  *err = (struct dd_Error){
      .type = dd_ErrorType_INT,
      .code = code,
      .eframes = {{.file = file, .func = func, .line = line}},
      .eframes_len = 1,
  };

  return err;
};

/**
 * Create struct dd_Error of type dd_ErrorType_STR.
 */
static inline dd_error_t dd_error_lstr(struct dd_Error *err, uint16_t code,
                                       const char *file, const char *func,
                                       int line, const char *msg) {
  *err = (struct dd_Error){
      .type = dd_ErrorType_STR,
      .code = code,
      .msg = msg,
      .eframes = {{.file = file, .func = func, .line = line}},
      .eframes_len = 1,
  };

  return err;
};

#ifndef DD_ERROR_OPTIMIZE
/**
 * Create struct dd_Error of type dd_ErrorType_FSTR.
 */
static inline dd_error_t dd_error_fstr(struct dd_Error *err, uint16_t code,
                                       const char *file, const char *func,
                                       int line, const char *fmt, ...) {

  *err = (struct dd_Error){
      .type = dd_ErrorType_FSTR,
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

/**
 * Dump all struct dd_XError to string.
 */
int dd_error_dumps(dd_error_t err, size_t buf_size, char *buf);
static inline void dd_error_add_frame(dd_error_t err, struct dd_EFrame *frame) {
  if (err->eframes_len >= DD_ERROR_BTRACE_MAX) {
    return;
  }
  err->eframes[err->eframes_len++] = *frame;
}

#ifndef DD_ERROR_OPTIMIZE
#define dd_error_wrap(err)                                                     \
  ({                                                                           \
    dd_error_add_frame(err, &(struct dd_EFrame){.file = __FILE_NAME__,         \
                                                .func = __func__,              \
                                                .line = __LINE__});            \
    err;                                                                       \
  })
#else
#define dd_error_wrap(err)
#endif

#define dd_error_return(ret, err)                                              \
  ({                                                                           \
    dd_error_wrap(err);                                                        \
    ret;                                                                       \
  })

#define dd_errori(err, code)                                                   \
  dd_error_int((err), (code), __FILE_NAME__, __func__, __LINE__);

#define dd_errors(err, code, msg)                                              \
  dd_error_lstr((err), (code), __FILE_NAME__, __func__, __LINE__, (msg))

#define dd_errorf(err, code, fmt, ...)                                         \
  dd_error_fstr((err), (code), __FILE_NAME__, __func__, __LINE__, (fmt),       \
                ##__VA_ARGS__)

/******************************************************************************
 *                                Errno API                                   *
 ******************************************************************************/
_Thread_local extern dd_error_t dd_errno;
_Thread_local extern struct dd_Error dd_hidden_errno;

#define dd_errnoi(code) dd_errori(&dd_hidden_errno, code)

#define dd_errnos(code, msg) dd_errors(&dd_hidden_errno, code, msg)

#ifndef DD_ERROR_OPTIMIZE
#define dd_errnof(code, fmt, ...)                                              \
  dd_errorf(&dd_hidden_errno, code, fmt, ##__VA_ARGS__)
#endif

#define dd_ewrap() dd_error_wrap(&dd_hidden_errno)

#define dd_ereturn(ret) dd_error_return((ret), &dd_hidden_errno)

#define dd_edumps(buf_size, buf) dd_error_dumps(&dd_hidden_errno, buf_size, buf)

#define DD_TRY_CATCH(err, golabel)                                             \
  do {                                                                         \
    if ((err)) {                                                               \
      dd_ewrap();                                                              \
      goto golabel;                                                            \
    }                                                                          \
  } while (0)

#define DD_TRY(err) DD_TRY_CATCH(err, error_out)

#endif
