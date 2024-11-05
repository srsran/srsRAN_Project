/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Defines helper macros to use RTSAN, if supported.

#pragma once

#ifdef __has_feature
#if __has_feature(realtime_sanitizer)
#define SRSRAN_RTSAN_ENABLED
#endif
#endif

#ifdef SRSRAN_RTSAN_ENABLED
#include <sanitizer/rtsan_interface.h>

#define SRSRAN_RTSAN_NONBLOCKING [[clang::nonblocking]]
#define SRSRAN_RTSAN_SCOPED_DISABLER(VAR) __rtsan::ScopedDisabler(VAR);
#else
#define SRSRAN_RTSAN_NONBLOCKING
#define SRSRAN_RTSAN_SCOPED_DISABLER(VAR)
#endif
