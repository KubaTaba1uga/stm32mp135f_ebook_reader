#include "error.h"

_Thread_local cdk_error_t cdk_errno = NULL;
_Thread_local struct cdk_Error cdk_hidden_errno = {0};

 int cdk_error_dumps(cdk_error_t err, size_t buf_size, char *buf) {
  size_t offset = 0;
  int written;

  written = snprintf(buf, buf_size,
                     "====== ERROR DUMP ======\n"
                     "Error code: %d\n"
                     "Error desc: %s\n",
                     err->code, strerror(err->code));
  if (written < 0 || (size_t)written >= buf_size) {
    return ENOBUFS;
  }
  offset += written;

  if (err->type > cdk_ErrorType_INT) {
    written =
        snprintf(buf + offset, buf_size - offset, "------------------------\n");
    if (written < 0 || (size_t)written >= buf_size) {
      return ENOBUFS;
    }
    offset += written;
  }

  switch (err->type) {
  case cdk_ErrorType_STR:
    written =
        snprintf(buf + offset, buf_size - offset, " Error msg: %s\n", err->msg);
    if (written < 0 || (size_t)written >= buf_size - offset) {
      return ENOBUFS;
    }
    offset += written;
    break;

#ifndef CDK_ERROR_OPTIMIZE
  case cdk_ErrorType_FSTR:
    written = snprintf(buf + offset, buf_size - offset, " Error msg: %.*s\n",
                       (int)sizeof(err->_msg_buf), err->msg);
    if (written < 0 || (size_t)written >= buf_size - offset) {
      return ENOBUFS;
    }
    offset += written;
    break;
#endif
  default:;
  }

  written =
      snprintf(buf + offset, buf_size - offset, "------------------------\n");
  if (written < 0 || (size_t)written >= buf_size - offset) {
    return ENOBUFS;
  }
  offset += written;

  written = snprintf(buf + offset, buf_size - offset, " Backtrace:\n");
  if (written < 0 || (size_t)written >= buf_size - offset) {
    return ENOBUFS;
  }
  offset += written;

  for (int i = 0; i < err->eframes_len; i++) {
    written = snprintf(buf + offset, buf_size - offset, "   [%02d] %s:%s:%d\n",
                       i, err->eframes[i].file, err->eframes[i].func,
                       err->eframes[i].line);
    if (written < 0 || (size_t)written >= buf_size - offset) {
      return ENOBUFS;
    }
    offset += written;
  }

  return 0;
}
