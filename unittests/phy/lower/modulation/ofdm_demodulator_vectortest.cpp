/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../resource_grid_test_doubles.h"
#include "ofdm_demodulator_test_data.h"
#include "srsgnb/phy/lower/modulation/modulation_factories.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/support/test_utils.h"

/// Defines the maximum allowed error at the OFDM demodulator output.
static constexpr float ASSERT_MAX_ERROR = 1e-5;

using namespace srsgnb;

int main()
{
  // Create a DFT factory.
  dft_processor_factory_fftw_configuration dft_factory_config;
  dft_factory_config.avoid_wisdom                    = false;
  dft_factory_config.wisdom_filename                 = "";
  std::unique_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw(dft_factory_config);
  TESTASSERT(dft_factory);

  // Prepare OFDM demodulator factory configuration.
  ofdm_factory_generic_configuration ofdm_factory_config;
  ofdm_factory_config.dft_factory = dft_factory.get();

  // Create OFDM demodulator factory.
  std::unique_ptr<ofdm_demodulator_factory> ofdm_factory = create_ofdm_demodulator_factory_generic(ofdm_factory_config);
  TESTASSERT(ofdm_factory);

  // Run all defined tests
  for (const test_case_t& test_case : ofdm_demodulator_test_data) {
    resource_grid_writer_spy grid;

    // Create FFTW configuration;
    dft_processor::configuration config;
    config.size = test_case.test_config.config.dft_size;
    config.dir  = dft_processor::direction::DIRECT;

    // Create DFT processor
    std::unique_ptr<dft_processor> dft = dft_factory->create(config);

    // Create OFDM modulator.
    std::unique_ptr<ofdm_slot_demodulator> ofdm =
        ofdm_factory->create_ofdm_slot_demodulator(test_case.test_config.config);
    TESTASSERT(ofdm != nullptr);

    // Load the input data.
    std::vector<cf_t> data(ofdm->get_slot_size(test_case.test_config.slot_idx));
    data = test_case.data.read();

    // Demodulate signal.
    ofdm->demodulate(grid, data, test_case.test_config.port_idx, test_case.test_config.slot_idx);

    // Load the golden data.
    const std::vector<resource_grid_writer_spy::expected_entry_t> demodulated = test_case.demodulated.read();

    // Assert resource grid entries.
    grid.assert_entries(demodulated, ASSERT_MAX_ERROR);
  }

  return 0;
}
