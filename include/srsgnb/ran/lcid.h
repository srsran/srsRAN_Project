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

#include <cstdint>

namespace srsgnb {

/// Logical Channel Identity used to associate one logical channel to the corresponding RLC bearer. Values (0..32)
enum lcid_t : uint16_t {
  LCID_SRB0    = 0,
  LCID_SRB1    = 1,
  LCID_SRB2    = 2,
  LCID_SRB3    = 3,
  LCID_MIN_DRB = 4,
  LCID_MAX_DRB = 32,
  INVALID_LCID = 64
};

/// Maximum value of Logical Channel ID.
/// \remark See TS 38.331, maxLC-ID.
constexpr lcid_t MAX_LCID = LCID_MAX_DRB;

enum class srb_id_t : uint16_t { srb0 = 0, srb1, srb2, srb3, nulltype };

inline bool is_srb(lcid_t lcid)
{
  return lcid <= LCID_SRB3;
}

inline bool is_lcid_valid(lcid_t lcid)
{
  return lcid <= MAX_LCID;
}
inline srb_id_t to_srb_id(lcid_t lcid)
{
  return is_srb(lcid) ? static_cast<srb_id_t>(lcid) : srb_id_t::nulltype;
}

enum class drb_id_t : uint16_t {
  drb1 = 1,
  drb2,
  drb3,
  drb4,
  drb5,
  drb6,
  drb7,
  drb8,
  drb9,
  drb10,
  drb11,
  drb12,
  drb13,
  drb14,
  drb15,
  drb16,
  drb17,
  drb18,
  drb19,
  drb20,
  drb21,
  drb22,
  drb23,
  drb24,
  drb25,
  drb26,
  drb27,
  drb28,
  drb29,
  invalid
};

} // namespace srsgnb
