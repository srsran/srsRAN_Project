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

///
/// @brief  Logical Channel Group and UL Buffer Size format definition and levels according to 3GPP TS 38.321
/// version 15.3.0
///

namespace srsgnb {

/// Logical Channel Group as per TS38.331.
enum lcg_id_t : uint8_t { MAX_LCG_ID = 7, MAX_NOF_LCGS = 8, LCG_ID_INVALID = 8 };

inline lcg_id_t uint_to_lcg_id(std::underlying_type_t<lcg_id_t> val)
{
  return static_cast<lcg_id_t>(val);
}

/// TS 38.321, 6.1.3.1 - Buffer Status Report MAC CEs
enum class bsr_format { SHORT_BSR, LONG_BSR, SHORT_TRUNC_BSR, LONG_TRUNC_BSR };

inline const char* to_string(bsr_format bsr)
{
  static constexpr std::array<const char*, 4> names = {
      "Short BSR", "Long BSR", "Short Truncated BSR", "Long Truncated BSR"};
  return (size_t)bsr < names.size() ? names[(size_t)bsr] : "Invalid BSR format";
}

} // namespace srsgnb
