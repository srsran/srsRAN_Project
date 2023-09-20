/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

/// \c maxNrofTAGs, TS 38.331.
const unsigned MAX_NOF_TAGS = 4;

/// \c TAG-Id, TS 38.331.
enum tag_id_t { MAX_TAG_ID = 3, INVALID_TAG_ID = MAX_NOF_TAGS };

/// Converts a uint into a \ref enum tag_id_t .
inline tag_id_t uint_to_tag_id(unsigned tag_id)
{
  return static_cast<tag_id_t>(tag_id);
}

/// \c TimeAlignmentTimer, TS 38.331.
enum class time_alignment_timer {
  ms500    = 500,
  ms750    = 750,
  ms1280   = 1280,
  ms1920   = 1920,
  ms2560   = 2560,
  ms5120   = 5120,
  ms10240  = 10240,
  infinity = 0
};

/// Return the value of \ref time_alignment_timer.
inline std::underlying_type<time_alignment_timer>::type time_alignment_timer_to_value(time_alignment_timer timer)
{
  return static_cast<std::underlying_type<time_alignment_timer>::type>(timer);
}

/// \c TAG, or Time Alignment Group, TS 38.331.
struct tag {
  tag_id_t             tag_id;
  time_alignment_timer ta_timer;

  bool operator==(const tag& rhs) const { return tag_id == rhs.tag_id && ta_timer == rhs.ta_timer; }
  bool operator!=(const tag& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
