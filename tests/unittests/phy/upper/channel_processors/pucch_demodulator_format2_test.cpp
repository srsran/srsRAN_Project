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

#include "pucch_demodulator_format2_test_data.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

using PucchDemodulatorParams = test_case_t;

namespace srsran {

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os,
             "ports=[{}] first_prb={} nof_prb={} start_symbol_index={} nof_symbols={} rnti={} n_id={}",
             span<const uint8_t>(test_case.context.config.rx_ports),
             test_case.context.config.first_prb,
             test_case.context.config.nof_prb,
             test_case.context.config.start_symbol_index,
             test_case.context.config.nof_symbols,
             test_case.context.config.rnti,
             test_case.context.config.n_id);
  return os;
}

static bool operator==(const std::vector<log_likelihood_ratio>& a, const std::vector<log_likelihood_ratio>& b)
{
  return std::equal(a.cbegin(), a.cend(), b.cbegin(), [](log_likelihood_ratio x, log_likelihood_ratio y) {
    return ((x - y >= -1) && (x - y <= 1));
  });
}

} // namespace srsran

class PucchDemodulatorFixture : public ::testing::TestWithParam<PucchDemodulatorParams>
{
protected:
  pucch_demodulator::format2_configuration config;
  std::vector<log_likelihood_ratio>        uci_expected;
  channel_estimate                         channel_est;
  resource_grid_reader_spy                 rg_spy;

  // PUCCH Demodulator.
  static std::unique_ptr<pucch_demodulator> demodulator;

  static void SetUpTestSuite()
  {
    if (!demodulator) {
      // Create factories required by the PUCCH demodulator factory.
      std::shared_ptr<channel_equalizer_factory> equalizer_factory = create_channel_equalizer_factory_zf();
      ASSERT_NE(equalizer_factory, nullptr) << "Cannot create equalizer factory";

      std::shared_ptr<channel_modulation_factory> demod_factory = create_channel_modulation_sw_factory();
      ASSERT_NE(demod_factory, nullptr) << "Cannot create channel modulation factory";

      std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
      ASSERT_NE(prg_factory, nullptr) << "Cannot create pseudo-random generator factory";

      // Create PUCCH demodulator factory.
      std::shared_ptr<pucch_demodulator_factory> pucch_demod_factory =
          create_pucch_demodulator_factory_sw(equalizer_factory, demod_factory, prg_factory);
      ASSERT_NE(pucch_demod_factory, nullptr) << "Cannot create PUCCH demodulator factory.";

      // Create PUCCH demodulator.
      demodulator = pucch_demod_factory->create();
      ASSERT_NE(demodulator, nullptr) << "Cannot create PUCCH demodulator.";
    }
  }

  void SetUp() override
  {
    const test_case_t& test_case = GetParam();

    // Assert PUCCH demodulator.
    ASSERT_NE(demodulator, nullptr) << "Cannot create PUCCH demodulator.";

    // Prepare PUCCH demodulator configuration.
    config = test_case.context.config;

    // Prepare expected UCI codeword.
    uci_expected = test_case.uci_codeword.read();

    // Determine resource grid dimensions.
    unsigned nof_prb      = test_case.context.grid_nof_prb;
    unsigned nof_symbols  = test_case.context.grid_nof_symbols;
    unsigned nof_rx_ports = config.rx_ports.size();

    // Prepare the resource grid.
    unsigned nof_test_symbols = test_case.context.config.rx_ports.size() * test_case.context.config.nof_symbols *
                                test_case.context.config.nof_prb * pucch_constants::FORMAT2_NOF_DATA_SC;

    std::vector<resource_grid_reader_spy::expected_entry_t> grid_entries = test_case.symbols.read();
    ASSERT_EQ(grid_entries.size(), nof_test_symbols)
        << "The number of grid entries and the number of PUCCH REs do not match";

    rg_spy.write(grid_entries);

    // Prepare channel estimates.
    channel_estimate::channel_estimate_dimensions ce_dims;
    ce_dims.nof_prb       = nof_prb;
    ce_dims.nof_symbols   = nof_symbols;
    ce_dims.nof_rx_ports  = nof_rx_ports;
    ce_dims.nof_tx_layers = 1;

    channel_est.resize(ce_dims);

    // Set estimated channel.
    std::vector<cf_t> estimates = test_case.estimates.read();
    srsvec::copy(channel_est.get_path_ch_estimate(0, 0), estimates);

    // Set noise variance.
    channel_est.set_noise_variance(test_case.context.noise_var, 0);
  }

  ~PucchDemodulatorFixture() = default;
};

std::unique_ptr<pucch_demodulator> PucchDemodulatorFixture::demodulator = nullptr;

TEST_P(PucchDemodulatorFixture, PucchDemodulatorVectorTest)
{
  std::vector<log_likelihood_ratio> uci_data(uci_expected.size());

  // Perform demodulation.
  demodulator->demodulate(uci_data, rg_spy, channel_est, config);

  // Assert UCI codeword matches.
  ASSERT_EQ(uci_expected, uci_data);
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PucchDemodulatorVectorTest,
                         PucchDemodulatorFixture,
                         ::testing::ValuesIn(pucch_demodulator_format2_test_data));
