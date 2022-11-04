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
#include "dmrs_pusch_estimator_test_data.h"
#include "srsgnb/phy/support/support_factories.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"
#include "srsgnb/srslog/bundled/fmt/ostream.h"
#include "gtest/gtest.h"

using namespace srsgnb;

namespace srsgnb {

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
             span<const bool>(config.symbols_mask),
             config.rb_mask,
             config.first_symbol,
             config.nof_symbols,
             config.nof_tx_layers,
             span<const uint8_t>(config.rx_ports));
  return os;
}

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os, "config={} symbols={};", test_case.config, test_case.symbols.get_file_name());
  return os;
}

} // namespace srsgnb

namespace {
const float ASSERT_MAX_ERROR = 1E-5;

class DmrsPuschEstimatorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<dmrs_pusch_estimator> estimator;
  std::unique_ptr<resource_grid>        grid;

  void SetUp() override
  {
    test_case_t test_case = GetParam();

    // Create PRG.
    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    ASSERT_TRUE(prg_factory);

    // Create port estimator.
    std::shared_ptr<port_channel_estimator_factory> port_estimator_factory = create_port_channel_estimator_factory_sw();
    ASSERT_TRUE(port_estimator_factory);

    // Create estimator factory.
    std::shared_ptr<dmrs_pusch_estimator_factory> estimator_factory =
        create_dmrs_pusch_estimator_factory_sw(prg_factory, port_estimator_factory);
    ASSERT_TRUE(estimator_factory);

    // Create actual channel estimator.
    estimator = estimator_factory->create();
    ASSERT_TRUE(estimator);

    // Create grid.
    grid = create_resource_grid(
        test_case.config.rx_ports.size(), MAX_NSYMB_PER_SLOT, NRE * test_case.config.rb_mask.size());
    ASSERT_TRUE(grid);

    // Write zeros in grid.
    grid->set_all_zero();

    // Setup resource grid symbols.
    std::vector<resource_grid_reader_spy::expected_entry_t> rg_entries = test_case.symbols.read();
    for (unsigned i_port = 0, i_port_end = test_case.config.rx_ports.size(); i_port != i_port_end; ++i_port) {
      // Create vector of coordinates and values for the port.
      std::vector<resource_grid_coordinate> coordinates(0);
      std::vector<cf_t>                     values(0);

      // Reserve to avoid continuous memory allocation.
      coordinates.reserve(rg_entries.size());
      values.reserve(rg_entries.size());

      // Select the grid entries that match the port.
      for (const resource_grid_reader_spy::expected_entry_t& rg_entry : rg_entries) {
        if (rg_entry.port == i_port) {
          resource_grid_coordinate coordinate;
          coordinate.subcarrier = rg_entry.subcarrier;
          coordinate.symbol     = rg_entry.symbol;
          coordinates.emplace_back(coordinate);
          values.emplace_back(rg_entry.value);
        }
      }

      // Put elements in the grid for the selected port.
      grid->put(i_port, coordinates, values);
    }
  }
};
} // namespace

TEST_P(DmrsPuschEstimatorFixture, Average)
{
  dmrs_pusch_estimator::configuration config = GetParam().config;

  // The current estimator does not support Type2 DM-RS.
  if (config.type == dmrs_type::TYPE2) {
    GTEST_SKIP();
  }

  // Prepare channel estimate.
  channel_estimate::channel_estimate_dimensions ch_estimate_dims;
  ch_estimate_dims.nof_prb       = config.rb_mask.count();
  ch_estimate_dims.nof_symbols   = config.nof_symbols;
  ch_estimate_dims.nof_rx_ports  = config.rx_ports.size();
  ch_estimate_dims.nof_tx_layers = config.nof_tx_layers;
  channel_estimate ch_est(ch_estimate_dims);

  // Estimate.
  estimator->estimate(ch_est, *grid, config);

  // Current estimator is provisional.
  GTEST_SKIP();

  // Check the results. The dummy channel estimator sets all RE elements corresponding to one TX-RX path to a number
  // equal to 10 * Rx port index + layer index.
  for (unsigned i_layer = 0; i_layer != config.nof_tx_layers; ++i_layer) {
    for (unsigned i_port = 0, nof_rx_ports = config.rx_ports.size(); i_port != nof_rx_ports; ++i_port) {
      // Extract the layer and port path.
      span<const cf_t> path = ch_est.get_path_ch_estimate(i_port, i_layer);

      // Expect an identity matrix.
      cf_t expected = (i_layer == i_port) ? cf_t(1) : cf_t(0);

      for (cf_t value : path) {
        ASSERT_NEAR(expected.real(), value.real(), ASSERT_MAX_ERROR);
        ASSERT_NEAR(expected.imag(), value.imag(), ASSERT_MAX_ERROR);
      }
    }
  }
}

// Creates test suite with al the test cases.
INSTANTIATE_TEST_SUITE_P(DmrsPuschEstimatorTest,
                         DmrsPuschEstimatorFixture,
                         ::testing::ValuesIn(dmrs_pusch_estimator_test_data));
