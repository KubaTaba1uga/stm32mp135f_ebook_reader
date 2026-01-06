#Ifndef EBK_ERROR_H
#define EBK_ERROR_H

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
 more than just a status code. `ebk_error` brings that spirit to C: a simple,
 lightweight error API with extra context such as string messages and manual
 backtraces.

 By default there are three error kinds:
   - Integer error
   - String error
   - Formatted string error (can be disabled)

 Each type exists for performance reasons. Integer errors are the fastest,
 string errors add only a little overhead, and formatted errors are the slowest
 because of formatting work. Since C is often chosen for performance, you can
 trim down to only what you need. Defining `EBK_ERROR_OPTIMIZE` removes the
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
#ifndef EBK_ERROR_FSTR_MAX
#define EBK_ERROR_FSTR_MAX 255
#endif

#ifndef EBK_ERROR_BTRACE_MAX
#define EBK_ERROR_BTRACE_MAX 32
#endif

#ifndef EBK_DISABLE_ERRNO_API
#endif

#ifdef EBK_ERROR_OPTIMIZE
#undef EBK_ERROR_BTRACE_MAX
#define EBK_ERROR_BTRACE_MAX 1
#endif

/******************************************************************************
 *                             Data types *
 ******************************************************************************/
/**
 * Error type.
 */
enum ebk_ErrorType {
  ebk_ErrorType_INT,
  ebk_ErrorType_STR,
#ifndef EBK_ERROR_OPTIMIZE
  ebk_ErrorType_FSTR,
#endif
};

/**
 * Error frame object.
 */
struct ebk_EFrame {
  const char *file;
  const char *func;
  uint32_t line;
};

/**
 * Common error object.
 */
struct ebk_Error {
  enum ebk_ErrorType type;                         // Error type
  uint16_t code;                                   // Status code
  const char *msg;                                 // String msg, can be NULL
  struct ebk_EFrame eframes[EBK_ERROR_BTRACE_MAX]; // Backtrace frames
  size_t eframes_len;                              // Backtrace frames length

#ifndef EBK_ERROR_OPTIMIZE
  char _msg_buf[EBK_ERROR_FSTR_MAX]; // Internal storage for formatted string
#endif
};

typedef struct ebk_Error *ebk_error_t;

/******************************************************************************
 *                                 Generic API                                *
 ******************************************************************************/
/**
 * Create struct ebk_Error of type ebk_ErrorType_INT.
 */
static inline ebk_error_t ebk_error_int(struct ebk_Error *err, uint16_t code,
                                        const char *file, const char *func,
                                        int line) {
  *err = (struct ebk_Error){
      .type = ebk_ErrorType_INT,
      .code = code,
      .eframes = {{.file = file, .func = func, .line = line}},
      .eframes_len = 1,
  };

  return err;
};

/**
 * Create struct ebk_Error of type ebk_ErrorType_STR.
 */
static inline ebk_error_t ebk_error_lstr(struct ebk_Error *err, uint16_t code,
                                         const char *file, const char *func,
                                         int line, const char *msg) {
  *err = (struct ebk_Error){
      .type = ebk_ErrorType_STR,
      .code = code,
      .msg = msg,
      .eframes = {{.file = file, .func = func, .line = line}},
      .eframes_len = 1,
  };

  return err;
};

#ifndef EBK_ERROR_OPTIMIZE
/**
 * Create struct ebk_Error of type ebk_ErrorType_FSTR.
 */
static inline ebk_error_t ebk_error_fstr(struct ebk_Error *err, uint16_t code,
                                         const char *file, const char *func,
                                         int line, const char *fmt, ...) {

  *err = (struct ebk_Error){
      .type = ebk_ErrorType_FSTR,
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
 * Dump all struct ebk_XError to string.
 */
int ebk_error_dumps(ebk_error_t err, size_t buf_size, char *buf);
static inline void ebk_error_aebk_frame(ebk_error_t err,
                                       struct ebk_EFrame *frame) {
  if (err->eframes_len >= EBK_ERROR_BTRACE_MAX) {
    return;
  }
  err->eframes[err->eframes_len++] = *frame;
}

#ifndef EBK_ERROR_OPTIMIZE
#define ebk_error_wrap(err)                                                    \
  ({                                                                           \
    ebk_error_aebk_frame(err, &(struct ebk_EFrame){.file = __FILE_NAME__,       \
                                                  .func = __func__,            \
                                                  .line = __LINE__});          \
    err;                                                                       \
  })
#else
#define ebk_error_wrap(err)
#endif

#define ebk_error_return(ret, err)                                             \
  ({                                                                           \
    ebk_error_wrap(err);                                                       \
    ret;                                                                       \
  })

#define ebk_errori(err, code)                                                  \
  ebk_error_int((err), (code), __FILE_NAME__, __func__, __LINE__);

#define ebk_errors(err, code, msg)                                             \
  ebk_error_lstr((err), (code), __FILE_NAME__, __func__, __LINE__, (msg))

#define ebk_errorf(err, code, fmt, ...)                                        \
  ebk_error_fstr((err), (code), __FILE_NAME__, __func__, __LINE__, (fmt),      \
                 ##__VA_ARGS__)

/******************************************************************************
 *                                Errno API                                   *
 ******************************************************************************/
_Thread_local extern ebk_error_t ebk_errno;
_Thread_local extern struct ebk_Error ebk_hidden_errno;

#define ebk_errnoi(code) ebk_errori(&ebk_hidden_errno, code)

#define ebk_errnos(code, msg) ebk_errors(&ebk_hidden_errno, code, msg)

#ifndef EBK_ERROR_OPTIMIZE
#define ebk_errnof(code, fmt, ...)                                             \
  ebk_errorf(&ebk_hidden_errno, code, fmt, ##__VA_ARGS__)
#endif

#define ebk_ewrap() ebk_error_wrap(&ebk_hidden_errno)

#define ebk_ereturn(ret) ebk_error_return((ret), &ebk_hidden_errno)

#define ebk_edumps(buf_size, buf)                                              \
  ebk_error_dumps(&ebk_hidden_errno, buf_size, buf)

#define EBK_TRY_CATCH(err, golabel)    \
  do {                        \
    if ((err)) {       \
      ebk_ewrap();             \
      goto golabel;             \
    }                         \
  } while (0)

#define EBK_TRY(err) EBK_TRY_CATCH(err, error_out)

#endif
