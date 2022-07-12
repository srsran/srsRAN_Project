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

#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// \brief SSB periodicity in milliseconds as per TS38.331 Section 6.3.2 IE ssb-periodicityServingCell.
enum class ssb_periodicity { ms5 = 5, ms10 = 10, ms20 = 20, ms40 = 40, ms80 = 80, ms160 = 160 };

/// \brief Converts the SSB periodicity property to its corresponding value in milliseconds.
inline unsigned ssb_periodicity_to_value(ssb_periodicity periodicity)
{
  return static_cast<unsigned>(periodicity);
}

/// \brief PSS EPRE to SSS EPRE for SSB, as per TS 38.213, Section 4.1.
enum class ssb_pss_to_sss_epre { dB_0, dB_3 };

/// Labels for the different SS/PBCH block patterns defined in TS38.213 Section 4.1.
enum class ssb_pattern_case {
  /// Case A - 15 kHz SCS, FR1: index pattern \f$\{2, 8\} + 14\cdot n\f$.
  A,
  /// Case B - 30 kHz SCS, FR1: index pattern \f$\{4, 8, 16, 20\} + 28\cdot n\f$.
  B,
  /// Case C - 30 kHz SCS, FR1: index pattern \f$\{2, 8\} + 14\cdot n\f$.
  C,
  /// Case D - 120 kHz SCS, FR2: index pattern \f$\{4, 8, 16, 20\} + 28\cdot n\f$.
  D,
  /// Case E - 240 kHz SCS, FR2: index pattern \f$\{8, 12, 16, 20, 32, 36, 40, 44 \} + 56\cdot n\f$.
  E,
  /// Invalid case.
  invalid
};

/// Returns the subcarrier spacing corresponding to an SS/PBCH block pattern.
inline subcarrier_spacing to_subcarrier_spacing(ssb_pattern_case pattern_case)
{
  switch (pattern_case) {
    case ssb_pattern_case::A:
      return subcarrier_spacing::kHz15;
    case ssb_pattern_case::B:
    case ssb_pattern_case::C:
      return subcarrier_spacing::kHz30;
    case ssb_pattern_case::D:
      return subcarrier_spacing::kHz120;
    case ssb_pattern_case::E:
      return subcarrier_spacing::kHz240;
    case ssb_pattern_case::invalid:
    default:
      return subcarrier_spacing::invalid;
  }
}

/// Returns the frequency range corresponding to an SS/PBCH block pattern.
inline frequency_range to_frequency_range(ssb_pattern_case pattern_case)
{
  srsran_assert(pattern_case != ssb_pattern_case::invalid, "Invalid pattern case.");

  if (pattern_case < ssb_pattern_case::D) {
    return frequency_range::FR1;
  }
  return frequency_range::FR2;
}

/// \brief Data type used to represent the frequency offset between Point A and the lowest subcarrier of the lowest
/// common resource block that overlaps with the SS/PBCH block.
///
/// The quantity expressed in terms of this data type corresponds to \e offsetToPointA in TS38.211 Section 4.4.4.2 and
/// it is used in TS38.211 Section 7.4.3.1 to compute \f$N_{CRB}^{SSB}\f$.
/// It is measured as a number of resource blocks, assuming:
/// - 15kHz SCS for FR1, and
/// - 60kHz SCS for FR2.
///
/// The range of values is {0, ..., 2199} &mdash; see TS38.331 Section 6.3.2, Information Element \e
/// FrequencyInfoDL-SIB.
///
/// The reader is referred to [this page](https://www.sharetechnote.com/html/5G/5G_ResourceBlockIndexing.html) and [this
/// page](http://nrexplained.com/rbs) for more details about resource block indexing.
///
/// \sa ssb_subcarrier_offset
class ssb_offset_to_pointA
{
private:
  /// Data type for the value.
  using type = uint16_t;
  /// Maximum possible value.
  static constexpr type MAX_VALUE = 2199;
  /// Actual value.
  type value;

public:
  /// Returns the maximum value.
  static constexpr unsigned max() { return static_cast<unsigned>(MAX_VALUE); }

  /// Default constructor: sets an invalid value.
  ssb_offset_to_pointA() : value(MAX_VALUE + 1) {}

  /// Constructs the object from a value.
  template <typename T>
  ssb_offset_to_pointA(T value_) : value(static_cast<type>(value_))
  {
    static_assert(std::is_convertible<T, type>::value && std::is_literal_type<T>::value, "Invalid value type.");
  }

  /// Copy constructor.
  ssb_offset_to_pointA(const ssb_offset_to_pointA& other) : value(other.value) {}

  /// Returns true if the value is within the range.
  bool is_valid() const { return value <= max(); }

  /// Gets the value as an unsigned integer.
  unsigned to_uint() const { return static_cast<unsigned>(value); }
};

/// \brief Data type used to represent the offset from subcarrier zero in common resource block \f$N_{CRB}^{SSB}\f$ to
/// subcarrier zero of the SS/PBCH block.
///
/// The quantity expressed in terms of this data type corresponds to \f$ k_{SSB} \f$ in TS38.211 Section 7.4.3.1. It is
/// measured as a number of subcarriers of the same spacing as the SS/PBCH block.
///
/// For numerology \f$\mu\in\{0,1\}\f$:
/// - the range is {0, ..., 23}, and
/// - the reference SS/PBCH subcarrier spacing is 15 kHz.
///
/// For numerology \f$\mu\in\{3,4\}\f$:
/// - the range is {0, ..., 11}, and
/// - the reference SS/PBCH subcarrier spacing is provided by the higher layers parameter \e subCarrierSpacingCommon
/// (see TS38.331 Section 6.2.2, Information Element \e MIB).
///
/// The reader is referred to [this page](https://www.sharetechnote.com/html/5G/5G_ResourceBlockIndexing.html) and [this
/// page](http://nrexplained.com/rbs) for a more details about resource block indexing.
///
/// \sa ssb_offset_to_pointA
class ssb_subcarrier_offset
{
private:
  /// Data type for the value.
  using type = uint8_t;
  /// Maximum possible value for FR1.
  static constexpr type MAX_VALUE_FR1 = 23;
  /// Maximum possible value for FR2.
  static constexpr type MAX_VALUE_FR2 = 11;
  /// Actual value.
  type value;

public:
  /// Returns the maximum value for the given frequency range.
  static constexpr unsigned max(frequency_range fr)
  {
    return static_cast<unsigned>((fr == frequency_range::FR1) ? MAX_VALUE_FR1 : MAX_VALUE_FR2);
  }

  /// Default constructor: sets an invalid value.
  ssb_subcarrier_offset() : value(MAX_VALUE_FR1 + 1) {}

  /// Constructs the object from a value.
  template <typename T>
  ssb_subcarrier_offset(T value_) : value(static_cast<type>(value_))
  {
    static_assert(std::is_convertible<T, type>::value, "Invalid value type.");
  }

  /// Returns true if the value is within the range.
  bool is_valid(frequency_range fr) const { return value <= max(fr); }

  /// Gets the value as an unsigned integer.
  unsigned to_uint() const { return static_cast<unsigned>(value); }
};

} // namespace srsgnb
