/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Modulation schemes and orders.

#pragma once

#include <string>

namespace srsgnb {

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
  QAM256 = 8
};

inline std::string to_string(modulation_scheme mod)
{
  switch (mod) {
    case modulation_scheme::PI_2_BPSK:
      return "π/2-BPSK";
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

/// \brief Bits per symbol for a given modulation scheme.
/// \param[in] mod Modulation scheme.
/// \returns The number of bits per modulated symbol (sometimes referred to as modulation order).
inline constexpr unsigned get_bits_per_symbol(modulation_scheme mod)
{
  return std::max(1U, static_cast<unsigned>(mod));
}

/// Defines the maximum bits per symbol.
static constexpr unsigned MODULATION_MAX_BITS_PER_SYMBOL = get_bits_per_symbol(modulation_scheme::QAM256);

} // namespace srsgnb
