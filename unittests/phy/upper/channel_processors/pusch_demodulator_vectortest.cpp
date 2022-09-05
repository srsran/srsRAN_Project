/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../phy_test_utils.h"
#include "pusch_demodulator_test_data.h"
#include "srsgnb/phy/support/support_factories.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/srslog/bundled/fmt/ostream.h"
#include "srsgnb/srsvec/compare.h"
#include <gtest/gtest.h>

using namespace srsgnb;

using PuschDemodulatorParams = test_case_t;

namespace srsgnb {

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os,
             "rnti={} rb_mask=[{}] modulation={} t_alloc={}:{} dmrs_pos=[{}] dmrs_type={} nof_cdm...data={} n_id={} "
             "nof_tx_layers={} rx_ports={}",
             test_case.context.config.rnti,
             test_case.context.config.rb_mask,
             test_case.context.config.modulation,
             test_case.context.config.start_symbol_index,
             test_case.context.config.nof_symbols,
             span<const bool>(test_case.context.config.dmrs_symb_pos),
             test_case.context.config.dmrs_config_type == dmrs_type::TYPE1 ? 1 : 2,
             test_case.context.config.nof_cdm_groups_without_data,
             test_case.context.config.n_id,
             test_case.context.config.nof_tx_layers,
             span<const uint8_t>(test_case.context.config.rx_ports));
  return os;
}

} // namespace srsgnb

namespace {
class channel_equalizer_spy : public channel_equalizer
{
public:
  void equalize(equalizer_symbol_list&          mod_symbols,
                equalizer_noise_var_list&       noise_vars,
                const equalizer_ch_symbol_list& ch_symbols,
                const channel_estimate&         ch_estimates,
                float /**/) override
  {
    for (unsigned i_layer = 0, i_layer_end = noise_vars.size().nof_slices; i_layer != i_layer_end; ++i_layer) {
      float       noise_var  = ch_estimates.get_noise_variance(0, i_layer);
      span<float> layer_data = noise_vars.get_slice(i_layer);
      std::fill(layer_data.begin(), layer_data.end(), noise_var);

      srsvec::copy(mod_symbols.get_slice(i_layer), ch_symbols.get_slice(i_layer));
    }
  }
};

PHY_SPY_FACTORY_TEMPLATE(channel_equalizer);

} // namespace

class PuschDemodulatorFixture : public ::testing::TestWithParam<PuschDemodulatorParams>
{
protected:
  std::unique_ptr<pusch_demodulator> demodulator;
  std::unique_ptr<resource_grid>     grid;
  pusch_demodulator::configuration   config;
  std::vector<log_likelihood_ratio>  sch_expected;
  std::vector<log_likelihood_ratio>  harq_ack_expected;
  std::vector<log_likelihood_ratio>  csi_part1_expected;
  std::vector<log_likelihood_ratio>  csi_part2_expected;

  void SetUp() override
  {
    const test_case_t& test_case = GetParam();

    std::shared_ptr<channel_equalizer_factory> equalizer_factory = std::make_shared<channel_equalizer_factory_spy>();
    ASSERT_TRUE(equalizer_factory);

    std::shared_ptr<channel_modulation_factory> demod_factory = create_channel_modulation_sw_factory();
    ASSERT_TRUE(demod_factory);

    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    ASSERT_TRUE(prg_factory);

    std::shared_ptr<pusch_demodulator_factory> pusch_demod_factory =
        create_pusch_demodulator_factory_sw(equalizer_factory, demod_factory, prg_factory);
    ASSERT_TRUE(pusch_demod_factory);

    // Create actual PUSCH processor.
    demodulator = pusch_demod_factory->create();

    // Prepare resource grid.
    grid = create_resource_grid(test_case.context.config.rx_ports.size(),
                                test_case.context.config.start_symbol_index + test_case.context.config.nof_symbols,
                                test_case.context.config.rb_mask.size() * NRE);
    ASSERT_TRUE(grid);

    // Write zeros in grid.
    grid->set_all_zero();

    // Setup resource grid symbols.
    std::vector<resource_grid_reader_spy::expected_entry_t> rg_entries = test_case.symbols.read();
    for (unsigned i_port = 0, i_port_end = test_case.context.config.rx_ports.size(); i_port != i_port_end; ++i_port) {
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

    // Prepare PUSCH demodulator configuration.
    config = test_case.context.config;

    // Prepare SCH.
    sch_expected = test_case.sch_data.read();

    // Prepare SCH data.
    // harq_ack_expected = test_case.harq_ack.read();

    // Prepare CSI part 1 data.
    // csi_part1_expected = test_case.csi_part1.read();

    // Prepare CSI part 2 data.
    // csi_part2_expected = test_case.csi_part2.read();
  }

  ~PuschDemodulatorFixture() = default;
};

TEST_P(PuschDemodulatorFixture, PuschProcessorUnittest)
{
  const test_case_t& test_case = GetParam();

  // Prepare channel estimates.
  channel_estimate::channel_estimate_dimensions ce_dims;
  ce_dims.nof_prb       = config.rb_mask.count();
  ce_dims.nof_symbols   = config.nof_symbols;
  ce_dims.nof_rx_ports  = config.rx_ports.size();
  ce_dims.nof_tx_layers = config.nof_tx_layers;
  channel_estimate chan_estimates(ce_dims);

  // Set estimated channel.
  std::vector<cf_t> estimates = test_case.estimates.read();
  srsvec::copy(chan_estimates.get_path_ch_estimate(0, 0), estimates);

  // Set noise variance.
  chan_estimates.set_noise_variance(test_case.context.noise_var, 0, 0);

  std::vector<log_likelihood_ratio> sch_data(sch_expected.size());
  std::vector<log_likelihood_ratio> harq_ack(harq_ack_expected.size());
  std::vector<log_likelihood_ratio> csi_part1(csi_part1_expected.size());
  std::vector<log_likelihood_ratio> csi_part2(csi_part2_expected.size());

  demodulator->demodulate(sch_data, harq_ack, csi_part1, csi_part2, *grid, chan_estimates, config);

  // Assert shared channel data matches.
  for (unsigned i_sch_data = 0, i_sch_data_end = sch_expected.size(); i_sch_data != i_sch_data_end; ++i_sch_data) {
    ASSERT_EQ(sch_expected[i_sch_data], sch_data[i_sch_data]);
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PuschDemodulatorVectorTest,
                         PuschDemodulatorFixture,
                         ::testing::ValuesIn(pusch_demodulator_test_data));