/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/ran/resource_allocation/vrb_to_prb.h"
#include "srsran/adt/span.h"
#include "srsran/ran/resource_allocation/rb_bitmap.h"
#include <numeric>

using namespace srsran;
using namespace srsran::vrb_to_prb;

unsigned non_interleaved_mapping::vrb_to_prb(unsigned vrb) const
{
  return config.coreset_start + vrb;
}

unsigned non_interleaved_mapping::prb_to_vrb(unsigned prb) const
{
  srsran_assert(prb >= config.coreset_start, "PRB {} can not be lower than CORESET start {}.");
  return prb - config.coreset_start;
}

prb_interval non_interleaved_mapping::vrb_to_prb(const vrb_interval& vrbs) const
{
  return {vrb_to_prb(vrbs.start()), vrb_to_prb(vrbs.stop())};
}

vrb_interval non_interleaved_mapping::prb_to_vrb(const prb_interval& prbs) const
{
  return {prb_to_vrb(prbs.start()), prb_to_vrb(prbs.stop())};
}

prb_bitmap non_interleaved_mapping::vrb_to_prb(unsigned bwp_size, const vrb_bitmap& vrbs) const
{
  srsran_assert(config.coreset_start + vrbs.size() <= bwp_size,
                "This non-interleaved mapping ({} VRBs with CORESET start at {}) is not suitable for a BWP of size {}.",
                vrbs.size(),
                config.coreset_start,
                bwp_size);

  prb_bitmap prbs(bwp_size);
  vrbs.for_each(0, vrbs.size(), [&](unsigned vrb) { prbs.set(config.coreset_start + vrb, true); });
  return prbs;
}

vrb_bitmap non_interleaved_mapping::prb_to_vrb(const prb_bitmap& prbs) const
{
  srsran_assert(prbs.size() > config.coreset_start,
                "A BWP of {} PRBs is not suitable for a non-interleaved mapping with a CORESET start at {}.",
                prbs.size(),
                config.coreset_start);
  srsran_assert(prbs.none(0, config.coreset_start),
                "The first {} PRBs can not be set for a non-interleaved mapping with a CORESET start at {}.",
                config.coreset_start,
                config.coreset_start);

  vrb_bitmap vrbs(prbs.size() - config.coreset_start);
  prbs.for_each(config.coreset_start, prbs.size(), [&](unsigned prb) { vrbs.set(prb - config.coreset_start); });
  return vrbs;
}

crb_bitmap non_interleaved_mapping::vrb_to_crb(unsigned bwp_start, unsigned bwp_size, const vrb_bitmap& vrbs) const
{
  srsran_assert(config.coreset_start + vrbs.size() <= bwp_size,
                "This non-interleaved mapping ({} VRBs with CORESET start at {}) is not suitable for a BWP of size {}.",
                vrbs.size(),
                config.coreset_start,
                bwp_size);

  crb_bitmap crbs(bwp_start + bwp_size);
  for_each_interval(vrbs, 0, vrbs.size(), [&](size_t vrb_begin, size_t vrb_end) {
    crbs.fill(bwp_start + config.coreset_start + vrb_begin, bwp_start + config.coreset_start + vrb_end, true);
  });
  return crbs;
}

static_vector<uint16_t, MAX_NOF_PRBS>
non_interleaved_mapping::vrb_to_crb_indices(unsigned bwp_start, unsigned bwp_size, const srsran::vrb_bitmap& vrbs) const
{
  srsran_assert(config.coreset_start + vrbs.size() <= bwp_size,
                "This non-interleaved mapping ({} VRBs with CORESET start at {}) is not suitable for a BWP of size {}.",
                vrbs.size(),
                config.coreset_start,
                bwp_size);

  static_vector<uint16_t, MAX_NOF_PRBS> result;
  crb_bitmap                            crbs(bwp_start + bwp_size);
  vrbs.for_each(0, vrbs.size(), [&](unsigned vrb) { result.emplace_back(bwp_start + vrb_to_prb(vrb)); });
  return result;
}

static void vrb_to_prb_mapper_get_interleaved_prb(span<uint16_t> prb_indices,
                                                  unsigned       offset,
                                                  unsigned       nof_bundles,
                                                  unsigned       first_bundle_size,
                                                  unsigned       other_bundle_size,
                                                  unsigned       last_bundle_size)
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
  std::iota(last_bundle.begin(), last_bundle.end(), offset + (nof_bundles - 2) * other_bundle_size + first_bundle_size);

  // Allocation for interleaved mapping.
  for (unsigned c = 0; c != C; ++c) {
    for (unsigned r = 0; r != R; ++r) {
      // Calculate VRB bundle.
      const unsigned j = c * R + r;

      // Skip the first and last VRB bundles.
      if (j == 0 or j > nof_bundles - 2) {
        continue;
      }

      // Calculate PRB bundle index.
      unsigned f_j = r * C + c;

      // Make view of the bundle j.
      span<uint16_t> bundle_j = prb_indices.subspan((j - 1) * other_bundle_size + first_bundle_size, other_bundle_size);

      // Fill the bundle with the PRB indices.
      std::iota(bundle_j.begin(), bundle_j.end(), offset + (f_j - 1) * other_bundle_size + first_bundle_size);
    }
  }
}

interleaved_mapping::interleaved_mapping(const configuration& config_) :
  config(config_), vrb_to_prb_indices(config.nof_rbs), prb_to_vrb_indices(config.coreset_start + config.nof_rbs)
{
  srsran_assert(config.is_interleaved(), "Invalid configuration for interleaved mapping.");

  vrb_to_prb_mapper_get_interleaved_prb(vrb_to_prb_indices,
                                        config.coreset_start,
                                        config.nof_bundles,
                                        config.first_bundle_size,
                                        config.other_bundle_size,
                                        config.last_bundle_size);

  for (size_t vrb = 0; vrb != config.nof_rbs; ++vrb) {
    prb_to_vrb_indices[vrb_to_prb_indices[vrb]] = vrb;
  }
}

unsigned interleaved_mapping::vrb_to_prb(unsigned vrb) const
{
  srsran_assert(vrb <= config.nof_rbs, "VRB {} can not be higher than {}.");
  return vrb_to_prb_indices[vrb];
}

unsigned interleaved_mapping::prb_to_vrb(unsigned prb) const
{
  srsran_assert(
      prb >= config.coreset_start, "PRB {} can not be lower than CORESET start {}.", prb, config.coreset_start);
  srsran_assert(prb <= config.coreset_start + config.nof_rbs,
                "PRB {} can not be higher than {}.",
                prb,
                config.coreset_start + config.nof_rbs);
  return prb_to_vrb_indices[prb];
}

std::pair<prb_interval, prb_interval> interleaved_mapping::vrb_to_prb(const vrb_interval& vrbs) const
{
  if (vrbs.empty()) {
    return {{}, {}};
  }

  prb_bitmap prb_bits(config.coreset_start + config.nof_rbs);
  for (unsigned vrb = vrbs.start(), vrb_end = vrbs.stop(); vrb != vrb_end; ++vrb) {
    prb_bits.set(vrb_to_prb(vrb));
  }

  // Compute the first interval.
  int      start_prb = prb_bits.find_lowest();
  unsigned end_prb   = start_prb;
  do {
    ++end_prb;
    if (end_prb == prb_bits.size()) {
      return {{start_prb, end_prb}, {}};
    }
  } while (prb_bits.test(end_prb));

  prb_interval first{start_prb, end_prb};

  // Compute the second interval.
  start_prb = prb_bits.find_lowest(end_prb, prb_bits.size());
  if (start_prb == -1) {
    return {first, {}};
  }
  end_prb = start_prb;
  do {
    ++end_prb;
    if (end_prb == prb_bits.size()) {
      return {first, {start_prb, end_prb}};
    }
  } while (prb_bits.test(end_prb));

  return {first, {start_prb, end_prb}};
}

prb_bitmap interleaved_mapping::vrb_to_prb(unsigned bwp_size, const vrb_bitmap& vrbs) const
{
  srsran_assert(vrbs.size() <= config.nof_rbs,
                "The VRB bitmap size (i.e., {}) is greater than the size of the interleaving (i.e., {}).",
                vrbs.size(),
                config.nof_rbs);
  srsran_assert(config.coreset_start + config.nof_rbs <= bwp_size,
                "This interleaved mapping ({} VRBs with CORESET start at {}) is not suitable for a BWP of size {}.",
                config.nof_rbs,
                config.coreset_start,
                bwp_size);

  prb_bitmap prbs(bwp_size);
  vrbs.for_each(0, vrbs.size(), [&](unsigned vrb) { prbs.set(vrb_to_prb(vrb)); });
  return prbs;
}

vrb_bitmap interleaved_mapping::prb_to_vrb(const prb_bitmap& prbs) const
{
  srsran_assert(prbs.size() == (config.coreset_start + config.nof_rbs),
                "The PRB bitmap size (i.e., {}) doesn't match the size of the interleaving (i.e., {}).",
                prbs.size(),
                config.coreset_start + config.nof_rbs);
  srsran_assert(prbs.none(0, config.coreset_start),
                "The first {} PRBs can not be set for a interleaved mapping with a CORESET start at {}.",
                config.coreset_start,
                config.coreset_start);

  vrb_bitmap vrbs(config.nof_rbs);
  prbs.for_each(0, prbs.size(), [&](unsigned prb) { vrbs.set(prb_to_vrb(prb)); });
  return vrbs;
}

crb_bitmap interleaved_mapping::vrb_to_crb(unsigned bwp_start, unsigned bwp_size, const vrb_bitmap& vrbs) const
{
  srsran_assert(vrbs.size() <= config.nof_rbs,
                "The VRB bitmap size (i.e., {}) is greater than the size of the interleaving (i.e., {}).",
                vrbs.size(),
                config.nof_rbs);
  srsran_assert(config.coreset_start + config.nof_rbs <= bwp_size,
                "This interleaved mapping ({} VRBs with CORESET start at {}) is not suitable for a BWP of size {}.",
                config.nof_rbs,
                config.coreset_start,
                bwp_size);

  crb_bitmap crbs(bwp_start + bwp_size);
  vrbs.for_each(0, vrbs.size(), [&](unsigned vrb) { crbs.set(bwp_start + vrb_to_prb(vrb)); });
  return crbs;
}

static_vector<uint16_t, MAX_NOF_PRBS>
interleaved_mapping::vrb_to_crb_indices(unsigned bwp_start, unsigned bwp_size, const srsran::vrb_bitmap& vrbs) const
{
  static_vector<uint16_t, MAX_NOF_PRBS> result;
  crb_bitmap                            crbs(bwp_start + bwp_size);
  vrbs.for_each(0, vrbs.size(), [&](unsigned vrb) { result.emplace_back(bwp_start + vrb_to_prb(vrb)); });
  return result;
}
