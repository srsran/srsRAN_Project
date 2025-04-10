/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/resource_allocation/vrb_to_prb.h"
#include "srsran/adt/span.h"
#include <numeric>

using namespace srsran;
using namespace srsran::vrb_to_prb;

prb_interval non_interleaved_mapping::vrb_to_prb(const vrb_interval& vrbs) const
{
  return {config.coreset_start + vrbs.start(), config.coreset_start + vrbs.stop()};
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

prb_bitmap non_interleaved_mapping::vrb_to_prb(unsigned bwp_size, const vrb_bitmap& vrbs) const
{
  srsran_assert(config.coreset_start + vrbs.size() <= bwp_size,
                "This non-interleaved mapping ({} VRBs with CORESET start at {}) is not suitable for a BWP of size {}.",
                vrbs.size(),
                config.coreset_start,
                bwp_size);

  prb_bitmap prbs(bwp_size);
  vrbs.for_each(0, vrbs.size(), [&](unsigned vrb) { prbs.set(vrb + config.coreset_start, true); });
  return prbs;
}

crb_bitmap non_interleaved_mapping::vrb_to_crb(unsigned bwp_start, unsigned bwp_size, const vrb_bitmap& vrbs) const
{
  srsran_assert(config.coreset_start + vrbs.size() <= bwp_size,
                "This non-interleaved mapping ({} VRBs with CORESET start at {}) is not suitable for a BWP of size {}.",
                vrbs.size(),
                config.coreset_start,
                bwp_size);

  crb_bitmap crbs(bwp_start + bwp_size);
  vrbs.for_each(0, vrbs.size(), [&](unsigned vrb) { crbs.set(bwp_start + config.coreset_start + vrb, true); });
  return crbs;
}

static void vrb_to_prb_mapper_get_interleaved_prb(span<uint16_t> prb_indices,
                                                  unsigned       offset,
                                                  unsigned       nof_bundles,
                                                  unsigned       first_bundle_size,
                                                  unsigned       other_bundle_size,
                                                  unsigned       last_bundle_size)
{
  // TODO: check if these indices take into account the coreset start.

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

interleaved_mapping::interleaved_mapping(const configuration& config_) :
  config(config_), vrb_to_prb_indices(config.nof_rbs), prb_to_vrb_indices(config.nof_rbs)
{
  vrb_to_prb_mapper_get_interleaved_prb(vrb_to_prb_indices,
                                        config.coreset_start,
                                        config.nof_bundles,
                                        config.first_bundle_size,
                                        config.last_bundle_size,
                                        config.other_bundle_size);

  for (size_t vrb = 0; vrb < config.nof_rbs; ++vrb) {
    prb_to_vrb_indices[vrb_to_prb_indices[vrb]] = vrb;
  }
}

std::pair<prb_interval, prb_interval> interleaved_mapping::vrb_to_prb(const vrb_interval& vrbs) const
{
  // TODO: write this function.
  return {static_cast<prb_interval>(vrbs), {}};
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
  prbs.for_each(0, prbs.size(), [&](unsigned prb) { vrbs.set(prb_to_vrb_indices[prb]); });
  return vrbs;
}

prb_bitmap interleaved_mapping::vrb_to_prb(unsigned bwp_size, const vrb_bitmap& vrbs) const
{
  srsran_assert(vrbs.size() == config.nof_rbs,
                "The VRB bitmap size (i.e., {}) doesn't match the size of the interleaving (i.e., {}).",
                vrbs.size(),
                config.nof_rbs);
  srsran_assert(config.coreset_start + vrbs.size() <= bwp_size,
                "This interleaved mapping ({} VRBs with CORESET start at {}) is not suitable for a BWP of size {}.",
                vrbs.size(),
                config.coreset_start,
                bwp_size);

  prb_bitmap prbs(bwp_size);
  vrbs.for_each(0, vrbs.size(), [&](unsigned vrb) { prbs.set(vrb_to_prb_indices[vrb]); });
  return prbs;
}
crb_bitmap interleaved_mapping::vrb_to_crb(unsigned bwp_start, unsigned bwp_size, const vrb_bitmap& vrbs) const
{
  srsran_assert(vrbs.size() == config.nof_rbs,
                "The VRB bitmap size (i.e., {}) doesn't match the size of the interleaving (i.e., {}).",
                vrbs.size(),
                config.nof_rbs);
  srsran_assert(config.coreset_start + vrbs.size() <= bwp_size,
                "This interleaved mapping ({} VRBs with CORESET start at {}) is not suitable for a BWP of size {}.",
                vrbs.size(),
                config.coreset_start,
                bwp_size);

  crb_bitmap crbs(bwp_start + bwp_size);
  vrbs.for_each(0, vrbs.size(), [&](unsigned vrb) { crbs.set(bwp_start + prb_to_vrb_indices[vrb]); });
  return crbs;
}
