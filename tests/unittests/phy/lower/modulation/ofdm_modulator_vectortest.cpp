/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../support/resource_grid_test_doubles.h"
#include "ofdm_modulator_test_data.h"
#include "srsgnb/phy/generic_functions/generic_functions_factories.h"
#include "srsgnb/phy/lower/modulation/modulation_factories.h"
#include "srsgnb/support/test_utils.h"

/// Defines the maximum allowed error at the OFDM modulator output.
static constexpr float ASSERT_MAX_ERROR = 2e-5;

using namespace srsran;

int main()
{
  // Create a DFT factory.
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_generic();
  TESTASSERT(dft_factory);

  // Create OFDM modulator factory.
  ofdm_factory_generic_configuration ofdm_common_config;
  ofdm_common_config.dft_factory                       = dft_factory;
  std::shared_ptr<ofdm_modulator_factory> ofdm_factory = create_ofdm_modulator_factory_generic(ofdm_common_config);

  // Run all defined tests
  for (const test_case_t& test_case : ofdm_modulator_test_data) {
    unsigned nsubc = test_case.test_config.config.bw_rb * NRE;

    // Create OFDM modulator.
    std::unique_ptr<ofdm_slot_modulator> ofdm = ofdm_factory->create_ofdm_slot_modulator(test_case.test_config.config);
    TESTASSERT(ofdm != nullptr);

    // Load the input data.
    const std::vector<resource_grid_writer_spy::expected_entry_t> data = test_case.data.read();

    // Load the golden data.
    const std::vector<cf_t> modulated = test_case.modulated.read();

    // One slot per subframe will be tested.
    unsigned input_offset = 0;
    // Map the read data into a reader_spy structure.
    resource_grid_reader_spy rg;
    unsigned                 nsymb = get_nsymb_per_slot(test_case.test_config.config.cp);
    for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
      for (unsigned subc_idx = 0; subc_idx != nsubc; ++subc_idx) {
        resource_grid_reader_spy::expected_entry_t entry = {};
        entry.port                                       = data[input_offset + subc_idx].port;
        entry.symbol                                     = data[input_offset + subc_idx].symbol;
        entry.subcarrier                                 = data[input_offset + subc_idx].subcarrier;
        entry.value                                      = data[input_offset + subc_idx].value;
        rg.write(entry);
      }
      input_offset += nsubc;
    }

    // Modulate signal.
    std::vector<cf_t> output(ofdm->get_slot_size(test_case.test_config.slot_idx));
    ofdm->modulate(output, rg, test_case.test_config.port_idx, test_case.test_config.slot_idx);

    for (unsigned i = 0; i != modulated.size(); ++i) {
      float error =
          std::abs(output[i] - modulated[i]) / std::sqrt(static_cast<float>(test_case.test_config.config.dft_size));
      TESTASSERT(error < ASSERT_MAX_ERROR,
                 "Sample index {} error {} exceeds maximum allowed ({}). Expedted symbol {} but got {}.",
                 i,
                 error,
                 ASSERT_MAX_ERROR,
                 modulated[i],
                 output[i]);
    }
  }

  return 0;
}
