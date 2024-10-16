/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief  Channel estimator unit test.
///
/// The test executes a number of channel estimations. The input and the expected estimates are provided by test
/// vectors.

#include "port_channel_estimator_test_data.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_phy_capabilities.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator_parameters.h"
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
  return os << fmt::format("Symbol allocation [{}, {}], {} layers, {} allocated PRBs, RE pattern 0x{:x}, {}, "
                           "DM-RS-to-data scaling {}",
                           tc.cfg.first_symbol,
                           tc.cfg.nof_symbols,
                           tc.cfg.dmrs_pattern.size(),
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

      std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory =
          create_time_alignment_estimator_dft_factory(dft_factory);
      ASSERT_NE(ta_estimator_factory, nullptr) << "Cannot create TA estimator factory.";

      ch_est_factory = create_port_channel_estimator_factory_sw(std::move(ta_estimator_factory));
      ASSERT_NE(ch_est_factory, nullptr);
    }
  }

  void SetUp() override
  {
    // Assert factories again for compatibility with GTest < 1.11.
    ASSERT_NE(ch_est_factory, nullptr);

    port_channel_estimator_fd_smoothing_strategy smoothing      = GetParam().smoothing;
    bool                                         compensate_cfo = GetParam().compensate_cfo;
    ch_estimator                                                = ch_est_factory->create(smoothing, compensate_cfo);
    ASSERT_NE(ch_estimator, nullptr);
  }

  static std::shared_ptr<port_channel_estimator_factory> ch_est_factory;
  std::unique_ptr<port_channel_estimator>                ch_estimator;
};

std::shared_ptr<port_channel_estimator_factory> ChannelEstFixture::ch_est_factory = nullptr;

bool are_estimates_ok(span<const resource_grid_reader_spy::expected_entry_t> expected, const channel_estimate& computed)
{
  constexpr float     tolerance  = 0.008;
  unsigned            old_symbol = 15;
  span<const cbf16_t> computed_symbol;

  for (const auto& this_expected : expected) {
    unsigned i_symbol = this_expected.symbol;
    unsigned i_sc     = this_expected.subcarrier;
    unsigned i_layer  = this_expected.port;
    cf_t     value    = this_expected.value;

    if (i_symbol != old_symbol) {
      old_symbol      = i_symbol;
      computed_symbol = computed.get_symbol_ch_estimate(i_symbol, 0, i_layer);
    }

    cf_t ref = to_cf(computed_symbol[i_sc]) - value;
    if ((std::abs(std::real(ref)) > tolerance) || (std::abs(std::imag(ref)) > tolerance)) {
      return false;
    }
  }
  return true;
}

TEST_P(ChannelEstFixture, test)
{
  const test_case_t& test_params = GetParam();

  const port_channel_estimator::configuration& cfg = test_params.cfg;

  // For now, we consider at most 2 layers.
  unsigned nof_layers = cfg.dmrs_pattern.size();
  ASSERT_LE(nof_layers, 2) << "For now, at most two transmission layers are supported.";

  if ((nof_layers == 2) && (get_pusch_processor_phy_capabilities().max_nof_layers < 2)) {
    GTEST_SKIP() << "The 2-layer channel estimator is not supported in this version - skipping the test.";
  }

  // The test only considers a single port.
  ASSERT_EQ(cfg.rx_ports.size(), 1) << "The channel estimator test only considers a single Rx port.";

  // The second layer, if present, has the same pattern of the first one.
  const port_channel_estimator::layer_dmrs_pattern& dmrs_pattern = cfg.dmrs_pattern[0];

  unsigned nof_dmrs_symbols     = dmrs_pattern.symbols.count();
  unsigned nof_allocatd_rblocks = dmrs_pattern.rb_mask.count();
  unsigned nof_dmrs_scs         = nof_allocatd_rblocks * dmrs_pattern.re_pattern.count();
  unsigned nof_dmrs_pilots      = nof_dmrs_scs * nof_dmrs_symbols;

  std::vector<cf_t> pilots = test_params.pilots.read();
  ASSERT_EQ(pilots.size(), nof_dmrs_pilots * nof_layers) << fmt::format(
      "Number of DM-RS pilots mismatch: configured {}, provided {}.", nof_dmrs_pilots * nof_layers, pilots.size());

  std::vector<resource_grid_reader_spy::expected_entry_t> grid_entries = test_params.grid.read();
  ASSERT_EQ(grid_entries.size(), nof_dmrs_pilots) << fmt::format(
      "Number of received pilots mismatch: configured {}, read {}.", nof_dmrs_pilots, grid_entries.size());

  unsigned nof_allocated_res = nof_allocatd_rblocks * NRE * cfg.nof_symbols;

  std::vector<resource_grid_reader_spy::expected_entry_t> expected_estimates = test_params.estimates.read();
  ASSERT_EQ(expected_estimates.size(), nof_allocated_res * nof_layers)
      << fmt::format("Number of channel estimates mismatch: configured {}, provided {}",
                     nof_allocated_res * nof_layers,
                     expected_estimates.size());

  channel_estimate::channel_estimate_dimensions dims;
  dims.nof_prb       = test_params.grid_size_prbs;
  dims.nof_symbols   = MAX_NSYMB_PER_SLOT;
  dims.nof_rx_ports  = 1;
  dims.nof_tx_layers = nof_layers;
  channel_estimate estimates(dims);

  re_measurement_dimensions pilot_dims;
  pilot_dims.nof_subc    = nof_dmrs_scs;
  pilot_dims.nof_symbols = nof_dmrs_symbols;
  pilot_dims.nof_slices  = nof_layers;
  dmrs_symbol_list pilots_arranged(pilot_dims);
  span<cf_t>       pilot_read = pilots;
  for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
    pilots_arranged.set_slice(pilot_read.first(nof_dmrs_pilots), i_layer);
    pilot_read = pilot_read.last(pilot_read.size() - nof_dmrs_pilots);
  }

  resource_grid_reader_spy grid;
  grid.write(grid_entries);
  ch_estimator->compute(estimates, grid, 0, pilots_arranged, cfg);

  // Calculate the tolerance for the measured TA. It assumes a DFT size of 4096 and a maximum error of ±1 sample.
  double tolerance_ta_us = 1e3 / (4096 * scs_to_khz(test_params.cfg.scs));

  ASSERT_TRUE(are_estimates_ok(expected_estimates, estimates));
  ASSERT_NEAR(estimates.get_rsrp(0, 0), test_params.rsrp, 5e-4);
  ASSERT_NEAR(estimates.get_epre(0, 0), test_params.epre, 5e-4);
  ASSERT_NEAR(estimates.get_noise_variance(0, 0), test_params.noise_var_est, 5e-4);
  ASSERT_NEAR(estimates.get_snr_dB(0, 0), test_params.snr_est, 0.002 * std::abs(test_params.snr_est));
  ASSERT_NEAR(estimates.get_time_alignment(0, 0).to_seconds() * 1e6, test_params.ta_us, tolerance_ta_us);
  if (test_params.cfo_est_Hz.has_value()) {
    ASSERT_TRUE(estimates.get_cfo_Hz(0, 0).has_value()) << "CFO estimation was expected, none obtained.";
    ASSERT_NEAR(estimates.get_cfo_Hz(0, 0).value(),
                test_params.cfo_est_Hz.value(),
                0.001 * std::abs(test_params.cfo_est_Hz.value()));
  } else {
    ASSERT_FALSE(estimates.get_cfo_Hz(0, 0).has_value()) << "No CFO estimation was expected.";
  }
}

INSTANTIATE_TEST_SUITE_P(ChannelEstSuite, ChannelEstFixture, ::testing::ValuesIn(port_channel_estimator_test_data));

} // namespace

/// \endcond
