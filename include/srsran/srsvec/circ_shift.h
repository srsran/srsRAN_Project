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

/// \file
/// \brief Circular shifts.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/srsvec/copy.h"

namespace srsran {
namespace srsvec {

/// \brief Circularly shifts a sequence in the forward direction.
///
/// The element at position \c i is moved to position <tt>i + shift<\tt>. Elements that fall beyond the end of the
/// sequence are reintroduced at its start.
/// \param[out] out       Shifted output sequence.
/// \param[in]  in        Original input sequence.
/// \param[in]  shift     The number of positions the sequence is shifted by.
/// \remark Cannot be used to override memory.
template <typename T, typename U>
void circ_shift_forward(T&& out, const U& in, unsigned shift)
{
  static_assert(detail::is_span_compatible<T>::value, "Template type is not compatible with a span");
  static_assert(detail::is_span_compatible<U>::value, "Template type is not compatible with a span");
  srsran_srsvec_assert_size(out, in);

  unsigned length = out.size();
  srsran_assert(std::abs(in.data() - out.data()) >= length, "Input and output memory overlap.");

  copy(out.first(shift), in.last(shift));
  copy(out.subspan(shift, length - shift), in.first(length - shift));
}

/// \brief Circularly shifts a sequence in the backward direction.
///
/// The element at position \c i is moved to position <tt>i - shift<\tt>. Elements that fall beyond the beginning of the
/// sequence are reintroduced at its end.
/// \param[out] out       Shifted output sequence.
/// \param[in]  in        Original input sequence.
/// \param[in]  shift     The number of positions the sequence is shifted by.
/// \remark Cannot be used to override memory.
template <typename T, typename U>
void circ_shift_backward(T&& out, const U& in, unsigned shift)
{
  static_assert(detail::is_span_compatible<T>::value, "Template type is not compatible with a span");
  static_assert(detail::is_span_compatible<U>::value, "Template type is not compatible with a span");
  srsran_srsvec_assert_size(out, in);

  unsigned length = out.size();
  srsran_assert(std::abs(in.data() - out.data()) >= length, "Input and output memory overlap.");

  copy(out.first(length - shift), in.last(length - shift));
  copy(out.subspan(length - shift, shift), in.first(shift));
}

} // namespace srsvec
} // namespace srsran