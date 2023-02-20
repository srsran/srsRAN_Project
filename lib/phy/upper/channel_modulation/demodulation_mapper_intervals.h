/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/math_utils.h"

namespace srsran {

/// \brief Helper function to calculate an interval index from floating point value.
/// \param[in] value          Input value.
/// \param[in] interval_width Interval width.
/// \param[in] nof_intervals  Number of intervals.
/// \return Resulting interval index.
inline unsigned compute_interval_idx(float value, float interval_width, unsigned nof_intervals)
{
  int nof_intervals_int = static_cast<int>(nof_intervals);
  int idx               = static_cast<int>(std::floor(value / interval_width)) + nof_intervals_int / 2;

  return clamp(idx, 0, nof_intervals_int - 1);
}

/// \brief Applies an interval function.
/// \tparam Table             Look-up table type. All tables mut be of the same type.
/// \param[in] value          Input value.
/// \param[in] rcp_noise      The noise reciprocal corresponding to the value.
/// \param[in] nof_intervals  Number of intervals.
/// \param[in] interval_width Interval width to quantify the interval index.
/// \param[in] slopes         Table with the slope of each interval.
/// \param[in] intercepts     Table with the interception points of each interval.
/// \return The results of the interval function.
/// \remark The number of intervals must be lower than or equal to \c Table size.
template <typename Table>
inline float interval_function(float        value,
                               float        rcp_noise,
                               float        interval_width,
                               unsigned     nof_intervals,
                               const Table& slopes,
                               const Table& intercepts)
{
  unsigned idx     = compute_interval_idx(value, interval_width, nof_intervals);
  float    l_value = slopes[idx] * value + intercepts[idx];
  l_value *= rcp_noise;
  return l_value;
}

} // namespace srsran
