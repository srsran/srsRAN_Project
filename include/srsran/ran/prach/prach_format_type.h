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
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief PRACH preamble formats.
///
/// PRACH preamble formats are described in TS38.211 Table 6.3.3.1-1 and 6.3.3.1-2.
enum class prach_format_type : uint8_t {
  zero = 0,
  one,
  two,
  three,
  A1,
  A2,
  A3,
  B1,
  B4,
  C0,
  C2,
  A1_B1,
  A2_B2,
  A3_B3,
  invalid
};

inline constexpr const char* to_string(prach_format_type format)
{
  switch (format) {
    case prach_format_type::zero:
      return "0";
    case prach_format_type::one:
      return "1";
    case prach_format_type::two:
      return "2";
    case prach_format_type::three:
      return "3";
    case prach_format_type::A1:
      return "A1";
    case prach_format_type::A1_B1:
      return "A1/B1";
    case prach_format_type::A2:
      return "A2";
    case prach_format_type::A2_B2:
      return "A2/B2";
    case prach_format_type::A3:
      return "A3";
    case prach_format_type::A3_B3:
      return "A3/B3";
    case prach_format_type::B1:
      return "B1";
    case prach_format_type::B4:
      return "B4";
    case prach_format_type::C0:
      return "C0";
    case prach_format_type::C2:
      return "C2";
    default:
      srsran_assert(0, "Invalid PRACH format={}", format);
      break;
  }
  return "";
}

inline prach_format_type to_prach_format_type(const char* string)
{
  using int_type = std::underlying_type_t<prach_format_type>;
  for (int_type i_format     = static_cast<int_type>(prach_format_type::zero),
                i_format_end = static_cast<int_type>(prach_format_type::invalid);
       i_format != i_format_end;
       ++i_format) {
    prach_format_type format = static_cast<prach_format_type>(i_format);
    if (strcmp(string, to_string(format)) == 0) {
      return format;
    }
  }

  return prach_format_type::invalid;
}

/// \brief Checks if the preamble format is long.
///
/// Long preambles are listed in TS38.211 Table 6.3.3.1-1.
constexpr bool is_long_preamble(prach_format_type type)
{
  return (type <= prach_format_type::three);
}

/// \brief Checks if the preamble format is short.
///
/// Short preambles are listed in TS38.211 Table 6.3.3.1-2.
constexpr bool is_short_preamble(prach_format_type type)
{
  return (type < prach_format_type::invalid) && (type >= prach_format_type::A1);
}

/// \brief Gets the PRACH time-domain occasion duration in symbols relative to the RA subcarrier spacing.
///
/// Corresponds to parameter \f$N_\textup{dur}^\textup{RA}\f$ is TS38.211 Section 5.3.2. It is selected from TS38.211
/// Tables 6.3.3.2-2, 6.3.3.2-3 and 6.3.3.2-4.
///
/// \param[in] format PRACH preamble format.
/// \return The PRACH time-domain occasion duration for short PRACH formats, and 0 otherwise.
inline unsigned get_preamble_duration(prach_format_type format)
{
  switch (format) {
    case prach_format_type::zero:
    case prach_format_type::one:
    case prach_format_type::two:
    case prach_format_type::three:
    case prach_format_type::invalid:
    default:
      return 0;
    case prach_format_type::A1:
    case prach_format_type::B1:
    case prach_format_type::A1_B1:
    case prach_format_type::C0:
      return 2;
    case prach_format_type::A2:
    case prach_format_type::A2_B2:
      return 4;
    case prach_format_type::A3:
    case prach_format_type::A3_B3:
    case prach_format_type::C2:
      return 6;
    case prach_format_type::B4:
      return 12;
  }
}

} // namespace srsran
