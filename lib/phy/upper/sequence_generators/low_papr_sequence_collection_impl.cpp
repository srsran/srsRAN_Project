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

#include "low_papr_sequence_collection_impl.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

low_papr_sequence_collection_impl::low_papr_sequence_collection_impl(const low_papr_sequence_generator& generator,
                                                                     unsigned                           m,
                                                                     unsigned                           delta,
                                                                     span<const float>                  alphas)

{
  // Calculate sequence length.
  unsigned M_zc = (NRE * m) / pow2(delta);

  // Calculate number of bases.
  unsigned nof_bases = MAX_NOF_BASES;
  if ((NRE / 2) <= M_zc && M_zc <= (5 * NRE)) {
    nof_bases = 1;
  }

  unsigned nof_alphas = alphas.size();

  pregen_signals.reserve({M_zc, NOF_GROUPS, nof_bases, nof_alphas});

  // Create sequence collection for all possible of u, v and alpha index values.
  for (unsigned u = 0; u != NOF_GROUPS; ++u) {
    for (unsigned v = 0; v != nof_bases; ++v) {
      for (unsigned alpha_idx = 0; alpha_idx != nof_alphas; ++alpha_idx) {
        // Generate a sequence for each alpha.
        generator.generate(pregen_signals.get_view<1>({u, v, alpha_idx}), u, v, alphas[alpha_idx]);
      }
    }
  }
}

span<const cf_t> low_papr_sequence_collection_impl::get(unsigned u, unsigned v, unsigned alpha_idx) const
{
  std::array<unsigned, 4> dimensions = pregen_signals.get_dimensions_size();
  std::array<unsigned, 3> indexes    = {u, v, alpha_idx};

  // Make sure the indexes are valid.
  srsran_assert(u < dimensions[1] && v < dimensions[2] && alpha_idx < dimensions[3],
                "The sequence collection was initialized with dimension sizes {} and it is trying to access {}",
                span<const unsigned>(dimensions),
                span<const unsigned>(indexes));

  // Return the vector view of the sequence.
  return pregen_signals.get_view<1>({u, v, alpha_idx});
}
