/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../../lib/phy/upper/signal_processors/pucch/pucch_orthogonal_sequence.h"
#include "../../resource_grid_test_doubles.h"
#include "dmrs_pucch_processor_test_data.h"
#include "srsgnb/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pucch_processor.h"

using namespace srsgnb;

// forward-declare
namespace srsgnb {
std::unique_ptr<low_papr_sequence_generator> create_low_papr_sequence_generator();
}

int main()
{
  // Fixed BW
  const unsigned nof_prb = 52;

  // Create low PAPR sequence generator.
  std::unique_ptr<low_papr_sequence_generator> generator = create_low_papr_sequence_generator();

  // Low PAPR sequence parameters, assumes M_zc = 12
  unsigned m     = 1;
  unsigned delta = 0;
  // Generate alphas for 1PRB (see TS38.211 clause 6.3.2.2.2).
  std::vector<float> alphas_1prb;
  for (unsigned i = 0; i != NRE; ++i) {
    alphas_1prb.emplace_back(2U * M_PI * static_cast<float>(i) / static_cast<float>(NRE));
  }

  // Pre-generate a low PAPR sequence collection.
  low_papr_sequence_collection collection(*generator, m, delta, alphas_1prb);

  // Pre-generate orthogonal cover sequences.
  pucch_orthogonal_sequence orthogonal_sequences;

  // Iterate over test cases.
  for (const test_case_t& test_case : dmrs_pucch_processor_test_data) {
    // Create DMRS-PUCCH processor.
    std::unique_ptr<dmrs_pucch_processor> dmrs_pucch =
        create_dmrs_pucch_processor(test_case.config.format, &collection, &orthogonal_sequences);

    // Load Matlab generated symbols.
    const std::vector<resource_grid_reader_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Create resource grid
    resource_grid_reader_spy grid;
    grid.write(testvector_symbols);

    // Object to store channel estimation results
    channel_estimate::channel_estimate_dimensions ch_est_dims = {};
    ch_est_dims.nof_prb                                       = nof_prb;
    ch_est_dims.nof_symbols                                   = get_nsymb_per_slot(test_case.config.cp);
    ch_est_dims.nof_tx_layers                                 = 1;
    ch_est_dims.nof_rx_antennas                               = 1;
    channel_estimate estimate(ch_est_dims);

    // Estimate channel
    dmrs_pucch->estimate(estimate, grid, test_case.config);

    srsran_assert(estimate.noise_variance[0] < 1e-6, "Expected an ideal channel!");
  }
}
