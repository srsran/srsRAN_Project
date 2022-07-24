/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prach_generator_test_data.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

static constexpr float ASSERT_MAX_ERROR = 1E-4F;

int main()
{
  unsigned                               nof_prb_ul_grid = 0;
  unsigned                               dft_size_15kHz  = 0;
  std::shared_ptr<dft_processor_factory> dft_factory     = create_dft_processor_factory_generic();
  TESTASSERT(dft_factory);

  std::unique_ptr<prach_generator> prach_generator = nullptr;

  for (const test_case_t& test_case : prach_generator_test_data) {
    // Detect if the UL grid size or DFT size for 15kHz have changed.
    if (test_case.context.nof_prb_ul_grid != nof_prb_ul_grid || test_case.context.dft_size_15kHz != dft_size_15kHz) {
      nof_prb_ul_grid = test_case.context.nof_prb_ul_grid;
      dft_size_15kHz  = test_case.context.dft_size_15kHz;

      std::shared_ptr<prach_generator_factory> prach_gen_factory =
          create_prach_generator_factory_sw(dft_factory, nof_prb_ul_grid, dft_size_15kHz);
      TESTASSERT(prach_gen_factory);

      prach_generator = prach_gen_factory->create();
      TESTASSERT(prach_generator);
    }

    span<const cf_t> sequence = prach_generator->generate(test_case.context.config);

    const std::vector<cf_t> expected = test_case.sequence.read();

    TESTASSERT_EQ(sequence.size(),
                  expected.size(),
                  "Preamble format {}.",
                  static_cast<unsigned>(test_case.context.config.format));
    for (unsigned i = 0; i != sequence.size(); ++i) {
      float error = std::abs(sequence[i] - expected[i]);
      TESTASSERT(error < ASSERT_MAX_ERROR,
                 "Error {} exceeds maximum for sample {}. Sequence generated {} but expected {}.",
                 error,
                 i,
                 sequence[i],
                 expected[i]);
    }
  }

  return 0;
}
