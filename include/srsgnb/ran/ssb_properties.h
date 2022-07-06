/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_SSB_PROPERTIES_H
#define SRSGNB_RAN_SSB_PROPERTIES_H

#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// \brief SSB periodicity in milliseconds as per TS38.331 Section 6.3.2 IE ssb-periodicityServingCell.
enum class ssb_periodicity { ms5 = 5, ms10 = 10, ms20 = 20, ms40 = 40, ms80 = 80, ms160 = 160 };

/// \brief Converts the SSB periodicity property to its corresponding value in milliseconds.
inline unsigned ssb_periodicity_to_value(ssb_periodicity periodicity)
{
  return static_cast<unsigned>(periodicity);
}

/// \brief PSS power level allocation in dB, relative to SSS as per TS38.213 Section 4.1.
enum class ssb_beta_pss { dB_0, dB_3 };

/// Describes the different SS/PBCH block patterns defined in 38.213 section 4.1 Cell search.
enum class ssb_pattern_case {
  /// Case A - 15 kHz SCS, FR1.
  A,
  /// Case B - 30 kHz SCS, FR1.
  B,
  /// Case C - 30 kHz SCS, FR1.
  C,
  /// Case D - 120 kHz SCS, FR2.
  D,
  /// Case E - 240 kHz SCS, FR2.
  E,
  /// Invalid case.
  invalid
};

/// Convert a SSB pattern case to subcarrier spacing.
inline subcarrier_spacing to_subcarrier_spacing(ssb_pattern_case pattern_case)
{
  srsran_assert(pattern_case != ssb_pattern_case::invalid, "Invalid pattern case.");

  switch (pattern_case) {
    case ssb_pattern_case::A:
      return subcarrier_spacing::kHz15;
    case ssb_pattern_case::B:
    case ssb_pattern_case::C:
      return subcarrier_spacing::kHz30;
    case ssb_pattern_case::D:
      return subcarrier_spacing::kHz120;
    case ssb_pattern_case::E:
    case ssb_pattern_case::invalid:
      break;
  }
  return subcarrier_spacing::kHz240;
}

/// Convert a SSB pattern case to frequency range.
inline frequency_range to_frequency_range(ssb_pattern_case pattern_case)
{
  srsran_assert(pattern_case != ssb_pattern_case::invalid, "Invalid pattern case.");

  if (pattern_case < ssb_pattern_case::D) {
    return frequency_range::FR1;
  }
  return frequency_range::FR2;
}

/// \brief Data type used to represent the frequency offset between point A and the lowest subcarrier of the lowest
/// common resource block that overlaps with the SS/PBCH block.
///
/// Parameter \c offsetToPointA as per TS38.211 Section 4.4.4.2.
///
/// Read [this page](https://www.sharetechnote.com/html/5G/5G_ResourceBlockIndexing.html) and [this
/// page](http://nrexplained.com/rbs) for a more detailed explanation of resource block indexing .
///
/// Used as \f$N_{CRB}^{SSB}\f$ in TS38.211 Section 7.4.3.1.
///
/// The range is {0, ..., 2199} as per TS38.331 \c FrequencyInfoDL-SIB.
///
/// It is expressed in units of resource blocks assuming:
/// - 15kHz SCS for FR1, and
/// - 60kHz SCS for FR2.
///
/// \sa ssb_subcarrier_offset
using ssb_offset_to_pointA = uint16_t;

/// \brief Data type used to represent the offset from subcarrier 0 in common resource block \f$N_{CRB}^{SSB}\f$ to
/// subcarrier zero of the SS/PBCH block.
///
/// Parameter \f$ k_{SSB} \f$ as per TS38.211 Section 7.4.3.1. It is expressed in subcarriers of the same SCS than the
/// SS/PBCH block.
///
/// Read [this page](https://www.sharetechnote.com/html/5G/5G_ResourceBlockIndexing.html) and [this
/// page](http://nrexplained.com/rbs) for a more detailed explanation of resource block indexing .
///
/// For numerology \f$\mu\in\{0,1\}\f$ the field is 5-bit wide, the range is {0, ..., 23}.
///
/// For numerology \f$\mu\in\{3,4\}\f$ the field is 4-bit wide, the range is {0, ..., 11}.
///
/// The four least significant bits bits are given by the higher layer parameter \c ssb-SubcarrierOffset as per TS38.331
/// \c MIB. If the field is 5-bit wide, the MSB is given by bit \f$\bar{a}_{\bar{A}+5}\f$ in the PBCH payload as per
/// TS38.212 Section 7.1.1.
///
/// \sa ssb_offset_to_pointA
using ssb_subcarrier_offset = uint8_t;

} // namespace srsgnb

#endif // SRSGNB_RAN_SSB_PROPERTIES_H
