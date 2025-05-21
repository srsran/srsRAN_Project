/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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

namespace srsran {
namespace detail {
class scoped_enabler
{
public:
  scoped_enabler() { __rtsan_enable(); }
  ~scoped_enabler() { __rtsan_disable(); }
  scoped_enabler(const scoped_enabler&)            = delete;
  scoped_enabler& operator=(const scoped_enabler&) = delete;
  scoped_enabler(scoped_enabler&&)                 = delete;
  scoped_enabler& operator=(scoped_enabler&&)      = delete;
};
} // namespace detail
} // namespace srsran

#define SRSRAN_RTSAN_SCOPED_ENABLER ::srsran::detail::scoped_enabler rtsan_enabler##__LINE__

#else
#define SRSRAN_RTSAN_NONBLOCKING
#define SRSRAN_RTSAN_SCOPED_DISABLER(VAR)
#define SRSRAN_RTSAN_SCOPED_ENABLER
#endif
