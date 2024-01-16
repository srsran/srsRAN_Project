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

#include "srsran/phy/upper/vrb_to_prb_mapper.h"
#include "srsran/adt/span.h"
#include "srsran/support/math_utils.h"
#include <numeric>

using namespace srsran;

static void vrb_to_prb_mapper_get_non_interleaved_prb(span<uint16_t> prb_indices, unsigned offset)
{
  std::iota(prb_indices.begin(), prb_indices.end(), offset);
}

static void vrb_to_prb_mapper_get_interleaved_prb(span<uint16_t> prb_indices,
                                                  unsigned       offset,
                                                  unsigned       nof_bundles,
                                                  unsigned       first_bundle_size,
                                                  unsigned       last_bundle_size,
                                                  unsigned       other_bundle_size)
{
  // Setup interleaver parameters.
  unsigned N_bundle = nof_bundles;
  unsigned R        = 2;
  unsigned C        = N_bundle / R;

  // Fill the first bundle indices.
  span<uint16_t> first_bundle = prb_indices.first(first_bundle_size);
  std::iota(first_bundle.begin(), first_bundle.end(), offset);

  // Fill the last bundle indices.
  span<uint16_t> last_bundle = prb_indices.last(last_bundle_size);
  std::iota(last_bundle.begin(), last_bundle.end(), (prb_indices.size() - last_bundle_size - 1) + offset);

  // Allocation for interleaved mapping.
  for (unsigned c = 0; c != C; ++c) {
    for (unsigned r = 0; r != R; ++r) {
      // Skip the first bundle.
      if (c == 0 && r == 0) {
        continue;
      }

      // Calculate VRB bundle.
      unsigned j = c * R + r;

      // Calculate PRB bundle index.
      unsigned f_j = r * C + c;

      // Make view of the bundle j.
      span<uint16_t> bundle_j = prb_indices.subspan((j - 1) * other_bundle_size + first_bundle_size, other_bundle_size);

      // Fill the bundle with the PRB indices.
      std::iota(bundle_j.begin(), bundle_j.end(), (f_j - 1) * other_bundle_size + first_bundle_size);
    }
  }
}

vrb_to_prb_mapper vrb_to_prb_mapper::create_non_interleaved_common_ss(unsigned N_start_coreset)
{
  vrb_to_prb_mapper mapper;

  // Set CORESET start.
  mapper.coreset_start = N_start_coreset;

  // Set bundle size as non-interleaved.
  mapper.nof_bundles = 0;

  return mapper;
}

vrb_to_prb_mapper vrb_to_prb_mapper::create_non_interleaved_other()
{
  // Keep default values.
  return vrb_to_prb_mapper();
}

vrb_to_prb_mapper vrb_to_prb_mapper::create_interleaved_coreset0(unsigned N_start_coreset, unsigned N_bwp_init_size)
{
  vrb_to_prb_mapper mapper;

  // CORESET0 constant bundle size.
  static constexpr unsigned L = 2;

  // Set CORESET start.
  mapper.coreset_start = N_start_coreset;

  // Set bundle size as interleaved.
  mapper.nof_bundles = divide_ceil(N_bwp_init_size, L);

  // Set the size of the last bundle.
  mapper.last_bundle_size = (N_bwp_init_size % L > 0) ? (N_bwp_init_size % L) : L;

  // All other RBs are set to L.
  mapper.first_bundle_size = L;
  mapper.other_bundle_size = L;

  return mapper;
}

vrb_to_prb_mapper
vrb_to_prb_mapper::create_interleaved_common(unsigned N_start_coreset, unsigned N_bwp_i_start, unsigned N_bwp_init_size)
{
  vrb_to_prb_mapper mapper;

  // Constant bundle size.
  static constexpr unsigned L = 2;

  // Set CORESET start.
  mapper.coreset_start = N_start_coreset;

  // Set bundle size as interleaved.
  mapper.nof_bundles = divide_ceil(N_bwp_init_size + N_start_coreset + N_bwp_i_start, L);

  // Set the size of the first bundle.
  mapper.first_bundle_size = L - ((N_bwp_i_start + N_start_coreset) % L);

  // Set the size of the last bundle.
  mapper.last_bundle_size = (((N_bwp_init_size + N_start_coreset + N_bwp_i_start) % L) > 0)
                                ? ((N_bwp_init_size + N_start_coreset + N_bwp_i_start) % L)
                                : L;

  // All other RBs are set to L.
  mapper.other_bundle_size = L;

  return mapper;
}

vrb_to_prb_mapper
vrb_to_prb_mapper::create_interleaved_other(unsigned int N_bwp_i_start, unsigned int N_bwp_i_size, unsigned int L_i)
{
  vrb_to_prb_mapper mapper;

  // Set number of bundles as interleaved.
  mapper.nof_bundles = divide_ceil(N_bwp_i_size + N_bwp_i_start, L_i);

  // Set the size of the first bundle.
  mapper.first_bundle_size = L_i - (N_bwp_i_size % L_i);

  // Set the size of the last bundle.
  mapper.last_bundle_size = (((N_bwp_i_size + N_bwp_i_start) % L_i) > 0) ? ((N_bwp_i_size + N_bwp_i_start) % L_i) : L_i;

  // All other RBs are set to L.
  mapper.other_bundle_size = L_i;

  return mapper;
}

static_vector<uint16_t, MAX_RB> vrb_to_prb_mapper::get_allocation_indices(unsigned bwp_size) const
{
  static_vector<uint16_t, MAX_RB> result(bwp_size);
  if (!is_interleaved()) {
    vrb_to_prb_mapper_get_non_interleaved_prb(result, coreset_start);
  } else {
    vrb_to_prb_mapper_get_interleaved_prb(
        result, coreset_start, nof_bundles, first_bundle_size, last_bundle_size, other_bundle_size);
  }

  return result;
}