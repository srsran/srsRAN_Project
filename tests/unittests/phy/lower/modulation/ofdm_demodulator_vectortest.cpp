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

#include "../../support/resource_grid_test_doubles.h"
#include "ofdm_demodulator_test_data.h"
#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/support/test_utils.h"

/// Defines the maximum allowed error at the OFDM demodulator output.
static constexpr float ASSERT_MAX_ERROR = 1e-4;

using namespace srsran;

int main()
{
  // Create a DFT factory.
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
  if (!dft_factory) {
    dft_factory = create_dft_processor_factory_generic();
  }
  TESTASSERT(dft_factory);

  // Prepare OFDM demodulator factory configuration.
  ofdm_factory_generic_configuration ofdm_common_config;
  ofdm_common_config.dft_factory = dft_factory;

  // Create OFDM demodulator factory.
  std::shared_ptr<ofdm_demodulator_factory> ofdm_factory = create_ofdm_demodulator_factory_generic(ofdm_common_config);
  TESTASSERT(ofdm_factory);

  // Run all defined tests
  for (const test_case_t& test_case : ofdm_demodulator_test_data) {
    resource_grid_writer_spy grid(MAX_PORTS, MAX_NSYMB_PER_SLOT, test_case.test_config.config.bw_rb);

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
