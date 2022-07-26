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

#include "srsgnb/ran/lcid.h"
#include "srsgnb/srslog/bundled/fmt/format.h"
#include "srsgnb/support/srsgnb_assert.h"

namespace srsgnb {

class lcid_ul_sch_t
{
  using underlying_type = std::underlying_type_t<lcid_t>;

public:
  /// 3GPP 38.321 v15.3.0, Table 6.2.1-2 - Values of LCID for UL-SCH Index
  enum options : underlying_type {
    /// CCCH of 64 bits
    CCCH_SIZE_64 = 0b000000,

    /// Identity of the logical channel
    LCID1 = 1,
    // ...
    LCID32 = 32,

    /// Reserved
    MIN_RESERVED = 33,
    MAX_RESERVED = 51,

    /// CCCH of 48 bits
    CCCH_SIZE_48 = 0b110100,

    BIT_RATE_QUERY = 0b110101,

    SE_PHR          = 0b111001, // Single Entry PHR
    CRNTI           = 0b111010,
    SHORT_TRUNC_BSR = 0b111011,
    LONG_TRUNC_BSR  = 0b111100,
    SHORT_BSR       = 0b111101,
    LONG_BSR        = 0b111110,
    PADDING         = 0b111111
  };

  lcid_ul_sch_t(underlying_type lcid_ = PADDING) : lcid_val(lcid_) { srsgnb_assert(lcid_ <= PADDING, "Invalid LCID"); }
  lcid_ul_sch_t& operator=(underlying_type lcid)
  {
    srsgnb_assert(lcid <= PADDING, "Invalid LCID");
    lcid_val = lcid;
    return *this;
  }

  explicit        operator underlying_type() const { return lcid_val; }
  underlying_type value() const { return lcid_val; }

  /// Whether LCID belongs to CCCH
  bool is_ccch() const { return (lcid_val == CCCH_SIZE_48 || lcid_val == CCCH_SIZE_64); }

  /// Whether LCID is an MAC CE
  bool is_ce() const { return lcid_val <= PADDING and lcid_val >= BIT_RATE_QUERY; }

  /// Whether LCID belongs to a Radio Bearer Logical Channel
  bool is_sdu() const { return lcid_val <= LCID32 and lcid_val >= LCID1; }

  /// Returns false for all reserved values in Table 6.2.1-1 and 6.2.1-2
  bool is_valid_lcid() const { return lcid_val <= PADDING and (lcid_val < MIN_RESERVED or lcid_val > MAX_RESERVED); }

  /// Whether LCID subPDU has associated length field
  bool has_length_field() const
  {
    // CCCH (both versions) don't have a length field in the UL
    if (is_ccch()) {
      return false;
    }
    return (is_sdu() || is_var_len_ce());
  }

  bool is_var_len_ce() const
  {
    switch (lcid_val) {
      case LONG_TRUNC_BSR:
      case LONG_BSR:
        return true;
      default:
        return false;
    }
  }

  uint32_t sizeof_ce() const
  {
    switch (lcid_val) {
      case CCCH_SIZE_48:
        return 6;
      case CCCH_SIZE_64:
        return 8;
      case CRNTI:
        return 2;
      case SHORT_BSR:
      case SHORT_TRUNC_BSR:
        return 1;
      case SE_PHR:
        return 2;
      // NOTE: LONG_BSR and LONG_TRUNC_BSR are variable-sized MAC CE, not fixed-sized. Right now this function is not
      // called for these two cases.
      // TODO: Verify if these two cases will be used in the future by this function.
      case LONG_BSR:
      case LONG_TRUNC_BSR:
        return 1; // minimum size, could be more than that
      case PADDING:
        return 0;
      default:
        break;
    }
    return 0;
  }

  bool operator==(lcid_ul_sch_t other) const { return lcid_val == other.lcid_val; }
  bool operator!=(lcid_ul_sch_t other) const { return lcid_val != other.lcid_val; }

private:
  underlying_type lcid_val;
};

} // namespace srsgnb

namespace fmt {

/// FMT formatter of slot_point type.
template <>
struct formatter<srsgnb::lcid_ul_sch_t> : public formatter<uint32_t> {
  template <typename FormatContext>
  auto format(srsgnb::lcid_ul_sch_t lcid, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{}", (uint16_t)lcid);
  }
};

} // namespace fmt
