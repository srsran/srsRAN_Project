/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PRB_H
#define SRSGNB_PRB_H

#include "subcarrier_spacing.h"
#include <cstdint>

namespace srsgnb {

/// Maximum number of PRBs of a UE carrier, for any SCS.
/// \remark See TS 38.331 - "maxNrofPhysicalResourceBlocks".
constexpr std::size_t MAX_NOF_PRBS = 275;

/// See TS 38.211, 4.4.4.1 - Resource Blocks, General.
constexpr std::size_t NOF_SUBCARRIERS_PER_RB = 12;

/// Compute RB bandwidth in kHz, based on the used numerology.
constexpr inline std::size_t get_rb_bw_khz(subcarrier_spacing scs)
{
  return NOF_SUBCARRIERS_PER_RB * scs_to_khz(scs);
}

/// Converts channel bandwidth in MHz to index.
constexpr inline std::size_t channel_bw_mhz_to_index(unsigned dl_bw_mhz)
{
  switch (dl_bw_mhz) {
    case 5:
      return 0;
    case 10:
      return 1;
    case 15:
      return 2;
    case 20:
      return 3;
    case 25:
      return 4;
    case 30:
      return 5;
    case 40:
      return 6;
    case 50:
      return 7;
    case 60:
      return 8;
    case 70:
      return 9;
    case 80:
      return 10;
    case 90:
      return 11;
    case 100:
      return 12;
    case 200:
      return 13;
    case 400:
      return 14;
    default:
      break;
  }
  return std::numeric_limits<size_t>::max();
}

/// Computes maximum transmission bandwidth in number of PRBs for a UE channel.
/// \remark See TS 38.101-1/2, 5.3.2 - Maximum transmission bandwidth configuration.
inline std::size_t get_max_Nprb(unsigned dl_bw_mhz, subcarrier_spacing scs, frequency_range fr)
{
  srsran_sanity_check(is_scs_valid(scs, fr), "Invalid SCS");
  static constexpr std::array<std::size_t, 3> rb_table_fr1[] = {
      // clang-format off
      // SCS
      // 15,  30,  60.
      {  25,  11,   0}, // 5 MHz.
      {  52,  24,  11}, // 10 MHz.
      {  79,  38,  18}, // 15 MHz.
      { 106,  51,  24}, // 20 MHz.
      { 133,  65,  31}, // 25 MHz.
      { 160,  78,  38}, // 30 MHz.
      { 216, 106,  51}, // 40 MHz.
      { 270, 133,  65}, // 50 MHz.
      {   0, 162,  79}, // 60 MHz.
      {   0, 217, 107}, // 80 MHz.
      {   0, 245, 121}, // 90 MHz.
      {   0, 273, 135}, // 100 MHz.
      // clang-format on
  };
  static constexpr std::array<std::size_t, 2> rb_table_fr2[] = {
      // clang-format off
      // SCS
      // 60, 120.
      {  66,  32}, // 50 MHz.
      { 132,  66}, // 100 MHz.
      { 264, 132}, // 200 MHz.
      {   0, 264}, // 400 MHz.
      // clang-format on
  };
  size_t bw_idx = channel_bw_mhz_to_index(dl_bw_mhz);
  if (bw_idx == std::numeric_limits<size_t>::max()) {
    return 0;
  }
  if (fr == frequency_range::FR1) {
    return rb_table_fr1[bw_idx][to_numerology_value(scs)];
  }
  return rb_table_fr2[bw_idx][to_numerology_value(scs) - to_numerology_value(subcarrier_spacing::kHz60)];
}

} // namespace srsgnb

#endif // SRSGNB_PRB_H
