/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../support/resource_grid_test_doubles.h"
#include "ofdm_demodulator_test_data.h"
#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/phy/lower/modulation/modulation_factories.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>

namespace srsran {

static std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "numerology={} bw_rb={} dft_size={} cp={} wo={} sf={} cf={:.3f}MHz",
             test_case.test_config.config.numerology,
             test_case.test_config.config.bw_rb,
             test_case.test_config.config.dft_size,
             test_case.test_config.config.cp.to_string(),
             test_case.test_config.config.nof_samples_window_offset,
             test_case.test_config.config.scale,
             test_case.test_config.config.center_freq_hz / 1e6);
  return os;
}

} // namespace srsran

using namespace srsran;

class ofdm_demodulator_tester : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<ofdm_slot_demodulator> demodulator;

  void SetUp() override
  {
    const test_case_t& test_case = GetParam();

    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_generic();
    ASSERT_TRUE(dft_factory);

    ofdm_factory_generic_configuration factory_config = {.dft_factory = dft_factory};

    std::shared_ptr<ofdm_demodulator_factory> ofdm_factory = create_ofdm_demodulator_factory_generic(factory_config);
    ASSERT_TRUE(ofdm_factory);

    demodulator = ofdm_factory->create_ofdm_slot_demodulator(test_case.test_config.config);
    ASSERT_TRUE(demodulator);
  }
};

TEST_P(ofdm_demodulator_tester, vector)
{
  const test_case_t& test_case = GetParam();

  resource_grid_writer_spy grid(MAX_PORTS, MAX_NSYMB_PER_SLOT, test_case.test_config.config.bw_rb);

  // Load the input data.
  std::vector<cf_t> data(demodulator->get_slot_size(test_case.test_config.slot_idx));
  data = test_case.data.read();

  // Demodulate signal.
  demodulator->demodulate(grid, data, test_case.test_config.port_idx, test_case.test_config.slot_idx);

  // Load the golden data.
  const std::vector<resource_grid_writer_spy::expected_entry_t> demodulated = test_case.demodulated.read();

  // Assert resource grid entries.
  grid.assert_entries(demodulated);
}

INSTANTIATE_TEST_SUITE_P(ofdm_demodulator_vectortest,
                         ofdm_demodulator_tester,
                         ::testing::ValuesIn(ofdm_demodulator_test_data));
