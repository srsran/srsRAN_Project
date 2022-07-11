
#pragma once

///
/// @brief  Logical Channel Group and UL Buffer Size format definition and levels according to 3GPP TS 38.321
/// version 15.3.0
///

namespace srsgnb {

/// Logical Channel Group
using lcg_id_t = uint32_t;

/// TS 38.321, 6.1.3.1 - Buffer Status Report MAC CEs
const lcg_id_t MAX_LOGICAL_CHANNEL_GROUP = 8;

/// TS 38.321, 6.1.3.1 - Buffer Status Report MAC CEs
enum class bsr_format { SHORT_BSR, LONG_BSR, SHORT_TRUNC_BSR, LONG_TRUNC_BSR };

inline const char* to_string(bsr_format bsr)
{
  static constexpr std::array<const char*, 4> names = {
      "Short BSR", "Long BSR", "Short Truncated BSR", "Long Truncated BSR"};
  return (size_t)bsr < names.size() ? names[(size_t)bsr] : "Invalid BSR format";
}

} // namespace srsgnb
