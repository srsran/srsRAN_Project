/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

namespace srsran {

/// \brief Helper function that determines from SSB bitmap whether n-th beam is used.
/// \param[in] ssb_bitmap SSB bitmap.
/// \param[in] ssb_index SSB block index/ Beam index.
/// \return true if SSB block/ Beam at index \c ssb_index is active, false otherwise.
inline bool is_nth_ssb_beam_active(uint64_t ssb_bitmap, unsigned ssb_index)
{
  // In the current implementation state, only SSB indices within the interval [0, 7] are allowed.
  srsran_sanity_check(
      ssb_index >= 0 and ssb_index < MAX_NUM_BEAMS, "SSB index must be within the interval [0, '{}')", MAX_NUM_BEAMS);
  return (ssb_bitmap & (static_cast<uint64_t>(0b1U) << static_cast<uint64_t>(63U - ssb_index))) > 0;
}

} // namespace srsran
