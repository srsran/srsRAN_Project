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

#include "srsran/phy/upper/rb_allocation.h"

using namespace srsran;

bounded_bitset<MAX_RB> rb_allocation::get_contiguous_prb_mask(unsigned bwp_start_rb, unsigned bwp_size_rb) const
{
  unsigned offset = vrb_to_prb_map.get_coreset_start();

  // Make sure the VRB-to-PRB allocation does not exceed the BWP region.
  srsran_assert(offset + vrb_mask.size() <= bwp_size_rb,
                "The contiguous VRB allocation {}:{} with CORESET start {} does not fit in BWP of size {}.",
                vrb_mask.find_lowest(0, vrb_mask.size()),
                vrb_mask.find_highest(0, vrb_mask.size()),
                offset,
                bwp_size_rb);

  bounded_bitset<MAX_RB> prb_mask(bwp_start_rb + bwp_size_rb);

  int lowest_vrb_index = vrb_mask.find_lowest();
  if (lowest_vrb_index == -1) {
    return prb_mask;
  }

  // Calculate PRB boundaries.
  unsigned startpos = bwp_start_rb + offset + static_cast<unsigned>(lowest_vrb_index);
  unsigned endpos   = startpos + vrb_mask.count();

  // Fill consecutive PRB.
  prb_mask.fill(startpos, endpos);

  return prb_mask;
}

bounded_bitset<MAX_RB> rb_allocation::get_other_prb_mask(unsigned bwp_start_rb, unsigned bwp_size_rb) const
{
  bounded_bitset<MAX_RB>          result(bwp_start_rb + bwp_size_rb);
  static_vector<uint16_t, MAX_RB> vrb_to_prb_indices = vrb_to_prb_map.get_allocation_indices(bwp_size_rb);

  for (unsigned vrb_index = 0; vrb_index != vrb_mask.size(); ++vrb_index) {
    // Skip if VRB is not used.
    if (!vrb_mask.test(vrb_index)) {
      continue;
    }

    unsigned prb_index = vrb_to_prb_indices[vrb_index];
    result.set(prb_index, true);
  }

  return result;
}

rb_allocation srsran::rb_allocation::make_type0(const bounded_bitset<MAX_RB>&      vrb_bitmap,
                                                const optional<vrb_to_prb_mapper>& vrb_to_prb_map_)
{
  rb_allocation result;

  result.vrb_mask = vrb_bitmap;

  if (vrb_to_prb_map_.has_value()) {
    result.vrb_to_prb_map = vrb_to_prb_map_.value();
  }

  return result;
}

rb_allocation srsran::rb_allocation::make_type1(unsigned                           rb_start,
                                                unsigned                           rb_count,
                                                const optional<vrb_to_prb_mapper>& vrb_to_prb_map_)
{
  rb_allocation result;

  result.vrb_mask = bounded_bitset<MAX_RB>(rb_start + rb_count);
  result.vrb_mask.fill(rb_start, rb_start + rb_count);

  if (vrb_to_prb_map_.has_value()) {
    result.vrb_to_prb_map = vrb_to_prb_map_.value();
  }

  return result;
}

rb_allocation srsran::rb_allocation::make_custom(std::initializer_list<const unsigned> vrb_indexes,
                                                 const optional<vrb_to_prb_mapper>&    vrb_to_prb_map_)
{
  rb_allocation result;

  // Find the maximum VRB index.
  unsigned rb_end = *std::max_element(vrb_indexes.begin(), vrb_indexes.end());

  // Allocate the VRB mask so the maximum index fits.
  result.vrb_mask = bounded_bitset<MAX_RB>(rb_end + 1);
  for (unsigned vrb_index : vrb_indexes) {
    result.vrb_mask.set(vrb_index, true);
  }

  if (vrb_to_prb_map_.has_value()) {
    result.vrb_to_prb_map = vrb_to_prb_map_.value();
  }

  return result;
}

bounded_bitset<MAX_RB> rb_allocation::get_prb_mask(unsigned bwp_start_rb, unsigned bwp_size_rb) const
{
  srsran_assert(is_bwp_valid(bwp_start_rb, bwp_size_rb),
                "Invalid BWP configuration {}:{} for a VRB mask of size {}.",
                bwp_start_rb,
                bwp_size_rb,
                vrb_mask.size());

  if (vrb_mask.is_contiguous() && !vrb_to_prb_map.is_interleaved()) {
    return get_contiguous_prb_mask(bwp_start_rb, bwp_size_rb);
  }

  return get_other_prb_mask(bwp_start_rb, bwp_size_rb);
}

static_vector<uint16_t, MAX_RB> rb_allocation::get_prb_indices(unsigned bwp_start_rb, unsigned bwp_size_rb) const
{
  srsran_assert(is_bwp_valid(bwp_start_rb, bwp_size_rb),
                "Invalid BWP configuration {}:{} for a VRB mask of size {}.",
                bwp_start_rb,
                bwp_size_rb,
                vrb_mask.size());

  static_vector<uint16_t, MAX_RB> result;
  static_vector<uint16_t, MAX_RB> vrb_to_prb_indices = vrb_to_prb_map.get_allocation_indices(bwp_size_rb);

  for (unsigned vrb_index = 0; vrb_index != vrb_mask.size(); ++vrb_index) {
    // Skip unused VRB
    if (vrb_mask.test(vrb_index)) {
      result.push_back(vrb_to_prb_indices[vrb_index]);
    }
  }

  return result;
}
