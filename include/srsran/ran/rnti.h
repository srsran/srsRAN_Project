/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/support/srsran_assert.h"
#include "fmt/format.h"
#include <cstdint>
#include <type_traits>

namespace srsran {

/// \remark See TS 38.331 - RNTI-Value and TS 38.321, Table 7.1-1: RNTI Values. Values: (0..65535)
enum class rnti_t : uint16_t {
  INVALID_RNTI = 0x0,
  MIN_CRNTI    = 0x1,
  // ...
  MAX_CRNTI         = 0xffef,
  MIN_RESERVED_RNTI = 0xfff0,
  // ...
  MAX_RESERVED_RNTI = 0xfffd,
  P_RNTI            = 0xfffe,
  SI_RNTI           = 0xffff
};

/// Checks whether RNTI value corresponds to a C-RNTI value.
constexpr bool is_crnti(rnti_t rnti)
{
  return rnti <= rnti_t::MAX_CRNTI and rnti >= rnti_t::MIN_CRNTI;
}

/// Converts integer to RNTI value.
constexpr rnti_t to_rnti(std::underlying_type_t<rnti_t> number)
{
  return static_cast<rnti_t>(number);
}

/// Converts RNTI value to integer.
constexpr uint16_t to_value(rnti_t rnti)
{
  return static_cast<uint16_t>(rnti);
}

/// \brief Common type for Full-I-RNTI (40 Bit) and Short-I-RNTI (20 Bit) as specified in TS 38.331 section 6.3.2 and
/// TS 38.300 Annex F.
/// \remark The I-RNTI is composed of an I-RNTI profile, a gNB ID and a UE ID. The lengths of the gNB ID and UE ID are
/// defined by the used I-RNTI profile.
/// Note: Currently only Short/Full-I-RNTI Profile 0 is supported, the number of UE ID bits is configurable.
class i_rnti_t
{
  constexpr i_rnti_t(uint32_t gnb_id_, uint32_t ue_id_, uint8_t nof_ue_bits_, bool is_full_ = true)
  {
    val = static_cast<uint64_t>(gnb_id_ << nof_ue_bits_) + (ue_id_ & ((static_cast<uint16_t>(1) << nof_ue_bits_) - 1));

    max_full = static_cast<uint64_t>(gnb_id_ << nof_ue_bits_) + (static_cast<uint32_t>(1 << nof_ue_bits_) - 1);

    min_i_rnti = gnb_id_ << nof_ue_bits_;

    is_full = is_full_;
  }

public:
  /// Returns the maximum Full-I-RNTI value.
  constexpr uint64_t max_full_i_rnti() const { return max_full; }

  /// Returns the maximum Short-I-RNTI value.
  constexpr uint32_t max_short_i_rnti() const { return max_full & 0x7ffff; }

  /// Returns the minimum Short/Full-I-RNTI value.
  constexpr uint32_t min() const { return min_i_rnti; }

  i_rnti_t() = default;

  /// Creates an I-RNTI from a gNB ID and UE ID.
  /// \param[in] gnb_id_ gNB ID.
  /// \param[in] ue_id_ UE ID.
  /// \param[in] nof_ue_bits_ Number of bits used for the UE ID.
  static i_rnti_t create(uint32_t gnb_id_, uint32_t ue_id_, uint8_t nof_ue_bits_)
  {
    return i_rnti_t{gnb_id_, ue_id_, nof_ue_bits_};
  }

  /// Creates an I-RNTI from an Short-I-RNTI integer representation.
  /// \param[in] value Integer representation of the Short-I-RNTI.
  /// \param[in] nof_ue_bits_ Number of bits used for the UE ID.
  static expected<i_rnti_t> from_uint(uint32_t value, uint8_t nof_ue_bits_)
  {
    return from_uint(static_cast<uint64_t>(value), nof_ue_bits_, false);
  }

  /// Creates an I-RNTI from an I-RNTI integer representation.
  /// \param[in] value Integer representation of the I-RNTI.
  /// \param[in] nof_ue_bits_ Number of bits used for the UE ID.
  /// \param[in] is_full_ Whether the I-RNTI is a Full-I-RNTI.
  /// \return The created I-RNTI or an error if the value is invalid.
  static expected<i_rnti_t> from_uint(uint64_t value, uint8_t nof_ue_bits_, bool is_full_)
  {
    if (is_full_) {
      if (value > 0x3fffffffff) {
        return make_unexpected(default_error_t{});
      }

      auto gnb_id_val = static_cast<uint32_t>(value >> nof_ue_bits_);
      auto ue_id_val  = static_cast<uint32_t>(value - (gnb_id_val << nof_ue_bits_));
      return i_rnti_t{gnb_id_val, ue_id_val, nof_ue_bits_};
    }

    if (value > 0x7ffff) {
      return make_unexpected(default_error_t{});
    }

    auto gnb_id_val = static_cast<uint32_t>(value >> nof_ue_bits_);
    auto ue_id_val  = static_cast<uint32_t>(value - (gnb_id_val << nof_ue_bits_));
    return i_rnti_t{gnb_id_val, ue_id_val, nof_ue_bits_, false};
  }

  /// Returns the Full-I-RNTI value.
  expected<uint64_t> full_i_rnti() const
  {
    if (!is_full) {
      return make_unexpected(default_error_t{});
    }
    return (val & 0x3fffffffff);
  }

  /// Returns the Short-I-RNTI value.
  uint32_t short_i_rnti() const { return static_cast<uint32_t>(val & 0x7ffff); }

  bool operator==(const i_rnti_t& i_rnti) const
  {
    if (is_full & i_rnti.is_full) {
      return val == i_rnti.val;
    }
    return short_i_rnti() == i_rnti.short_i_rnti();
  }
  bool operator!=(const i_rnti_t& i_rnti) const
  {
    if (is_full & i_rnti.is_full) {
      return val != i_rnti.val;
    }
    return short_i_rnti() != i_rnti.short_i_rnti();
  }
  bool operator<(const i_rnti_t& i_rnti) const
  {
    if (is_full & i_rnti.is_full) {
      return val < i_rnti.val;
    }
    return short_i_rnti() < i_rnti.short_i_rnti();
  }
  bool operator<=(const i_rnti_t& i_rnti) const
  {
    if (is_full & i_rnti.is_full) {
      return val <= i_rnti.val;
    }
    return short_i_rnti() <= i_rnti.short_i_rnti();
  }
  bool operator>(const i_rnti_t& i_rnti) const
  {
    if (is_full & i_rnti.is_full) {
      return val > i_rnti.val;
    }
    return short_i_rnti() > i_rnti.short_i_rnti();
  }
  bool operator>=(const i_rnti_t& i_rnti) const
  {
    if (is_full & i_rnti.is_full) {
      return val >= i_rnti.val;
    }
    return short_i_rnti() >= i_rnti.short_i_rnti();
  }

private:
  uint64_t val        = 0;
  uint64_t max_full   = 0;
  uint32_t min_i_rnti = 0;
  bool     is_full    = true;
};

} // namespace srsran

// Formatters
namespace fmt {
template <>
struct formatter<srsran::rnti_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rnti_t rnti, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{:#x}", to_value(rnti));
  }
};

template <>
struct formatter<srsran::i_rnti_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::i_rnti_t i_rnti, FormatContext& ctx) const
  {
    if (!i_rnti.full_i_rnti().has_value()) {
      return format_to(ctx.out(), "short-i-rnti={:#x}", i_rnti.short_i_rnti());
    }

    return format_to(
        ctx.out(), "short-i-rnti={:#x} full-i-rnti={:#x}", i_rnti.short_i_rnti(), i_rnti.full_i_rnti().value());
  }
};

} // namespace fmt
