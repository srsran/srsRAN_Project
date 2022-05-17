/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_LCID_H
#define SRSGNB_RAN_LCID_H

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

} // namespace srsgnb

#endif // SRSGNB_RAN_LCID_H
