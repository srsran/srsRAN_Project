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

#pragma once

namespace srsran {

/// The IE Alpha defines possible values of a the pathloss compensation coefficient for uplink power control. Value
/// alpha0 corresponds to the value 0, Value alpha04 corresponds to the value 0.4, and so on. Value alpha1 corresponds
/// to value 1.
/// \remark See TS 38.331, Alpha. Also, see clause 7.1 of TS 38.213.
enum class alpha : unsigned { alpha0, alpha04, alpha05, alpha06, alpha07, alpha08, alpha09, alpha1, not_set };

/// \brief Converts an alpha value to a float.
inline float alpha_to_float(alpha a)
{
  switch (a) {
    case alpha::alpha0:
      return 0.0f;
    case alpha::alpha04:
      return 0.4f;
    case alpha::alpha05:
      return 0.5f;
    case alpha::alpha06:
      return 0.6f;
    case alpha::alpha07:
      return 0.7f;
    case alpha::alpha08:
      return 0.8f;
    case alpha::alpha09:
      return 0.9f;
    case alpha::alpha1:
      return 1.0f;
    case alpha::not_set:
    default:
      return 0.0f;
  }
  return 0.0f;
}

/// \brief Converts an alpha value to a float.
inline alpha float_to_alpha(float alpha_float)
{
  switch (static_cast<int>(alpha_float * 10)) {
    case 0:
      return alpha::alpha0;
    case 4:
      return alpha::alpha04;
    case 5:
      return alpha::alpha05;
    case 6:
      return alpha::alpha06;
    case 7:
      return alpha::alpha07;
    case 8:
      return alpha::alpha08;
    case 9:
      return alpha::alpha09;
    case 10:
      return alpha::alpha1;
    default:
      return alpha::not_set;
  }
}

} // namespace srsran
