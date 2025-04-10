/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/upper/rb_allocation.h"
#include "srsran/ran/resource_allocation/vrb_to_prb.h"

using namespace srsran;

rb_allocation srsran::rb_allocation::make_type0(const vrb_bitmap&                               vrb_bitmap,
                                                const std::optional<vrb_to_prb::configuration>& vrb_to_prb_config)
{
  rb_allocation result;

  result.vrb_mask = vrb_bitmap;
  if (vrb_to_prb_config.has_value()) {
    result.vrb_to_prb_config = *vrb_to_prb_config;
  }

  return result;
}

rb_allocation srsran::rb_allocation::make_type1(unsigned                                        rb_start,
                                                unsigned                                        rb_count,
                                                const std::optional<vrb_to_prb::configuration>& vrb_to_prb_config)
{
  rb_allocation result;

  result.vrb_mask = vrb_bitmap(rb_start + rb_count);
  result.vrb_mask.fill(rb_start, rb_start + rb_count);
  if (vrb_to_prb_config.has_value()) {
    result.vrb_to_prb_config = *vrb_to_prb_config;
  }

  return result;
}

rb_allocation srsran::rb_allocation::make_custom(std::initializer_list<const unsigned>           vrb_indexes,
                                                 const std::optional<vrb_to_prb::configuration>& vrb_to_prb_config)
{
  rb_allocation result;

  // Find the maximum VRB index.
  unsigned rb_end = *std::max_element(vrb_indexes.begin(), vrb_indexes.end());

  // Allocate the VRB mask so the maximum index fits.
  result.vrb_mask = vrb_bitmap(rb_end + 1);
  for (unsigned vrb_index : vrb_indexes) {
    result.vrb_mask.set(vrb_index, true);
  }
  if (vrb_to_prb_config.has_value()) {
    result.vrb_to_prb_config = *vrb_to_prb_config;
  }

  return result;
}

crb_bitmap rb_allocation::get_crb_mask(unsigned bwp_start_rb, unsigned bwp_size_rb) const
{
  srsran_assert(is_bwp_valid(bwp_start_rb, bwp_size_rb),
                "Invalid BWP configuration {}:{} for a VRB mask of size {}.",
                bwp_start_rb,
                bwp_size_rb,
                vrb_mask.size());

  if (vrb_to_prb_config.is_interleaved()) {
    vrb_to_prb::interleaved_mapping map(vrb_to_prb_config);
    return map.vrb_to_crb(bwp_start_rb, bwp_size_rb, vrb_mask);
  }
  vrb_to_prb::non_interleaved_mapping map(vrb_to_prb_config);
  return map.vrb_to_crb(bwp_start_rb, bwp_size_rb, vrb_mask);
}
