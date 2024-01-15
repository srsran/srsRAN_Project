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

/// \file
/// \brief  Channel estimator unit test.
///
/// The test executes a number of channel estimations. The input and the expected estimates are provided by test
/// vectors.

#include "port_channel_estimator_test_data.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"

#include <gtest/gtest.h>

using namespace srsran;

/// \cond

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& tc)
{
  const port_channel_estimator::layer_dmrs_pattern& dmrs_pattern = tc.cfg.dmrs_pattern[0];
  std::string                                       hops =
      (dmrs_pattern.hopping_symbol_index.has_value() ? "intraslot frequency hopping" : "no frequency hopping");
  return os << fmt::format(
             "Symbol allocation [{}, {}], {} allocated PRBs, RE pattern 0x{:x}, {}, DM-RS-to-data scaling {}",
             tc.cfg.first_symbol,
             tc.cfg.nof_symbols,
             dmrs_pattern.rb_mask.count(),
             dmrs_pattern.re_pattern,
             hops,
             tc.cfg.scaling);
}

} // namespace srsran

namespace {
class ChannelEstFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static void SetUpTestSuite()
  {
    if (!ch_est_factory) {
      std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
      if (!dft_factory) {
        dft_factory = create_dft_processor_factory_generic();
      }
      ASSERT_NE(dft_factory, nullptr) << "Cannot create DFT factory.";

      ch_est_factory = create_port_channel_estimator_factory_sw(std::move(dft_factory));
      ASSERT_NE(ch_est_factory, nullptr);
    }
  }

  void SetUp() override
  {
    // Assert factories again for compatibility with GTest < 1.11.
    ASSERT_NE(ch_est_factory, nullptr);

    ch_estimator = ch_est_factory->create(port_channel_estimator_fd_smoothing_strategy::filter);
    ASSERT_NE(ch_estimator, nullptr);
  }

  static std::shared_ptr<port_channel_estimator_factory> ch_est_factory;
  std::unique_ptr<port_channel_estimator>                ch_estimator;
};

std::shared_ptr<port_channel_estimator_factory> ChannelEstFixture::ch_est_factory = nullptr;

constexpr float tolerance = 5e-4;

bool are_estimates_ok(span<const resource_grid_reader_spy::expected_entry_t> expected, const channel_estimate& computed)
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

TEST_P(ChannelEstFixture, test)
{
  test_case_t test_params = GetParam();

  port_channel_estimator::configuration cfg = test_params.cfg;

  // For now, single-layer/single-port transmissions only.
  ASSERT_EQ(cfg.dmrs_pattern.size(), 1) << "For now, only single-layer transmissions.";
  ASSERT_EQ(cfg.rx_ports.size(), 1) << "For now, only single-port reception is implemented.";

  port_channel_estimator::layer_dmrs_pattern& dmrs_pattern = cfg.dmrs_pattern[0];

  unsigned nof_dmrs_symbols     = dmrs_pattern.symbols.count();
  unsigned nof_allocatd_rblocks = dmrs_pattern.rb_mask.count();
  unsigned nof_dmrs_scs         = nof_allocatd_rblocks * dmrs_pattern.re_pattern.count();
  unsigned nof_dmrs_pilots      = nof_dmrs_scs * nof_dmrs_symbols;

  std::vector<cf_t> pilots = test_params.pilots.read();
  ASSERT_EQ(pilots.size(), nof_dmrs_pilots)
      << fmt::format("Number of DM-RS pilots mismatch: configured {}, provided {}.", nof_dmrs_symbols, pilots.size());

  std::vector<resource_grid_reader_spy::expected_entry_t> grid_entries = test_params.grid.read();
  ASSERT_EQ(grid_entries.size(), nof_dmrs_pilots) << fmt::format(
      "Number of received pilots mismatch: configured {}, read {}.", nof_dmrs_pilots, grid_entries.size());

  unsigned nof_allocated_res = nof_allocatd_rblocks * NRE * cfg.nof_symbols;

  std::vector<resource_grid_reader_spy::expected_entry_t> expected_estimates = test_params.estimates.read();
  ASSERT_EQ(expected_estimates.size(), nof_allocated_res) << fmt::format(
      "Number of channel estimates mismatch: configured {}, provided {}", nof_allocated_res, expected_estimates.size());

  channel_estimate::channel_estimate_dimensions dims;
  dims.nof_prb       = test_params.grid_size_prbs;
  dims.nof_symbols   = MAX_NSYMB_PER_SLOT;
  dims.nof_rx_ports  = 1;
  dims.nof_tx_layers = 1;
  channel_estimate estimates(dims);

  re_measurement_dimensions pilot_dims;
  pilot_dims.nof_subc    = nof_dmrs_scs;
  pilot_dims.nof_symbols = nof_dmrs_symbols;
  pilot_dims.nof_slices  = 1;
  dmrs_symbol_list pilots_arranged(pilot_dims);
  pilots_arranged.set_slice(pilots, 0);

  resource_grid_reader_spy grid;
  grid.write(grid_entries);
  ch_estimator->compute(estimates, grid, 0, pilots_arranged, cfg);

  ASSERT_TRUE(are_estimates_ok(expected_estimates, estimates));
  ASSERT_NEAR(estimates.get_rsrp(0, 0), test_params.rsrp, tolerance);
  ASSERT_NEAR(estimates.get_epre(0, 0), test_params.epre, tolerance);
  ASSERT_NEAR(estimates.get_noise_variance(0, 0), test_params.noise_var_est, tolerance);
  ASSERT_NEAR(estimates.get_snr_dB(0, 0), test_params.snr_est, tolerance);
  ASSERT_NEAR(estimates.get_time_alignment(0, 0).to_seconds() * 1e6, test_params.ta_us, tolerance);
}

INSTANTIATE_TEST_SUITE_P(ChannelEstSuite, ChannelEstFixture, ::testing::ValuesIn(port_channel_estimator_test_data));

} // namespace

/// \endcond
