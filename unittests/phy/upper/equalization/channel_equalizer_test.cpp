/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "channel_equalizer_test_data.h"
#include "srsgnb/phy/upper/equalization/equalization_factories.h"
#include "srsgnb/srslog/bundled/fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsgnb;

namespace srsgnb {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "{}_{}Tx_{}Rx",
             test_case.equalizer_type,
             test_case.ch_estimates.nof_tx_layers,
             test_case.ch_estimates.nof_rx_ports);
  return os;
}

} // namespace srsgnb

namespace {

using ChannelEqualizerParams = test_case_t;

class ChannelEqualizerFixture : public ::testing::TestWithParam<ChannelEqualizerParams>
{
protected:
  static constexpr float MAX_ERROR_EQ = 1e-5;

  static_re_measurement<cf_t, MAX_RB * NRE, MAX_NSYMB_PER_SLOT, MAX_PORTS>                       rx_symbols;
  static_re_measurement<cf_t, MAX_RB * NRE, MAX_NSYMB_PER_SLOT, pusch_constants::MAX_NOF_LAYERS> eq_symbols_expected;
  static_re_measurement<cf_t, MAX_RB * NRE, MAX_NSYMB_PER_SLOT, pusch_constants::MAX_NOF_LAYERS> eq_symbols_actual;
  static_re_measurement<float, MAX_RB * NRE, MAX_NSYMB_PER_SLOT, pusch_constants::MAX_NOF_LAYERS>
      eq_noise_vars_expected;
  static_re_measurement<float, MAX_RB * NRE, MAX_NSYMB_PER_SLOT, pusch_constants::MAX_NOF_LAYERS> eq_noise_vars_actual;
  channel_estimate                                                                                test_ch_estimate;

  static std::shared_ptr<channel_equalizer_factory> equalizer_factory;
  static std::unique_ptr<channel_equalizer>         test_equalizer;

  static void SetUpTestSuite()
  {
    // Create channel equalizer factory.
    if (!equalizer_factory) {
      equalizer_factory = create_channel_equalizer_factory_zf();
      ASSERT_NE(equalizer_factory, nullptr) << "Cannot create equalizer factory";
    }

    // Create channel equalizer.
    if (!test_equalizer) {
      test_equalizer = equalizer_factory->create();
      ASSERT_NE(test_equalizer, nullptr) << "Cannot create channel equalizer";
    }
  }

  void ReadData(const ChannelEqualizerParams& t_case)
  {
    // Read the test case symbols and estimates.
    ASSERT_NO_FATAL_FAILURE(ReadReMeasurement(rx_symbols, t_case.received_symbols));
    ASSERT_NO_FATAL_FAILURE(ReadReMeasurement(eq_symbols_expected, t_case.equalized_symbols));
    ASSERT_NO_FATAL_FAILURE(ReadReMeasurement(eq_noise_vars_expected, t_case.equalized_noise_vars));
    ASSERT_NO_FATAL_FAILURE(ReadChannelEstimates(test_ch_estimate, t_case.ch_estimates));

    // Resize the equalizer output data structures.
    eq_noise_vars_actual.resize(eq_symbols_expected.size());
    eq_symbols_actual.resize(eq_symbols_expected.size());
  }

private:
  template <typename T>
  static void ReadReMeasurement(re_measurement<T>& data, const re_measurement_exploded<T>& data_exploded)
  {
    re_measurement_dimensions re_dims;
    re_dims.nof_subc    = data_exploded.nof_prb * NRE;
    re_dims.nof_symbols = data_exploded.nof_symbols;
    re_dims.nof_slices  = data_exploded.nof_slices;

    data.resize(re_dims);
    const std::vector<T> all_syms     = data_exploded.measurements.read();
    unsigned             slice_length = re_dims.nof_symbols * re_dims.nof_subc;
    ASSERT_EQ(slice_length * re_dims.nof_slices, all_syms.size()) << "Wrong number of symbols.";

    span<const T> all_syms_span(all_syms);
    for (unsigned i_slice = 0, skip = 0; i_slice != re_dims.nof_slices; ++i_slice) {
      data.set_slice(all_syms_span.subspan(skip, slice_length), i_slice);
      skip += slice_length;
    }
  }

  static void ReadChannelEstimates(channel_estimate& ch_est, const ch_estimates_exploded& ch_est_expl)
  {
    channel_estimate::channel_estimate_dimensions ch_dims;
    ch_dims.nof_prb       = ch_est_expl.nof_prb;
    ch_dims.nof_symbols   = ch_est_expl.nof_symbols;
    ch_dims.nof_rx_ports  = ch_est_expl.nof_rx_ports;
    ch_dims.nof_tx_layers = ch_est_expl.nof_tx_layers;

    ch_est.resize(ch_dims);
    std::vector<cf_t> all_ests     = ch_est_expl.estimates.read();
    unsigned          slice_length = ch_dims.nof_symbols * ch_dims.nof_prb * NRE;
    unsigned          nof_paths    = ch_dims.nof_tx_layers * ch_dims.nof_rx_ports;
    ASSERT_EQ(slice_length * nof_paths, all_ests.size()) << "Wrong number of channel estimates.";

    span<const cf_t> all_ests_span(all_ests);
    unsigned         skip = 0;
    for (unsigned i_tx = 0; i_tx != ch_dims.nof_tx_layers; ++i_tx) {
      for (unsigned i_rx = 0; i_rx != ch_dims.nof_rx_ports; ++i_rx) {
        span<cf_t> path = ch_est.get_path_ch_estimate(i_rx, i_tx);
        srsvec::copy(path, all_ests_span.subspan(skip, slice_length));
        skip += slice_length;

        ch_est.set_noise_variance(ch_est_expl.noise_var, i_rx, i_tx);
      }
    }
  }
};

std::shared_ptr<channel_equalizer_factory> ChannelEqualizerFixture::equalizer_factory = nullptr;
std::unique_ptr<channel_equalizer>         ChannelEqualizerFixture::test_equalizer    = nullptr;

TEST_P(ChannelEqualizerFixture, ChannelEqualizerTest)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();
  ReadData(t_case);

  // For now, check only Zero Forcing equalizer, since MMSE equalizer is not implemented yet.
  if (t_case.equalizer_type != "ZF") {
    GTEST_SKIP();
  }

  // Equalize the symbols coming from the Rx ports.
  test_equalizer->equalize(eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimate, t_case.scaling);

  // Assert results.
  for (unsigned i_layer = 0; i_layer != t_case.ch_estimates.nof_tx_layers; ++i_layer) {
    for (unsigned i_symbol = 0; i_symbol != t_case.ch_estimates.nof_symbols; ++i_symbol) {
      span<const cf_t>  eq_subcs_actual         = eq_symbols_actual.get_symbol(i_symbol, i_layer);
      span<const cf_t>  eq_subcs_expected       = eq_symbols_expected.get_symbol(i_symbol, i_layer);
      span<const float> eq_nvars_subcs_expected = eq_noise_vars_expected.get_symbol(i_symbol, i_layer);
      span<const float> eq_nvars_subcs_actual   = eq_noise_vars_actual.get_symbol(i_symbol, i_layer);

      for (unsigned i_subc = 0; i_subc != t_case.ch_estimates.nof_prb * NRE; ++i_subc) {
        // Assert error between the expected and the actual symbols after equalization.
        ASSERT_NEAR(std::real(eq_subcs_expected[i_subc]), std::real(eq_subcs_actual[i_subc]), MAX_ERROR_EQ)
            << "Excessive equalized symbols error.";
        ASSERT_NEAR(std::imag(eq_subcs_expected[i_subc]), std::imag(eq_subcs_actual[i_subc]), MAX_ERROR_EQ)
            << "Excessive equalized symbols error.";

        // Assert error between the expected and the actual post equalization noise variances.
        ASSERT_NEAR(eq_nvars_subcs_expected[i_subc], eq_nvars_subcs_actual[i_subc], MAX_ERROR_EQ)
            << "Excessive post-equalization equivalent noise variance error.";
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(ChannelEqualizerTest,
                         ChannelEqualizerFixture,
                         ::testing::ValuesIn(channel_equalizer_test_data),
                         ::testing::PrintToStringParamName());

} // namespace