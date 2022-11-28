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

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/adt/variant.h"
#include <cstdint>

namespace srsgnb {

/// \brief Implementation of an optional unsigned type; if it contains a value, this is within range {0,...,MaxValue}.
///
/// \remark This allows representing an optional unsigned without resorting to the \ref optional<> class.
///
/// \tparam T is the underlying type of the optional unsigned. It must be an integer and unsigned type.
/// \tparam MaxValue is the maximum (valid) value of the optional unsigned; for any value larger than MaxValue assigned
/// to uint_optional_t, the property has_value() will return false.
template <typename T, size_t MaxValue>
struct uint_optional_t {
  static_assert(std::is_integral<T>::value and std::is_unsigned<T>::value, "Integral and unsigned type required.");
  static_assert(static_cast<T>(MaxValue) < std::numeric_limits<T>::max(),
                "MaxValue must be smaller than the type limit.");
  /// Value of the unsigned optional.
  T value;

  /// Initialize by default as not present.
  uint_optional_t() : value{std::numeric_limits<T>::max()} {};

  explicit uint_optional_t(T init_value) : value{init_value} {};

  /// Tells whether the unsigned has to be considered present.
  bool has_value() const { return value <= static_cast<T>(MaxValue); }

  uint_optional_t& operator=(const uint_optional_t& other) = default;
  /// Assignment operator that allowing us to assign a value as a normal uint.
  uint_optional_t& operator=(T val)
  {
    value = val;
    return *this;
  };

  bool operator==(const uint_optional_t& rhs) const
  {
    return has_value() == rhs.has_value() and (has_value() ? value == rhs.value : true);
  }

  bool operator!=(const T& rhs) const { return !(rhs == *this); }
};

/// \c scaling values options, part of \c UCI-OnPUSCH, part of \c PUSCH-Config, as per TS 38.331.
enum class alpha_scaling_opt { f0p5, f0p65, f0p8, f1 };

/// \c BetaOffsets, as per TS 38.331. These values are mapped into floats as per Tables in Section 9.3, TS 38.213.
struct beta_offsets {
  /// \remark Apply to all members. For values larger than 31 mean, treat the parameter as "not present".
  static constexpr uint8_t max_beta_offset_value = 31;

  /// Values {0,...,15}, as per Table 9.3-1, TS 38.213.
  /// \remark As per \c BetaOffsets, TS 38.331, {16,...,31} are accepted but treated as "reserved".
  uint_optional_t<uint8_t, max_beta_offset_value> beta_offset_ack_idx_1;
  uint_optional_t<uint8_t, max_beta_offset_value> beta_offset_ack_idx_2;
  uint_optional_t<uint8_t, max_beta_offset_value> beta_offset_ack_idx_3;
  /// Values {0,...,18}, as per Table 9.3-2, TS 38.213.
  /// \remark As per \c BetaOffsets, TS 38.331, {19,...,31} are accepted but treated as "reserved".
  uint_optional_t<uint8_t, max_beta_offset_value> beta_offset_csi_p1_idx_1;
  uint_optional_t<uint8_t, max_beta_offset_value> beta_offset_csi_p1_idx_2;
  uint_optional_t<uint8_t, max_beta_offset_value> beta_offset_csi_p2_idx_1;
  uint_optional_t<uint8_t, max_beta_offset_value> beta_offset_csi_p2_idx_2;

  bool operator==(const beta_offsets& rhs) const
  {
    return beta_offset_ack_idx_1 == rhs.beta_offset_ack_idx_1 and beta_offset_ack_idx_2 == rhs.beta_offset_ack_idx_2 and
           beta_offset_ack_idx_3 == rhs.beta_offset_ack_idx_3 and
           beta_offset_csi_p1_idx_1 == rhs.beta_offset_csi_p1_idx_1 and
           beta_offset_csi_p1_idx_2 == rhs.beta_offset_csi_p1_idx_2 and
           beta_offset_csi_p2_idx_1 == rhs.beta_offset_csi_p2_idx_1 and
           beta_offset_csi_p2_idx_2 == rhs.beta_offset_csi_p2_idx_2;
  }
  bool operator!=(const beta_offsets& rhs) const { return !(rhs == *this); }
};

/// \c UCI-OnPUSCH, part of \c PUSCH-Config, as per TS 38.331.
struct uci_on_pusch {
  static constexpr size_t max_nof_beta_offsets = 4;

  using beta_offsets_semi_static = beta_offsets;
  using beta_offsets_dynamic     = static_vector<beta_offsets, max_nof_beta_offsets>;

  variant<beta_offsets_dynamic, beta_offsets_semi_static> beta_offsets_cfg;
  alpha_scaling_opt                                       scaling;

  bool operator==(const uci_on_pusch& rhs) const
  {
    return beta_offsets_cfg == rhs.beta_offsets_cfg and scaling == rhs.scaling;
  }

  bool operator!=(const uci_on_pusch& rhs) const { return !(rhs == *this); }
};

} // namespace srsgnb
