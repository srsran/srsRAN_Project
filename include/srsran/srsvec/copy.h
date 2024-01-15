/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "srsran/srsvec/detail/traits.h"
#include "srsran/srsvec/types.h"

namespace srsran {
namespace srsvec {

template <typename T, typename U>
void copy(T&& dst, const U& src)
{
  static_assert(detail::is_span_compatible<T>::value, "Template type is not compatible with a span");
  static_assert(detail::is_span_compatible<U>::value, "Template type is not compatible with a span");
  srsran_srsvec_assert_size(dst, src);

  std::copy(src.begin(), src.end(), dst.begin());
}

} // namespace srsvec
} // namespace srsran
