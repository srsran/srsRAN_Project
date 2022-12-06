/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsgnb/srsvec/detail/traits.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <numeric>

namespace srsgnb {
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

  srsgnb_assert(!x.empty(), "The input span cannot be empty.");

  return std::accumulate(x.begin(), x.end(), static_cast<MeanType>(0)) / static_cast<MeanType>(x.size());
}

} // namespace srsvec
} // namespace srsgnb
