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

#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/support/srsgnb_test.h"

namespace srsgnb {
std::unique_ptr<low_papr_sequence_generator> create_low_papr_sequence_generator();
}

using namespace srsgnb;

int main()
{
  std::vector<unsigned>                   sizes = {6, 12, 18, 24, 36, 768};
  std::uniform_int_distribution<unsigned> dist(0, INT32_MAX);

  // Generate alphas.
  std::vector<float> alphas;
  for (unsigned i = 0; i != NRE; ++i) {
    alphas.emplace_back(M_PI_4 * static_cast<float>(i) / static_cast<float>(NRE));
  }

  // Create generator.
  std::unique_ptr<low_papr_sequence_generator> generator = create_low_papr_sequence_generator();

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
    low_papr_sequence_collection collection(*generator, m, delta, alphas);

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
          span<const cf_t> sequence = collection.get(u, v, alpha_idx);

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