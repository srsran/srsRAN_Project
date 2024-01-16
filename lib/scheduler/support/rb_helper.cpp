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

#include "rb_helper.h"

using namespace srsran;
using namespace srsran::rb_helper;

crb_interval
rb_helper::find_next_empty_interval(const prb_bitmap& used_rb_bitmap, size_t start_crb_idx, size_t last_crb_idx)
{
  int rb_start = used_rb_bitmap.find_lowest(start_crb_idx, std::min(used_rb_bitmap.size(), last_crb_idx), false);
  if (rb_start != -1) {
    int rb_end = used_rb_bitmap.find_lowest(rb_start + 1, std::min(used_rb_bitmap.size(), last_crb_idx), true);
    return {(uint32_t)rb_start, (uint32_t)(rb_end < 0 ? used_rb_bitmap.size() : rb_end)};
  }
  return {};
}

crb_interval
rb_helper::find_empty_interval_of_length(const prb_bitmap& used_rb_bitmap, size_t nof_rbs, uint32_t start_crb_idx)
{
  crb_interval max_interv;
  do {
    crb_interval interv = find_next_empty_interval(used_rb_bitmap, start_crb_idx, used_rb_bitmap.size());
    if (interv.empty()) {
      break;
    }
    if (interv.length() >= nof_rbs) {
      max_interv.set(interv.start(), interv.start() + nof_rbs);
      break;
    }
    if (interv.length() > max_interv.length()) {
      max_interv = interv;
    }
    start_crb_idx = interv.stop() + 1;
  } while (start_crb_idx < used_rb_bitmap.size());
  return max_interv;
}
