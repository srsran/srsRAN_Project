/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../../../lib/phy/upper/signal_processors/pucch/pucch_orthogonal_sequence.h"
#include "../../support/resource_grid_test_doubles.h"
#include "dmrs_pucch_processor_test_data.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"

using namespace srsran;

int main()
{
  // Fixed BW
  const unsigned nof_prb = 52;

  // Create pseudo-random sequence generator factory.
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  // Create low PAPR sequence generator factory.
  std::shared_ptr<low_papr_sequence_generator_factory> lpg_factory = create_low_papr_sequence_generator_sw_factory();
  TESTASSERT(lpg_factory);

  // Create low PAPR collection factory.
  std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory =
      create_low_papr_sequence_collection_sw_factory(lpg_factory);
  TESTASSERT(lpc_factory);

  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw();
  if (!dft_factory) {
    dft_factory = create_dft_processor_factory_generic();
  }
  TESTASSERT(dft_factory, "Cannot create DFT factory.");

  // Create channel estimator factory.
  std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
      create_port_channel_estimator_factory_sw(dft_factory);
  TESTASSERT(port_chan_estimator_factory);

  // Create DM-RS for PUCCH estimator factory.
  std::shared_ptr<dmrs_pucch_estimator_factory> estimator_factory =
      create_dmrs_pucch_estimator_factory_sw(prg_factory, lpc_factory, port_chan_estimator_factory);
  TESTASSERT(estimator_factory);

  // Iterate over test cases.
  for (const test_case_t& test_case : dmrs_pucch_processor_test_data) {
    // Create DMRS-PUCCH processor.
    std::unique_ptr<dmrs_pucch_processor> dmrs_pucch = nullptr;
    switch (test_case.config.format) {
      case pucch_format::FORMAT_1:
        dmrs_pucch = estimator_factory->create_format1();
        break;
      case pucch_format::FORMAT_2:
        dmrs_pucch = estimator_factory->create_format2();
      case pucch_format::FORMAT_0:
      case pucch_format::FORMAT_3:
      case pucch_format::FORMAT_4:
      case pucch_format::NOF_FORMATS:
      default:
        break;
    }
    TESTASSERT(dmrs_pucch);

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
    ch_est_dims.nof_rx_ports                                  = 1;
    channel_estimate estimate(ch_est_dims);

    // Estimate channel
    dmrs_pucch->estimate(estimate, grid, test_case.config);

    srsran_assert(estimate.get_noise_variance(0) < 1e-2, "Expected an ideal channel!");
  }
}
