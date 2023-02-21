/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/lcid.h"
#include "srsran/support/srsran_assert.h"
#include "fmt/format.h"

namespace srsran {

/// \brief LCID representation for PDSCH.
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

    // TODO: Add remaining.

    SCELL_ACTIV_4_OCTET = 0b111001,
    SCELL_ACTIV_1_OCTET = 0b111010,

    LONG_DRX_CMD  = 0b111011,
    DRX_CMD       = 0b111100,
    TA_CMD        = 0b111101,
    UE_CON_RES_ID = 0b111110,
    PADDING       = 0b111111
  };

  lcid_dl_sch_t(underlying_type lcid_ = PADDING) : lcid_val(lcid_) { srsran_assert(lcid_ <= PADDING, "Invalid LCID"); }
  lcid_dl_sch_t& operator=(underlying_type lcid)
  {
    srsran_assert(lcid <= PADDING, "Invalid LCID");
    lcid_val = lcid;
    return *this;
  }

  /// convert lcid_dl_sch_t to underlying integer type via cast.
  explicit operator underlying_type() const { return lcid_val; }

  /// convert lcid_dl_sch_t to underlying integer type.
  underlying_type value() const { return lcid_val; }

  /// Whether LCID is an MAC CE
  bool is_ce() const { return lcid_val <= PADDING and lcid_val >= RECOMMENDED_BIT_RATE; }

  /// Whether LCID belongs to a Radio Bearer Logical Channel
  bool is_sdu() const { return lcid_val <= LCID32 and lcid_val >= CCCH; }

  lcid_t to_lcid() const
  {
    srsran_assert(is_sdu(), "Invalid to_lcid() access");
    return (lcid_t)lcid_val;
  }

  /// Returns false for all reserved values in Table 6.2.1-1 and 6.2.1-2
  bool is_valid() const { return lcid_val <= PADDING and (lcid_val < MIN_RESERVED or lcid_val > MAX_RESERVED); }

  bool is_var_len_ce() const { return false; }

  uint32_t sizeof_ce() const
  {
    // Values taken from TS38.321, Section 6.1.3.
    switch (lcid_val) {
      case SCELL_ACTIV_4_OCTET:
        return 4;
      case SCELL_ACTIV_1_OCTET:
        return 1;
      case LONG_DRX_CMD:
      case DRX_CMD:
        return 0;
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

} // namespace srsran

namespace fmt {

/// FMT formatter of slot_point type.
template <>
struct formatter<srsran::lcid_dl_sch_t> : public formatter<uint16_t> {
  template <typename FormatContext>
  auto format(srsran::lcid_dl_sch_t lcid, FormatContext& ctx)
  {
    return format_to(ctx.out(), "{}", (uint16_t)lcid);
  }
};

} // namespace fmt
