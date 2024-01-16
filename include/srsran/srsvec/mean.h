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

#include "srsran/adt/complex.h"
#include "srsran/srsvec/detail/traits.h"
#include "srsran/support/srsran_assert.h"
#include <numeric>

namespace srsran {
namespace srsvec {

/// \brief Computes the mean of the values in the given sequence.
///
/// \tparam T Type of the input sequence, compatible with a span of arithmetic or cf_t typed values.
/// \param  x Sequence of values to compute the mean of.
/// \return   The mean value of the given sequence.
/// \remark For sequences of floating-point (including complex) values, the output type is the same as the value type
/// of the input. For integer-valued sequences, the output type is \c float.
template <typename T>
auto mean(const T& x)
{
  static_assert(detail::is_arithmetic_span_compatible<T>::value || detail::is_complex_span_compatible<T>::value,
                "The input type is not compatible with a span of arithmetic/cf_t values.");

  using DataType = typename T::value_type;

  using MeanType1 = typename std::conditional<is_complex<typename T::value_type>::value, DataType, float>::type;
  using MeanType  = typename std::conditional<std::is_floating_point<DataType>::value, DataType, MeanType1>::type;

  srsran_assert(!x.empty(), "The input span cannot be empty.");

  return std::accumulate(x.begin(), x.end(), static_cast<MeanType>(0)) / static_cast<MeanType>(x.size());
}

} // namespace srsvec
} // namespace srsran
