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
#include <type_traits>

namespace srsgnb {

/// Logical Channel Identity used to associate one logical channel to the corresponding RLC bearer. Values (0..32)
enum lcid_t : uint16_t {
  LCID_SRB0        = 0,
  LCID_SRB1        = 1,
  LCID_SRB2        = 2,
  LCID_SRB3        = 3,
  LCID_MIN_DRB     = 4,
  LCID_MAX_DRB     = 32,
  MAX_NOF_RB_LCIDS = 33,
  INVALID_LCID     = 64
};

constexpr inline lcid_t uint_to_lcid(std::underlying_type_t<lcid_t> val)
{
  return static_cast<lcid_t>(val);
}

/// Maximum value of Logical Channel ID.
/// \remark See TS 38.331, maxLC-ID.
constexpr lcid_t MAX_LCID = LCID_MAX_DRB;

enum class srb_id_t : uint16_t { srb0 = 0, srb1, srb2, srb3, nulltype };

constexpr std::size_t MAX_NOF_SRBS = 4;

constexpr inline uint16_t srb_id_to_uint(srb_id_t id)
{
  return static_cast<uint16_t>(id);
}

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

inline srb_id_t int_to_srb_id(std::underlying_type_t<srb_id_t> val)
{
  return static_cast<srb_id_t>(val);
}

/// Convert SRB ID to LCID.
inline lcid_t srb_id_to_lcid(srb_id_t srb_id)
{
  return static_cast<lcid_t>(srb_id);
}

inline const char* srb_id_to_string(srb_id_t srb_id)
{
  constexpr static const char* names[] = {"SRB0", "SRB1", "SRB2", "SRB3", "invalid"};
  return names[srb_id_to_uint(srb_id < srb_id_t::nulltype ? srb_id : srb_id_t::nulltype)];
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

constexpr static std::size_t MAX_NOF_DRBS = 29;

constexpr inline uint16_t drb_id_to_uint(drb_id_t id)
{
  return static_cast<uint16_t>(id);
}

constexpr inline drb_id_t uint_to_drb_id(uint16_t id)
{
  return static_cast<drb_id_t>(id);
}

} // namespace srsgnb
