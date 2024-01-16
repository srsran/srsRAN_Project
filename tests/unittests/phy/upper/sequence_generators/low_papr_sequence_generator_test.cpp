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

#include "srsran/phy/constants.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/srsvec/compare.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;

int main()
{
  std::vector<unsigned>                   sizes = {6, 12, 18, 24, 36, 768};
  std::uniform_int_distribution<unsigned> dist(0, INT32_MAX);

  // Generate alphas.
  std::array<float, NRE> alphas;
  std::generate(alphas.begin(), alphas.end(), [&, n = 0]() mutable {
    return M_PI_4 * static_cast<float>(n++) / static_cast<float>(NRE);
  });

  // Create generator factory.
  std::shared_ptr<low_papr_sequence_generator_factory> generator_factory =
      create_low_papr_sequence_generator_sw_factory();
  TESTASSERT(generator_factory);

  std::shared_ptr<low_papr_sequence_collection_factory> collection_factory =
      create_low_papr_sequence_collection_sw_factory(generator_factory);
  TESTASSERT(collection_factory);

  // Create generator.
  std::unique_ptr<low_papr_sequence_generator> generator = generator_factory->create();
  TESTASSERT(generator);

  // For each possible size...
  for (unsigned M_zc : sizes) {
    // Calculate parameter m and delta.
    unsigned m      = divide_ceil(M_zc, NRE);
    unsigned delta  = 0;
    unsigned trials = 4;
    while ((m * NRE) / pow2(delta) != M_zc) {
      if ((m * NRE) / pow2(delta) < M_zc) {
        m *= 3;
      } else {
        delta++;
      }

      // Count calculation trial and make sure it does not exceed the maximum number of trials.
      --trials;
      TESTASSERT(trials, "Size M_zc={} cannot derive m and delta.", M_zc);
    }

    // Create collection.
    std::unique_ptr<low_papr_sequence_collection> collection = collection_factory->create(m, delta, alphas);

    // Iterate all possible groups.
    for (unsigned u = 0; u < 30; ++u) {
      unsigned nof_bases = 2;
      if ((NRE / 2) <= M_zc && M_zc <= (5 * NRE)) {
        nof_bases = 1;
      }

      // Iterate all possible bases.
      for (unsigned v = 0; v < nof_bases; ++v) {
        // Iterate all alpha indexes.
        for (unsigned alpha_idx = 0; alpha_idx != alphas.size(); ++alpha_idx) {
          // Get sequence from collection
          span<const cf_t> sequence = collection->get(u, v, alpha_idx);

          // Generate sequence.
          std::vector<cf_t> sequence2(M_zc);
          generator->generate(sequence2, u, v, alphas[alpha_idx]);

          // Assert sequence matches with generator.
          TESTASSERT(srsvec::equal(sequence, sequence2), "Collected and generated sequences do not match.");

          // Assert the sequence with an external reference.
          // ...
        }
      }
    }
  }
}