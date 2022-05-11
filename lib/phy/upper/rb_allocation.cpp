/**
 *
 * \section COPYRIGHT
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

void rb_allocation::get_allocation_mask(span<bool> mask, unsigned bwp_start_rb, unsigned bwp_size_rb) const
{
  srsran_assert(mask.size() >= bwp_start_rb + bwp_size_rb,
                "The mask with size %d cannot fit the BWP %d:%d",
                mask.size(),
                bwp_start_rb,
                bwp_start_rb + bwp_size_rb);

  // Non-interleaved case, copy VRB mask.
  if (mapping_type == vrb_to_prb_mapping_type::NON_INTERLEAVED) {
    std::copy(vrb_mask.begin(), vrb_mask.begin() + bwp_size_rb, mask.begin() + bwp_start_rb);
    return;
  }

  // Get interleaver of the BWP size.
  static_vector<unsigned, MAX_RB> interleaver(bwp_size_rb);
  generate_vrb_to_prb_indexes(interleaver, mapping_type, bwp_start_rb);

  // Set all entries to false.
  std::fill(mask.begin(), mask.end(), false);

  // For each VRB mask.
  for (unsigned vrb_idx = 0; vrb_idx != bwp_size_rb; ++vrb_idx) {
    if (vrb_mask[vrb_idx]) {
      unsigned prb_idx             = interleaver[vrb_idx];
      mask[bwp_start_rb + prb_idx] = true;
    }
  }
}

void rb_allocation::get_allocation_indices(span<unsigned> prb_indices,
                                           unsigned       bwp_start_rb,
                                           unsigned       bwp_size_rb) const
{
  srsran_assert(prb_indices.size() == rb_count,
                "The number of PRB indices (%d) does not match the number of allocated VRB (%d)",
                prb_indices.size(),
                rb_count);

  unsigned count = 0;

  // Non-interleaved case, set the PRB indexes as VRB index plus the BWP start.
  if (mapping_type == vrb_to_prb_mapping_type::NON_INTERLEAVED) {
    for (unsigned vrb_idx = 0; vrb_idx != bwp_size_rb; ++vrb_idx) {
      if (vrb_mask[vrb_idx]) {
        prb_indices[count] = vrb_idx + bwp_start_rb;
        ++count;
      }
    }
    return;
  }

  // Get interleaver of the BWP size.
  static_vector<unsigned, MAX_RB> interleaver(bwp_size_rb);
  generate_vrb_to_prb_indexes(interleaver, mapping_type, bwp_start_rb);

  // For each VRB mask.
  for (unsigned vrb_idx = 0; vrb_idx != bwp_size_rb; ++vrb_idx) {
    if (vrb_mask[vrb_idx]) {
      unsigned prb_idx   = interleaver[vrb_idx];
      prb_indices[count] = prb_idx + bwp_start_rb;
    }
  }
}