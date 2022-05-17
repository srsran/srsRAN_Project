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

#ifndef SRSGNB_PHY_MODULATION_SCHEME_H
#define SRSGNB_PHY_MODULATION_SCHEME_H

namespace srsgnb {

/// \brief Modulation schemes as described in TS38.211 Section 5.1.
///
/// Each modulation identifier is mapped to the corresponding modulation order.
enum class modulation_scheme {
  /// Binary Phase Shift Keying (BPSK) modulation.
  BPSK = 1,
  /// Quadrature Phase Shift Keying (QPSK) modulation.
  QPSK = 2,
  /// 16-point Quadrature Amplitude Modulation (16-QAM).
  QAM16 = 4,
  /// 64-point Quadrature Amplitude Modulation (64-QAM).
  QAM64 = 6,
  /// 256-point Quadrature Amplitude Modulation (256-QAM).
  QAM256 = 8
};

/// \brief Bits per symbol for a given modulation scheme.
/// \param[in] mod Modulation scheme.
/// \returns The number of bits per modulated symbol (sometimes referred to as modulation order).
inline constexpr unsigned get_bits_per_symbol(modulation_scheme mod)
{
  return static_cast<unsigned>(mod);
}

} // namespace srsgnb

#endif // SRSGNB_PHY_MODULATION_SCHEME_H
