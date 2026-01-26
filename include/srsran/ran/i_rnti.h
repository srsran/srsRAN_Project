/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "srsran/adt/expected.h"
#include "fmt/format.h"
#include <cstdint>

namespace srsran {

/// \brief Common type for Full-I-RNTI (40 Bit) as specified in TS 38.331 section 6.3.2 and TS 38.300 Annex F.
/// \remark The I-RNTI is composed of an I-RNTI profile, a gNB ID and a UE ID. The lengths of the gNB ID and UE ID are
/// defined by the used I-RNTI profile. The UE ID part of I-RNTI should not be derived from gNB UE ID to avoid the
/// possibility of UE tracking as specified in TS 33.501 section 6.8.2.1.2.
/// Note: Currently only Full-I-RNTI profile 0 is supported, the number of UE ID bits is configurable.
class full_i_rnti_t
{
public:
  /// Creates an Full-I-RNTI from a gNB ID and UE ID.
  /// \param[in] gnb_id_ gNB ID.
  /// \param[in] ue_id_ UE ID.
  /// \param[in] nof_ue_bits_ Number of bits used for the UE ID.
  constexpr full_i_rnti_t(uint32_t gnb_id_, uint32_t ue_id_, uint8_t nof_ue_bits_)
  {
    val = static_cast<uint64_t>(gnb_id_ << nof_ue_bits_) + (ue_id_ & ((static_cast<uint16_t>(1) << nof_ue_bits_) - 1));

    max_val = static_cast<uint64_t>(gnb_id_ << nof_ue_bits_) + (static_cast<uint32_t>(1 << nof_ue_bits_) - 1);

    min_val = gnb_id_ << nof_ue_bits_;
  }

  /// Returns the maximum Full-I-RNTI value.
  constexpr uint64_t max() const { return max_val; }

  /// Returns the minimum Full-I-RNTI value.
  constexpr uint32_t min() const { return min_val; }

  /// Creates an Full-I-RNTI from an Full-I-RNTI integer representation.
  /// \param[in] value Integer representation of the Full-I-RNTI.
  /// \param[in] nof_ue_bits_ Number of bits used for the UE ID.
  /// \return The created Full-I-RNTI or an error if the value is invalid.
  static expected<full_i_rnti_t> from_uint(uint64_t value, uint8_t nof_ue_bits_)
  {
    if (value > 0x3fffffffff) {
      return make_unexpected(default_error_t{});
    }

    auto gnb_id_val = static_cast<uint32_t>(value >> nof_ue_bits_);
    auto ue_id_val  = static_cast<uint32_t>(value - (gnb_id_val << nof_ue_bits_));
    return full_i_rnti_t{gnb_id_val, ue_id_val, nof_ue_bits_};
  }

  /// Returns the Full-I-RNTI value.
  uint64_t value() const { return val; }

  bool operator==(const full_i_rnti_t& i_rnti) const { return val == i_rnti.val; }
  bool operator!=(const full_i_rnti_t& i_rnti) const { return val != i_rnti.val; }
  bool operator<(const full_i_rnti_t& i_rnti) const { return val < i_rnti.val; }
  bool operator<=(const full_i_rnti_t& i_rnti) const { return val <= i_rnti.val; }
  bool operator>(const full_i_rnti_t& i_rnti) const { return val > i_rnti.val; }
  bool operator>=(const full_i_rnti_t& i_rnti) const { return val >= i_rnti.val; }

private:
  uint64_t val     = 0;
  uint64_t max_val = 0;
  uint32_t min_val = 0;
};

/// \brief Common type for Short-I-RNTI (20 Bit) as specified in TS 38.331 section 6.3.2 and TS 38.300 Annex F.
/// \remark The I-RNTI is composed of an I-RNTI profile, a gNB ID and a UE ID. The lengths of the gNB ID and UE ID are
/// defined by the used I-RNTI profile. The UE ID part of I-RNTI should not be derived from gNB UE ID to avoid the
/// possibility of UE tracking as specified in TS 33.501 section 6.8.2.1.2.
/// Note: Currently only Short-I-RNTI profile 0 is supported, the number of UE ID bits is configurable.
class short_i_rnti_t
{
public:
  /// Creates an Short-I-RNTI from a gNB ID and UE ID.
  /// \param[in] gnb_id_ gNB ID.
  /// \param[in] ue_id_ UE ID.
  /// \param[in] nof_ue_bits_ Number of bits used for the UE ID.
  constexpr short_i_rnti_t(uint32_t gnb_id_, uint32_t ue_id_, uint8_t nof_ue_bits_)
  {
    val = static_cast<uint32_t>(gnb_id_ << nof_ue_bits_) + (ue_id_ & ((static_cast<uint16_t>(1) << nof_ue_bits_) - 1));

    max_val =
        (static_cast<uint32_t>(gnb_id_ << nof_ue_bits_) + (static_cast<uint32_t>(1 << nof_ue_bits_) - 1)) & 0x7ffff;

    min_val = (gnb_id_ << nof_ue_bits_) & 0x7ffff;
  }

  /// Returns the maximum Short-I-RNTI value.
  constexpr uint32_t max() const { return max_val; }

  /// Returns the minimum Short-I-RNTI value.
  constexpr uint32_t min() const { return min_val; }

  /// Creates an Short-I-RNTI from an Short-I-RNTI integer representation.
  /// \param[in] value Integer representation of the Short-I-RNTI.
  /// \param[in] nof_ue_bits_ Number of bits used for the UE ID.
  static expected<short_i_rnti_t> from_uint(uint32_t value, uint8_t nof_ue_bits_)
  {
    if (value > 0x7ffff) {
      return make_unexpected(default_error_t{});
    }

    auto gnb_id_val = static_cast<uint32_t>(value >> nof_ue_bits_);
    auto ue_id_val  = static_cast<uint32_t>(value - (gnb_id_val << nof_ue_bits_));
    return short_i_rnti_t{gnb_id_val, ue_id_val, nof_ue_bits_};
  }

  /// Returns the Short-I-RNTI value.
  uint32_t value() const { return val; }

  bool operator==(const short_i_rnti_t& i_rnti) const { return val == i_rnti.val; }
  bool operator!=(const short_i_rnti_t& i_rnti) const { return val != i_rnti.val; }
  bool operator<(const short_i_rnti_t& i_rnti) const { return val < i_rnti.val; }
  bool operator<=(const short_i_rnti_t& i_rnti) const { return val <= i_rnti.val; }
  bool operator>(const short_i_rnti_t& i_rnti) const { return val > i_rnti.val; }
  bool operator>=(const short_i_rnti_t& i_rnti) const { return val >= i_rnti.val; }

private:
  uint32_t val     = 0;
  uint32_t max_val = 0;
  uint32_t min_val = 0;
};

} // namespace srsran

// Formatters
namespace fmt {

template <>
struct formatter<srsran::full_i_rnti_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::full_i_rnti_t i_rnti, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "full-i-rnti={:#x}", i_rnti.value());
  }
};

template <>
struct formatter<srsran::short_i_rnti_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::short_i_rnti_t i_rnti, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "short-i-rnti={:#x}", i_rnti.value());
  }
};

} // namespace fmt
