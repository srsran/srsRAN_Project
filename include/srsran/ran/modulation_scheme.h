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
/// \brief Modulation schemes and orders.

#pragma once

#include "srsran/support/srsran_assert.h"
#include <string>

namespace srsran {

/// Maximum number of bits per symbol.
static constexpr unsigned MODULATION_MAX_BITS_PER_SYMBOL = 8;

/// \brief Modulation schemes as described in TS38.211 Section 5.1.
///
/// Each modulation identifier is mapped to the corresponding modulation order.
enum class modulation_scheme {
  /// 90-degree shifting Binary Phase Shift Keying (π/2-BPSK) described in TS38.211 Section 5.1.1.
  PI_2_BPSK = 0,
  /// Binary Phase Shift Keying (BPSK) modulation described in TS38.211 Section 5.1.2.
  BPSK = 1,
  /// Quadrature Phase Shift Keying (QPSK) modulation described in TS38.211 Section 5.1.3.
  QPSK = 2,
  /// 16-point Quadrature Amplitude Modulation (16-QAM) described in TS38.211 Section 5.1.4.
  QAM16 = 4,
  /// 64-point Quadrature Amplitude Modulation (64-QAM) described in TS38.211 Section 5.1.5.
  QAM64 = 6,
  /// 256-point Quadrature Amplitude Modulation (256-QAM) described in TS38.211 Section 5.1.6.
  QAM256 = MODULATION_MAX_BITS_PER_SYMBOL
};

inline std::string to_string(modulation_scheme mod)
{
  switch (mod) {
    case modulation_scheme::PI_2_BPSK:
      return "pi/2-BPSK";
    case modulation_scheme::BPSK:
      return "BPSK";
    case modulation_scheme::QPSK:
      return "QPSK";
    case modulation_scheme::QAM16:
      return "16QAM";
    case modulation_scheme::QAM64:
      return "64QAM";
    case modulation_scheme::QAM256:
    default:
      return "256QAM";
  }
}

/// \brief Converts a string into a modulation scheme.
/// \param[in] mod_scheme_string input string.
/// \returns The corresponding \c modulation_scheme.
inline modulation_scheme modulation_scheme_from_string(const std::string& mod_scheme_string)
{
  modulation_scheme mod_scheme = modulation_scheme::QAM256;
  if (mod_scheme_string == "pi/2-BPSK") {
    mod_scheme = modulation_scheme::PI_2_BPSK;
  } else if (mod_scheme_string == "BPSK") {
    mod_scheme = modulation_scheme::BPSK;
  } else if (mod_scheme_string == "QPSK") {
    mod_scheme = modulation_scheme::QPSK;
  } else if (mod_scheme_string == "16QAM") {
    mod_scheme = modulation_scheme::QAM16;
  } else if (mod_scheme_string == "64QAM") {
    mod_scheme = modulation_scheme::QAM64;
  } else if (mod_scheme_string == "256QAM") {
    mod_scheme = modulation_scheme::QAM256;
  } else {
    srsran_assertion_failure("Invalid modulation scheme: {}", mod_scheme_string);
  }
  return mod_scheme;
}

/// \brief Bits per symbol for a given modulation scheme.
/// \param[in] mod Modulation scheme.
/// \returns The number of bits per modulated symbol (sometimes referred to as modulation order).
inline constexpr unsigned get_bits_per_symbol(modulation_scheme mod)
{
  if (mod == modulation_scheme::PI_2_BPSK) {
    return 1;
  }

  return static_cast<unsigned>(mod);
}

} // namespace srsran
