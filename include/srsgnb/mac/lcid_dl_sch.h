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

class lcid_dl_sch_t
{
  using underlying_type = std::underlying_type_t<lcid_t>;

public:
  /// 3GPP 38.321, Table 6.2.1-1 - Values of LCID for DL-SCH Index
  enum options : underlying_type {
    CCCH = 0b000000,

    /// Identity of the logical channel
    LCID1 = 1,
    // ...
    LCID32 = 32,

    /// Reserved
    MIN_RESERVED = 33,
    MAX_RESERVED = 51,

    RECOMMENDED_BIT_RATE = 0b101111,

    LONG_DRX_CMD  = 0b111011,
    DRX_CMD       = 0b111100,
    TA_CMD        = 0b111101,
    UE_CON_RES_ID = 0b111110,
    PADDING       = 0b111111
  };

  lcid_dl_sch_t(underlying_type lcid_ = PADDING) : lcid_val(lcid_) { srsgnb_assert(lcid_ <= PADDING, "Invalid LCID"); }
  lcid_dl_sch_t& operator=(underlying_type lcid)
  {
    srsgnb_assert(lcid <= PADDING, "Invalid LCID");
    lcid_val = lcid;
    return *this;
  }

  explicit        operator underlying_type() const { return lcid_val; }
  underlying_type value() const { return lcid_val; }

  /// Whether LCID is an MAC CE
  bool is_ce() const { return lcid_val <= PADDING and lcid_val >= RECOMMENDED_BIT_RATE; }

  /// Whether LCID belongs to a Radio Bearer Logical Channel
  bool is_sdu() const { return lcid_val <= LCID32 and lcid_val >= CCCH; }

  lcid_t to_lcid() const
  {
    srsgnb_assert(is_sdu(), "Invalid to_lcid() access");
    return (lcid_t)lcid_val;
  }

  /// Returns false for all reserved values in Table 6.2.1-1 and 6.2.1-2
  bool is_valid() const { return lcid_val <= PADDING and (lcid_val < MIN_RESERVED or lcid_val > MAX_RESERVED); }

  bool is_var_len_ce() const { return false; }

  uint32_t sizeof_ce() const
  {
    // Values taken from TS38.321, Section 6.1.3.
    switch (lcid_val) {
      case TA_CMD:
        return 1;
      case UE_CON_RES_ID:
        return 6;
      case PADDING:
        return 0;
      default:
        break;
    }
    return 0;
  }

  bool operator==(lcid_dl_sch_t other) const { return lcid_val == other.lcid_val; }
  bool operator!=(lcid_dl_sch_t other) const { return lcid_val != other.lcid_val; }

private:
  underlying_type lcid_val;
};

} // namespace srsgnb

namespace fmt {

/// FMT formatter of slot_point type.
template <>
struct formatter<srsgnb::lcid_dl_sch_t> : public formatter<uint16_t> {
  template <typename FormatContext>
  auto format(srsgnb::lcid_dl_sch_t lcid, FormatContext& ctx)
  {
    return format_to(ctx.out(), "{}", (uint16_t)lcid);
  }
};

} // namespace fmt
