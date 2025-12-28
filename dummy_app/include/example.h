/**
 * \file example.h
 * \brief Public API for integer math helpers.
 *
 * This header defines small arithmetic helpers used by the core.
 * All functions are pure (no side effects) and thread-safe.
 *
 * \note This file is part of the public SDK.
 * \warning Do not include internal headers directly.
 *
 * \code
   #include "example.h"
   int s = add(2, 3);
 * \endcode
*/
int add(int a, int b);
