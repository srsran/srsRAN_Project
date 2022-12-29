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

namespace srsgnb {

/// \brief Helper function that determines from SSB bitmap whether n-th beam is used.
/// \param[in] ssb_bitmap SSB bitmap.
/// \param[in] ssb_index SSB block index/ Beam index.
/// \return true if SSB block/ Beam at index \c ssb_index is active, false otherwise.
inline bool is_nth_ssb_beam_active(uint64_t ssb_bitmap, unsigned ssb_index)
{
  // In the current implementation state, only SSB indices within the interval [0, 7] are allowed.
  srsgnb_sanity_check(
      ssb_index >= 0 and ssb_index < MAX_NUM_BEAMS, "SSB index must be within the interval [0, '{}')", MAX_NUM_BEAMS);
  return (ssb_bitmap & (static_cast<uint64_t>(0b1U) << static_cast<uint64_t>(63U - ssb_index))) > 0;
}

} // namespace srsgnb
