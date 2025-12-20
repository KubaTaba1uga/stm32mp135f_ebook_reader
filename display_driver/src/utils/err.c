#include "err.h"

_Thread_local dd_error_t dd_errno = NULL;
_Thread_local struct dd_Error dd_hidden_errno = {0};
