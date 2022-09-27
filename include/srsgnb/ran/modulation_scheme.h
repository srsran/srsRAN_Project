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

#include <array>
#include <string>

namespace srsgnb {

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
  QAM256 = MODULATION_MAX_BITS_PER_SYMBOL,
  /// Invalid modulation scheme.
  INVALID = INT32_MAX
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
      return "256QAM";
    case modulation_scheme::INVALID:
    default:
      return "invalid";
  }
}

/// \brief Converts a string into a modulation scheme.
/// \param[in] mod_scheme_string input string.
/// \returns The corresponding \c modulation_scheme. If the input string does not correspond with any valid option, it
/// returns \c modulation_scheme::INVALID
inline modulation_scheme from_string(const std::string& mod_scheme_string)
{
  modulation_scheme mod_scheme = modulation_scheme::INVALID;
  if (mod_scheme_string == "π/2-BPSK") {
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
  }
  return mod_scheme;
}

/// \brief Bits per symbol for a given modulation scheme.
/// \param[in] mod Modulation scheme.
/// \returns The number of bits per modulated symbol (sometimes referred to as modulation order), or 0 if the modulation
/// scheme is invalid.
inline constexpr unsigned get_bits_per_symbol(modulation_scheme mod)
{
  unsigned ret = static_cast<unsigned>(mod);
  if (mod == modulation_scheme::PI_2_BPSK) {
    ret = 1;
  } else if (mod == modulation_scheme::INVALID) {
    ret = 0;
  }

  // Give a hint to the compiler that the returned bits per symbol will not be greater than 8 under any circumstance.
  if (ret > MODULATION_MAX_BITS_PER_SYMBOL) {
    __builtin_unreachable();
  }

  return ret;
}

/// \brief Checks if a modulation scheme is valid.
/// \param[in] mod Modulation scheme.
/// \returns \c true if the modulation scheme is valid, \c false otherwise.
inline bool is_valid(modulation_scheme mod)
{
  return !(mod == modulation_scheme::INVALID);
}

/// \brief Lists all the modulation schemes.
/// \returns A string containing a comma-separated list with the supported modulation schemes.
inline std::string list_all_modulation_schemes()
{
  std::string ret;
  for (modulation_scheme mod : {modulation_scheme::PI_2_BPSK,
                                modulation_scheme::BPSK,
                                modulation_scheme::QPSK,
                                modulation_scheme::QAM16,
                                modulation_scheme::QAM64,
                                modulation_scheme::QAM256}) {
    ret += to_string(mod);
    ret += ", ";
  }

  // Remove last comma and space.
  ret.erase(ret.size() - 2);
  return ret;
}

} // namespace srsgnb
