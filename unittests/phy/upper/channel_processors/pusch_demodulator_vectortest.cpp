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
#include "srsgnb/phy/upper/equalization/equalization_factories.h"
#include "srsgnb/srsvec/compare.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsgnb;

using PuschDemodulatorParams = test_case_t;

namespace srsgnb {

// Maximum allowed error.
constexpr log_likelihood_ratio::value_type LLR_MAX_ERROR = 1;

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

std::ostream& operator<<(std::ostream& os, span<const log_likelihood_ratio> llr)
{
  fmt::print(os, "{}", llr);
  return os;
}

bool operator==(span<const log_likelihood_ratio> lhs, span<const log_likelihood_ratio> rhs)
{
  return std::equal(
      lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](log_likelihood_ratio lhs_, log_likelihood_ratio rhs_) {
        return log_likelihood_ratio::abs(lhs_ - rhs_) <= LLR_MAX_ERROR;
      });
}

} // namespace srsgnb

class PuschDemodulatorFixture : public ::testing::TestWithParam<PuschDemodulatorParams>
{
protected:
  std::unique_ptr<pusch_demodulator> demodulator;
  std::unique_ptr<resource_grid>     grid;
  pusch_demodulator::configuration   config;
  std::vector<log_likelihood_ratio>  sch_expected;

  void SetUp() override
  {
    const test_case_t& test_case = GetParam();

    std::shared_ptr<channel_equalizer_factory> equalizer_factory = create_channel_equalizer_factory_zf();
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
  }

  ~PuschDemodulatorFixture() = default;
};

TEST_P(PuschDemodulatorFixture, PuschProcessorUnittest)
{
  const test_case_t& test_case = GetParam();

  if ((config.dmrs_config_type != dmrs_type::TYPE1) || (config.nof_cdm_groups_without_data != 2)) {
    GTEST_SKIP();
  }

  // Prepare channel estimates.
  channel_estimate::channel_estimate_dimensions ce_dims;
  ce_dims.nof_prb       = config.rb_mask.size();
  ce_dims.nof_symbols   = MAX_NSYMB_PER_SLOT;
  ce_dims.nof_rx_ports  = config.rx_ports.size();
  ce_dims.nof_tx_layers = config.nof_tx_layers;
  channel_estimate chan_estimates(ce_dims);

  // Set estimated channel.
  std::vector<cf_t> estimates = test_case.estimates.read();
  srsvec::copy(chan_estimates.get_path_ch_estimate(0, 0), estimates);

  // Set noise variance.
  chan_estimates.set_noise_variance(test_case.context.noise_var, 0, 0);

  std::vector<log_likelihood_ratio> sch_data(sch_expected.size());

  demodulator->demodulate(sch_data, *grid, chan_estimates, config);

  // Assert shared channel data matches.
  ASSERT_EQ(span<const log_likelihood_ratio>(sch_expected), span<const log_likelihood_ratio>(sch_data));
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PuschDemodulatorVectorTest,
                         PuschDemodulatorFixture,
                         ::testing::ValuesIn(pusch_demodulator_test_data));
