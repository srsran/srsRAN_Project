/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../../lib/phy/upper/signal_processors/dmrs_pusch_estimator_impl.h"
#include "../../support/resource_grid_test_doubles.h"
#include "dmrs_pusch_estimator_test_data.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"
#include "gtest/gtest.h"

using namespace srsgnb;

class port_channel_estimator_dummy : public port_channel_estimator
{
public:
  void compute(channel_estimate& estimate,
               const resource_grid_reader& /**/,
               unsigned port,
               span<const dmrs_symbol_list> /**/,
               span<const dmrs_mask> /**/,
               const configuration& cfg) override
  {
    for (unsigned i_layer = 0; i_layer != cfg.nof_tx_layers; ++i_layer) {
      float marker = static_cast<float>(10 * port + i_layer);
      // Get a view to the RE channel estimate corresponding to the port-i_layer path.
      span<cf_t> ch_est = estimate.get_path_ch_estimate(port, i_layer);
      srsgnb_assert(ch_est.size() == cfg.nof_symbols * cfg.rb_mask.count() * NRE,
                    "The size of the channel estimate view does not match with the configuration parameters.");
      std::fill(ch_est.begin(), ch_est.end(), marker);
    }
  };
};

std::unique_ptr<resource_grid_reader> create_rg_reader()
{
  return std::make_unique<resource_grid_reader_spy>();
}

std::unique_ptr<dmrs_pusch_estimator> create_dmrs_pusch_estimator()
{
  std::unique_ptr<port_channel_estimator> ch_est = std::make_unique<port_channel_estimator_dummy>();
  return std::make_unique<dmrs_pusch_estimator_impl>(std::move(ch_est));
}

class DmrsPuschEstimatorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<dmrs_pusch_estimator> estimator;
  resource_grid_reader_spy              rg_spy;

  void SetUp() override
  {
    test_case_t test_case = GetParam();

    // Create port estimator.
    std::shared_ptr<port_channel_estimator_factory> port_estimator_factory = create_port_channel_estimator_factory_sw();
    ASSERT_TRUE(port_estimator_factory);

    // Create estimator factory.
    std::shared_ptr<dmrs_pusch_estimator_factory> estimator_factory =
        create_dmrs_pusch_estimator_factory_sw(port_estimator_factory);
    ASSERT_TRUE(estimator_factory);

    // Create actual channel estimator.
    estimator = estimator_factory->create();

    // Setup resource grid symbols.
    std::vector<resource_grid_reader_spy::expected_entry_t> rg_entries = test_case.symbols.read();
    rg_spy.write(rg_entries);
  }
};

TEST_P(DmrsPuschEstimatorFixture, Average)
{
  dmrs_pusch_estimator::configuration config = GetParam().config;

  // Prepare channel estimate.
  channel_estimate::channel_estimate_dimensions ch_estimate_dims;
  ch_estimate_dims.nof_prb       = config.rb_mask.count();
  ch_estimate_dims.nof_symbols   = config.nof_symbols;
  ch_estimate_dims.nof_rx_ports  = config.rx_ports.size();
  ch_estimate_dims.nof_tx_layers = config.nof_tx_layers;
  channel_estimate ch_est(ch_estimate_dims);

  // Estimate.
  estimator->estimate(ch_est, rg_spy, config);

  // Check the results. The dummy channel estimator sets all RE elements corresponding to one TX-RX path to a number
  // equal to 10 * Rx port index + layer index.
  unsigned nof_rx_ports = config.rx_ports.size();
  for (unsigned i_layer = 0; i_layer != config.nof_tx_layers; ++i_layer) {
    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      span<const cf_t> path   = ch_est.get_path_ch_estimate(i_port, i_layer);
      float            marker = static_cast<float>(10 * i_port + i_layer);
      TESTASSERT(
          std::all_of(path.begin(), path.end(), [marker](cf_t a) { return ((a.real() == marker) && (a.imag() == 0)); }),
          "Something went wrong with the channel estimation.");
    }
  }
}

// Creates test suite with al the test cases.
INSTANTIATE_TEST_SUITE_P(DmrsPuschEstimatorTest,
                         DmrsPuschEstimatorFixture,
                         ::testing::ValuesIn(dmrs_pusch_estimator_test_data));
