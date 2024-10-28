/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../support/resource_grid_test_doubles.h"
#include "ofdm_modulator_test_data.h"
#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/support/test_utils.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>

namespace srsran {

static std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "numerology={} bw_rb={} dft_size={} cp={} sf={} cf={:.3f}MHz",
             test_case.test_config.config.numerology,
             test_case.test_config.config.bw_rb,
             test_case.test_config.config.dft_size,
             test_case.test_config.config.cp.to_string(),
             test_case.test_config.config.scale,
             test_case.test_config.config.center_freq_hz / 1e6);
  return os;
}

} // namespace srsran

using namespace srsran;

class ofdm_modulator_tester : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<ofdm_slot_modulator> modulator;

  void SetUp() override
  {
    const test_case_t& test_case = GetParam();

    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_generic();
    ASSERT_TRUE(dft_factory);

    ofdm_factory_generic_configuration factory_config = {.dft_factory = dft_factory};

    std::shared_ptr<ofdm_modulator_factory> ofdm_factory = create_ofdm_modulator_factory_generic(factory_config);
    ASSERT_TRUE(ofdm_factory);

    modulator = ofdm_factory->create_ofdm_slot_modulator(test_case.test_config.config);
    ASSERT_TRUE(modulator);
  }

  static float get_max_abs_error(cf_t expected) { return std::abs(expected) / 128; }
};

TEST_P(ofdm_modulator_tester, vector)
{
  const test_case_t& test_case = GetParam();
  unsigned           nsubc     = test_case.test_config.config.bw_rb * NRE;

  // Load the input data.
  const std::vector<resource_grid_writer_spy::expected_entry_t> data = test_case.data.read();

  // Load the golden data.
  const std::vector<cf_t> expected = test_case.modulated.read();

  // One slot per subframe will be tested.
  unsigned input_offset = 0;
  // Map the read data into a reader_spy structure.
  unsigned                 nsymb = get_nsymb_per_slot(test_case.test_config.config.cp);
  resource_grid_reader_spy rg;
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
  std::vector<cf_t> output(modulator->get_slot_size(test_case.test_config.slot_idx));
  modulator->modulate(output, rg, test_case.test_config.port_idx, test_case.test_config.slot_idx);

  for (unsigned i = 0; i != expected.size(); ++i) {
    float max_error = get_max_abs_error(expected[i]);
    float error     = std::abs(output[i] - expected[i]);
    TESTASSERT(error < max_error,
               "Sample index {} error {} exceeds maximum allowed ({}). Expected symbol {} but got {}.",
               i,
               error,
               max_error,
               expected[i],
               output[i]);
  }
}

INSTANTIATE_TEST_SUITE_P(ofdm_modulator_vectortest,
                         ofdm_modulator_tester,
                         ::testing::ValuesIn(ofdm_modulator_test_data));
