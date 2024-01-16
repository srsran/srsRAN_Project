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

#include "pusch_codeword_buffer_test_doubles.h"
#include "pusch_demodulator_notifier_test_doubles.h"
#include "pusch_demodulator_test_data.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/srsvec/compare.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsran {

// Maximum allowed error.
constexpr log_likelihood_ratio::value_type LLR_MAX_ERROR = 1;

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "rnti={} rb_mask=[{}] modulation={} t_alloc={}:{} dmrs_pos=[{}] dmrs_type={} nof_cdm...data={} n_id={} "
             "nof_tx_layers={} rx_ports={}",
             test_case.context.config.rnti,
             test_case.context.config.rb_mask,
             test_case.context.config.modulation,
             test_case.context.config.start_symbol_index,
             test_case.context.config.nof_symbols,
             test_case.context.config.dmrs_symb_pos,
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

std::ostream& operator<<(std::ostream& os, const bit_buffer& data)
{
  fmt::print(os, "{}", data);
  return os;
}

bool operator==(span<const log_likelihood_ratio> lhs, span<const log_likelihood_ratio> rhs)
{
  return std::equal(
      lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](log_likelihood_ratio lhs_, log_likelihood_ratio rhs_) {
        return log_likelihood_ratio::abs(lhs_ - rhs_) <= LLR_MAX_ERROR;
      });
}

} // namespace srsran

using namespace srsran;

namespace {

class PuschDemodulatorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<pusch_demodulator> demodulator;
  pusch_demodulator::configuration   config;
  std::vector<log_likelihood_ratio>  codeword;
  dynamic_bit_buffer                 scrambling_seq;

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
        create_pusch_demodulator_factory_sw(equalizer_factory, demod_factory, prg_factory, true, true);
    ASSERT_TRUE(pusch_demod_factory);

    // Create actual PUSCH demodulator.
    demodulator = pusch_demod_factory->create();

    // Prepare PUSCH demodulator configuration.
    config = test_case.context.config;

    // Load unpacked scrambling sequence.
    std::vector<uint8_t> unpacked_scrambling_seq = test_case.scrambling_seq.read();

    // Pack scrambling sequence.
    scrambling_seq.resize(unpacked_scrambling_seq.size());
    srsvec::bit_pack(scrambling_seq, unpacked_scrambling_seq);

    // Prepare descrambled codeword data.
    codeword = test_case.codeword.read();
  }

  ~PuschDemodulatorFixture() = default;
};

TEST_P(PuschDemodulatorFixture, PuschDemodulatorUnittest)
{
  const test_case_t& test_case = GetParam();

  // Prepare resource grid.
  resource_grid_reader_spy grid;
  grid.write(test_case.symbols.read());

  // Read estimated channel from the test case.
  dynamic_tensor<static_cast<unsigned>(ch_dims::nof_dims), cf_t, ch_dims> estimates = test_case.estimates.read();

  // Prepare channel estimates.
  channel_estimate::channel_estimate_dimensions ce_dims;
  ce_dims.nof_prb       = estimates.get_dimension_size(ch_dims::subcarrier) / NRE;
  ce_dims.nof_symbols   = estimates.get_dimension_size(ch_dims::symbol);
  ce_dims.nof_rx_ports  = estimates.get_dimension_size(ch_dims::rx_port);
  ce_dims.nof_tx_layers = estimates.get_dimension_size(ch_dims::tx_layer);
  channel_estimate chan_estimates(ce_dims);

  // Populate channel estimate.
  for (unsigned i_rx_port = 0; i_rx_port != ce_dims.nof_rx_ports; ++i_rx_port) {
    // Set noise variance.
    chan_estimates.set_noise_variance(test_case.context.noise_var, config.rx_ports[i_rx_port], 0);

    // Copy port channel estimates.
    srsvec::copy(chan_estimates.get_path_ch_estimate(config.rx_ports[i_rx_port], 0),
                 estimates.get_view<static_cast<unsigned>(ch_dims::rx_port)>({i_rx_port, 0}));
  }

  // Create a codeword buffer temporally. This will become a spy.
  pusch_codeword_buffer_spy codeword_buffer(codeword.size());

  // Demodulate.
  pusch_demodulator_notifier_spy notifier;
  demodulator->demodulate(codeword_buffer, notifier, grid, chan_estimates, config);

  // Check there are provisional stats.
  ASSERT_FALSE(notifier.get_provisional_stats_entries().empty());

  // Check and extract end stats.
  ASSERT_EQ(notifier.get_end_stats_entries().size(), 1);
  const pusch_demodulator_notifier::demodulation_stats& status = notifier.get_end_stats_entries().front();

  // Assert that EVM is present.
  ASSERT_TRUE(status.evm.has_value());

  // Assert SINR estimation is present and with the desired value.
  ASSERT_TRUE(status.sinr_dB.has_value());
  ASSERT_NEAR(status.sinr_dB.value(), test_case.context.sinr_dB, 0.5);

  // Assert demodulated soft bits matches.
  ASSERT_EQ(span<const log_likelihood_ratio>(codeword), codeword_buffer.get_data());

  // Assert descrambled soft bits matches.
  ASSERT_EQ(bit_buffer(scrambling_seq), codeword_buffer.get_scrambling_seq());
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PuschDemodulatorVectorTest,
                         PuschDemodulatorFixture,
                         ::testing::ValuesIn(pusch_demodulator_test_data));

} // namespace