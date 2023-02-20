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

#include <cstdint>
#include <type_traits>

namespace srsgnb {

/// Logical Channel Group as per TS38.331.
enum lcg_id_t : uint8_t { MAX_LCG_ID = 7, MAX_NOF_LCGS = 8, LCG_ID_INVALID = 8 };

inline lcg_id_t uint_to_lcg_id(std::underlying_type_t<lcg_id_t> val)
{
  return static_cast<lcg_id_t>(val);
}

} // namespace srsgnb