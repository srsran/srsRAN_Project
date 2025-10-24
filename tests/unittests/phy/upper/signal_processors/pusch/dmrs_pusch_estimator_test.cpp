/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../../../support/resource_grid_test_doubles.h"
#include "dmrs_pusch_estimator_test_data.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_phy_capabilities.h"
#include "srsran/phy/upper/signal_processors/pusch/factories.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/executors/inline_task_executor.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, dmrs_pusch_estimator::configuration config)
{
  fmt::print(os,
             "slot={}; type={}; scaling={:.3f}; cp={}; dmrs_pos={}; f_alloc={}; "
             "t_alloc={}:{}; tx_layers={}; rx_ports=[{}];",
             config.slot,
             config.get_dmrs_type() == dmrs_type::TYPE1 ? "1" : "2",
             config.scaling,
             config.c_prefix.to_string(),
             config.symbols_mask,
             config.rb_mask,
             config.first_symbol,
             config.nof_symbols,
             config.get_nof_tx_layers(),
             span<const uint8_t>(config.rx_ports));
  return os;
}

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os, "config={} symbols={};", test_case.config, test_case.rx_symbols.get_file_name());
  return os;
}

} // namespace srsran

template <>
struct fmt::formatter<srsran::dmrs_pusch_estimator::configuration> : ostream_formatter {};

namespace {

class dmrs_pusch_estimator_notifier_spy : public dmrs_pusch_estimator_notifier
{
public:
  void on_estimation_complete() override { ++estimation_notified; }

  bool has_notified() const { return (estimation_notified == 1); }

private:
  unsigned estimation_notified = 0;
};

class DmrsPuschEstimatorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<dmrs_pusch_estimator> estimator;
  resource_grid_reader_spy              grid;
  inline_task_executor                  ch_est_executor;

  // Default constructor - initializes the resource grid with the maximum size possible.
  DmrsPuschEstimatorFixture() : ::testing::TestWithParam<ParamType>(), grid(MAX_PORTS, MAX_NSYMB_PER_SLOT, MAX_NOF_PRBS)
  {
  }

  void SetUp() override
  {
    test_case_t test_case = GetParam();

    // Create PRG.
    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    ASSERT_TRUE(prg_factory);

    // Create low-PAPR sequence generator.
    std::shared_ptr<low_papr_sequence_generator_factory> low_papr_sequence_gen_factory_factory =
        create_low_papr_sequence_generator_sw_factory();
    ASSERT_TRUE(low_papr_sequence_gen_factory_factory);

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
        create_dmrs_pusch_estimator_factory_sw(prg_factory,
                                               low_papr_sequence_gen_factory_factory,
                                               port_estimator_factory,
                                               ch_est_executor,
                                               port_channel_estimator_fd_smoothing_strategy::filter,
                                               port_channel_estimator_td_interpolation_strategy::average,
                                               true);
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

static constexpr float tolerance = 0.01;

TEST_P(DmrsPuschEstimatorFixture, Creation)
{
  // This test only looks to whether the estimator places the DM-RS pilots in the correct position. To this end, the
  // received channel samples have been generated assuming that layers are transmitted on orthogonal channels (one layer
  // - one port), with no impairments. Then, the estimated channel should be one at the pilot coordinates and zero
  // elsewhere.

  dmrs_pusch_estimator::configuration config = GetParam().config;

  // The current estimator does not support Type2 DM-RS.
  // As well, the MIMO case only works with at most two layers.
  if ((config.get_dmrs_type() == dmrs_type::TYPE2) ||
      (config.get_nof_tx_layers() > get_pusch_processor_phy_capabilities().max_nof_layers)) {
    GTEST_SKIP() << "Configuration not supported yet, skipping.";
  }

  ASSERT_EQ(config.rx_ports.size(), config.get_nof_tx_layers())
      << "This simulation assumes an equal number of Rx ports and Tx layers.";

  // Prepare channel estimate (just to be sure, reset all entries).
  channel_estimate::channel_estimate_dimensions ch_estimate_dims;
  ch_estimate_dims.nof_prb       = config.rb_mask.size();
  ch_estimate_dims.nof_symbols   = config.nof_symbols + config.first_symbol;
  ch_estimate_dims.nof_rx_ports  = config.rx_ports.size();
  ch_estimate_dims.nof_tx_layers = config.get_nof_tx_layers();

  channel_estimate ch_est(ch_estimate_dims);

  for (unsigned i_port = 0; i_port != ch_estimate_dims.nof_rx_ports; ++i_port) {
    for (unsigned i_layer = 0; i_layer != ch_estimate_dims.nof_tx_layers; ++i_layer) {
      span<cbf16_t> path = ch_est.get_path_ch_estimate(i_port, i_layer);
      srsvec::zero(path);
    }
  }

  // Create a spy notifier.
  dmrs_pusch_estimator_notifier_spy notifier;

  // Estimate.
  estimator->estimate(ch_est, notifier, grid, config);

  // First, assert the channel estimate dimensions haven't changed.
  ch_estimate_dims = ch_est.size();
  ASSERT_EQ(ch_estimate_dims.nof_prb, config.rb_mask.size()) << "Wrong number of PRBs.";
  ASSERT_EQ(ch_estimate_dims.nof_symbols, config.nof_symbols + config.first_symbol) << "Wrong number of symbols.";
  ASSERT_EQ(ch_estimate_dims.nof_rx_ports, config.rx_ports.size()) << "Wrong number of Rx ports.";
  ASSERT_EQ(ch_estimate_dims.nof_tx_layers, config.get_nof_tx_layers()) << "Wrong number of Tx layers.";

  // Next, assert the notifier has been called.
  ASSERT_TRUE(notifier.has_notified()) << "The estimator notifier was not called.";

  for (unsigned i_port = 0; i_port != ch_estimate_dims.nof_rx_ports; ++i_port) {
    for (unsigned i_layer = 0; i_layer != ch_estimate_dims.nof_tx_layers; ++i_layer) {
      for (unsigned i_symbol = 0; i_symbol != ch_estimate_dims.nof_symbols; ++i_symbol) {
        span<const cbf16_t> current_symbol = ch_est.get_symbol_ch_estimate(i_symbol, i_port, i_layer);
        if ((i_port != i_layer) || (i_symbol < config.first_symbol)) {
          ASSERT_TRUE(std::all_of(current_symbol.begin(), current_symbol.end(), [](cbf16_t a) {
            return (a.real.value() == 0) && (a.imag.value() == 0);
          })) << "REs should be zero on cross paths and on not allocated symbols.";
          continue;
        }
        cf_t value        = cf_t(1, 0);
        bool is_ok        = true;
        auto check_symbol = [current_symbol, &value, &is_ok](unsigned i_prb) {
          unsigned            i_re        = i_prb * NRE;
          span<const cbf16_t> current_prb = current_symbol.subspan(i_re, NRE);
          is_ok = is_ok && std::all_of(current_prb.begin(), current_prb.end(), [value](cbf16_t a) {
                    return (std::abs(to_cf(a) - value) < tolerance);
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

// Creates test suite with all the test cases.
INSTANTIATE_TEST_SUITE_P(DmrsPuschEstimatorTest,
                         DmrsPuschEstimatorFixture,
                         ::testing::ValuesIn(dmrs_pusch_estimator_test_data));
