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
#include "dmrs_pusch_estimator_test_data.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"
#include "srsran/srsvec/zero.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, dmrs_pusch_estimator::configuration config)
{
  fmt::print(os,
             "slot={}; type={}; scrambling_id={}; n_scid={}; scaling={:.3f}; cp={}; dmrs_pos={}; f_alloc={}; "
             "t_alloc={}:{}; tx_layers={}; rx_ports={};",
             config.slot,
             config.type == dmrs_type::TYPE1 ? "1" : "2",
             config.scrambling_id,
             config.n_scid,
             config.scaling,
             config.c_prefix.to_string(),
             config.symbols_mask,
             config.rb_mask,
             config.first_symbol,
             config.nof_symbols,
             config.nof_tx_layers,
             span<const uint8_t>(config.rx_ports));
  return os;
}

std::ostream& operator<<(std::ostream& os, test_label label)
{
  std::string text = "DM-RS creation";
  if (label == test_label::ch_estimation) {
    text = "CH estimation";
  }
  fmt::print(os, text);
  return os;
}

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os, "{} - config={} symbols={};", test_case.label, test_case.config, test_case.rx_symbols.get_file_name());
  return os;
}

} // namespace srsran

namespace {

class DmrsPuschEstimatorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<dmrs_pusch_estimator> estimator;
  resource_grid_reader_spy              grid;

  void SetUp() override
  {
    test_case_t test_case = GetParam();

    // Create PRG.
    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    ASSERT_TRUE(prg_factory);

    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
    if (!dft_factory) {
      dft_factory = create_dft_processor_factory_generic();
    }
    ASSERT_NE(dft_factory, nullptr) << "Cannot create DFT factory.";

    std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory =
        create_time_alignment_estimator_dft_factory(dft_factory);
    ASSERT_NE(ta_estimator_factory, nullptr) << "Cannot create TA estimator factory.";

    // Create port estimator.
    std::shared_ptr<port_channel_estimator_factory> port_estimator_factory =
        create_port_channel_estimator_factory_sw(ta_estimator_factory);
    ASSERT_TRUE(port_estimator_factory);

    // Create estimator factory.
    std::shared_ptr<dmrs_pusch_estimator_factory> estimator_factory =
        create_dmrs_pusch_estimator_factory_sw(prg_factory, port_estimator_factory);
    ASSERT_TRUE(estimator_factory);

    // Create actual channel estimator.
    estimator = estimator_factory->create();
    ASSERT_TRUE(estimator);

    // Setup resource grid symbols.
    std::vector<resource_grid_reader_spy::expected_entry_t> rg_entries = test_case.rx_symbols.read();
    grid.write(rg_entries);
  }
};
} // namespace

static constexpr float tolerance = 0.001;

TEST_P(DmrsPuschEstimatorFixture, Creation)
{
  // This test only looks to whether the estimator places the DM-RS pilots in the correct position. To this end, the
  // received channel samples have been generated assuming that layers are transmitted on orthogonal channels (one layer
  // - one port), with no impairments. Then, the estimated channel should be one at the pilot coordinates and zero
  // elsewhere.

  dmrs_pusch_estimator::configuration config = GetParam().config;

  // This is a 'dmrs-position' test.
  if (GetParam().label != test_label::dmrs_creation) {
    GTEST_SKIP() << "Not a DM-RS creation test, skipping.";
  }

  // The current estimator does not support Type2 DM-RS.
  // As well, the MIMO case must be double-checked.
  if ((config.type == dmrs_type::TYPE2) || (config.nof_tx_layers > 1)) {
    GTEST_SKIP() << "Configuration not supported yet, skipping.";
  }

  ASSERT_EQ(config.rx_ports.size(), config.nof_tx_layers)
      << "This simulation assumes an equal number of Rx ports and Tx layers.";

  // Prepare channel estimate (just to be sure, reset all entries).
  channel_estimate::channel_estimate_dimensions ch_estimate_dims;
  ch_estimate_dims.nof_prb       = config.rb_mask.size();
  ch_estimate_dims.nof_symbols   = config.nof_symbols + config.first_symbol;
  ch_estimate_dims.nof_rx_ports  = config.rx_ports.size();
  ch_estimate_dims.nof_tx_layers = config.nof_tx_layers;

  channel_estimate ch_est(ch_estimate_dims);

  for (unsigned i_port = 0; i_port != ch_estimate_dims.nof_rx_ports; ++i_port) {
    for (unsigned i_layer = 0; i_layer != ch_estimate_dims.nof_tx_layers; ++i_layer) {
      span<cf_t> path = ch_est.get_path_ch_estimate(i_port, i_layer);
      srsvec::zero(path);
    }
  }

  // Estimate.
  estimator->estimate(ch_est, grid, config);

  // First, assert the channel estimate dimensions haven't changed.
  ch_estimate_dims = ch_est.size();
  ASSERT_EQ(ch_estimate_dims.nof_prb, config.rb_mask.size()) << "Wrong number of PRBs.";
  ASSERT_EQ(ch_estimate_dims.nof_symbols, config.nof_symbols + config.first_symbol) << "Wrong number of symbols.";
  ASSERT_EQ(ch_estimate_dims.nof_rx_ports, config.rx_ports.size()) << "Wrong number of Rx ports.";
  ASSERT_EQ(ch_estimate_dims.nof_tx_layers, config.nof_tx_layers) << "Wrong number of Tx layers.";

  for (unsigned i_port = 0; i_port != ch_estimate_dims.nof_rx_ports; ++i_port) {
    for (unsigned i_layer = 0; i_layer != ch_estimate_dims.nof_tx_layers; ++i_layer) {
      for (unsigned i_symbol = 0; i_symbol != ch_estimate_dims.nof_symbols; ++i_symbol) {
        span<const cf_t> current_symbol = ch_est.get_symbol_ch_estimate(i_symbol, i_port, i_layer);
        if ((i_port != i_layer) || (i_symbol < config.first_symbol)) {
          ASSERT_TRUE(std::all_of(current_symbol.begin(), current_symbol.end(), [](cf_t a) {
            return (a == cf_t(0, 0));
          })) << "REs should be zero on cross paths and on not allocated symbols.";
          continue;
        }
        cf_t value        = cf_t(1, 0);
        bool is_ok        = true;
        auto check_symbol = [current_symbol, &value, &is_ok](unsigned i_prb) {
          unsigned         i_re        = i_prb * NRE;
          span<const cf_t> current_prb = current_symbol.subspan(i_re, NRE);
          is_ok                        = is_ok && std::all_of(current_prb.begin(), current_prb.end(), [value](cf_t a) {
                    return (std::abs(a - value) < tolerance);
                  });
        };
        config.rb_mask.for_each(0, config.rb_mask.size(), check_symbol);
        ASSERT_TRUE(is_ok) << "All estimates in allocated REs should be 1.";

        value = cf_t(0, 0);
        is_ok = true;
        config.rb_mask.for_each(0, config.rb_mask.size(), check_symbol, false);
        ASSERT_TRUE(is_ok) << "All estimates in non-allocated REs should be 0.";
      }
    }
  }
}

static bool are_estimates_ok(span<const resource_grid_reader_spy::expected_entry_t> expected,
                             const channel_estimate&                                computed)
{
  unsigned         old_symbol = 15;
  span<const cf_t> computed_symbol;

  for (const auto& this_expected : expected) {
    unsigned i_symbol = this_expected.symbol;
    unsigned i_sc     = this_expected.subcarrier;
    cf_t     value    = this_expected.value;

    if (i_symbol != old_symbol) {
      old_symbol      = i_symbol;
      computed_symbol = computed.get_symbol_ch_estimate(i_symbol, 0, 0);
    }

    if (std::abs(computed_symbol[i_sc] - value) > tolerance) {
      return false;
    }
  }
  return true;
}

TEST_P(DmrsPuschEstimatorFixture, Average)
{
  dmrs_pusch_estimator::configuration config = GetParam().config;

  // This is a 'dmrs-position' test.
  if (GetParam().label != test_label::ch_estimation) {
    GTEST_SKIP() << "Not a channel estimation test, skipping.";
  }

  // The current estimator does not support Type2 DM-RS.
  // As well, the MIMO case must be double-checked.
  if ((config.type == dmrs_type::TYPE2) || (config.nof_tx_layers > 1)) {
    GTEST_SKIP() << "Configuration not supported yet, skipping.";
  }

  unsigned nof_allocated_res = config.rb_mask.count() * NRE * config.nof_symbols;

  // Read expected channel estimates.
  std::vector<resource_grid_reader_spy::expected_entry_t> expected_estimates = GetParam().ch_estimates.read();
  ASSERT_EQ(expected_estimates.size(), nof_allocated_res) << fmt::format(
      "Number of channel estimates mismatch: configured {}, provided {}", nof_allocated_res, expected_estimates.size());

  channel_estimate ch_est;

  // Estimate.
  estimator->estimate(ch_est, grid, config);

  // First, assert the channel estimate gets the proper dimensions.
  channel_estimate::channel_estimate_dimensions ch_estimate_dims = ch_est.size();
  ASSERT_EQ(ch_estimate_dims.nof_prb, config.rb_mask.size()) << "Wrong number of PRBs.";
  ASSERT_EQ(ch_estimate_dims.nof_symbols, config.nof_symbols + config.first_symbol) << "Wrong number of symbols.";
  ASSERT_EQ(ch_estimate_dims.nof_rx_ports, config.rx_ports.size()) << "Wrong number of Rx ports.";
  ASSERT_EQ(ch_estimate_dims.nof_tx_layers, config.nof_tx_layers) << "Wrong number of Tx layers.";

  // Assert that the channel estimates are correct.
  ASSERT_TRUE(are_estimates_ok(expected_estimates, ch_est));
  ASSERT_NEAR(ch_est.get_rsrp(0, 0), GetParam().est_rsrp, tolerance);
}

// Creates test suite with all the test cases.
INSTANTIATE_TEST_SUITE_P(DmrsPuschEstimatorTest,
                         DmrsPuschEstimatorFixture,
                         ::testing::ValuesIn(dmrs_pusch_estimator_test_data));
