/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/rb_allocation.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/support/math_utils.h"
#include "srsgnb/support/srsran_assert.h"

using namespace srsgnb;

void rb_allocation::generate_vrb_to_prb_indexes(span<unsigned>          prb_indices,
                                                vrb_to_prb_mapping_type vrb_to_prb_mapping,
                                                unsigned                bwp_start_rb)
{
  // Deduce the BWP size from the rb_indices size.
  unsigned bwp_size_rb = prb_indices.size();

  // Setup interleaver parameters.
  unsigned L        = (vrb_to_prb_mapping == vrb_to_prb_mapping_type::INTERLEAVED_L_2) ? 2 : 4;
  unsigned N_bundle = divide_ceil(bwp_size_rb, L);
  unsigned R        = 2;
  unsigned C        = N_bundle / R;

  // Calculate the first bundle size in RB.
  unsigned first_bundle_size = L - (bwp_start_rb % L);

  // Calculate the last bundle size in RB.
  unsigned last_bundle_size = (bwp_start_rb + bwp_size_rb) % L;
  if (last_bundle_size == 0) {
    last_bundle_size = L;
  }

  // Set the last bundle indices.
  for (unsigned l = 0; l != last_bundle_size; ++l) {
    prb_indices[(N_bundle - 1) * L + l] = (N_bundle - 1) * L + l;
  }

  // Allocation for interleaved mapping.
  for (unsigned c = 0; c != C; ++c) {
    for (unsigned r = 0; r != R; ++r) {
      // Calculate VRB bundle.
      unsigned j = c * R + r;

      unsigned bundle_size = L;
      if (j == 0) {
        bundle_size = first_bundle_size;
      }

      // Calculate PRB bundle index.
      unsigned i = r * C + c;

      // Set indices.
      for (unsigned l = 0; l != bundle_size; ++l) {
        prb_indices[(j * L) + l] = (i * L) + l;
      }
    }
  }
}
bounded_bitset<MAX_RB> rb_allocation::get_prb_mask(unsigned bwp_start_rb, unsigned bwp_size_rb) const
{
  bounded_bitset<MAX_RB> result(bwp_start_rb + bwp_size_rb);

  // Non-interleaved case, copy VRB mask.
  if (vrb_mask.is_contiguous()) {
    int vrb_begin = vrb_mask.find_lowest();
    int vrb_end   = vrb_mask.find_highest();

    // No PDSCH found.
    if ((vrb_begin == -1) || (vrb_end == -1)) {
      return result;
    }

    unsigned prb_begin = bwp_start_rb + vrb_begin;
    unsigned prb_end   = bwp_start_rb + vrb_end + 1;
    result.fill(prb_begin, prb_end);

    return result;
  }

  // Get interleaver of the BWP size.
  static_vector<unsigned, MAX_RB> interleaver(bwp_size_rb);
  generate_vrb_to_prb_indexes(interleaver, mapping_type, bwp_start_rb);

  // For each VRB mask.
  for (unsigned vrb_idx = 0; vrb_idx != bwp_size_rb; ++vrb_idx) {
    if (vrb_mask.test(vrb_idx)) {
      unsigned prb_idx = interleaver[vrb_idx];
      result.set(bwp_start_rb + prb_idx, true);
    }
  }
  return result;
}

static_vector<uint16_t, MAX_RB> rb_allocation::get_prb_indices(unsigned bwp_start_rb, unsigned bwp_size_rb) const
{
  static_vector<uint16_t, MAX_RB> result;

  // Non-interleaved case, set the PRB indexes as VRB index plus the BWP start.
  if (mapping_type == vrb_to_prb_mapping_type::NON_INTERLEAVED) {
    for (unsigned vrb_idx = 0; vrb_idx != bwp_size_rb; ++vrb_idx) {
      if (vrb_mask.test(vrb_idx)) {
        result.push_back(vrb_idx + bwp_start_rb);
      }
    }
    return result;
  }

  // Get interleaver of the BWP size.
  static_vector<unsigned, MAX_RB> interleaver(bwp_size_rb);
  generate_vrb_to_prb_indexes(interleaver, mapping_type, bwp_start_rb);

  // For each VRB mask.
  for (unsigned vrb_idx = 0; vrb_idx != bwp_size_rb; ++vrb_idx) {
    if (vrb_mask.test(vrb_idx)) {
      unsigned prb_idx = interleaver[vrb_idx];
      result.push_back(prb_idx + bwp_start_rb);
    }
  }
  return result;
}