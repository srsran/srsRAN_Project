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

using re_dims = channel_equalizer::re_list::dims;
using ch_dims = channel_equalizer::ch_est_list::dims;

class ChannelEqualizerFixture : public ::testing::TestWithParam<ChannelEqualizerParams>
{
protected:
  static constexpr float MAX_ERROR_EQ = 1e-5;

  static constexpr unsigned MAX_RX_RE   = MAX_RB * NRE * MAX_NSYMB_PER_SLOT * MAX_PORTS;
  static constexpr unsigned MAX_TX_RE   = MAX_RB * NRE * MAX_NSYMB_PER_SLOT * pusch_constants::MAX_NOF_LAYERS;
  static constexpr unsigned MAX_CH_ESTS = MAX_RX_RE * pusch_constants::MAX_NOF_LAYERS;

  static_tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), cf_t, MAX_RX_RE, re_dims>  rx_symbols;
  static_tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), cf_t, MAX_TX_RE, re_dims>  eq_symbols_expected;
  static_tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), cf_t, MAX_TX_RE, re_dims>  eq_symbols_actual;
  static_tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), float, MAX_TX_RE, re_dims> eq_noise_vars_expected;
  static_tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), float, MAX_TX_RE, re_dims> eq_noise_vars_actual;

  static_tensor<std::underlying_type_t<ch_dims>(ch_dims::nof_dims), cf_t, MAX_CH_ESTS, ch_dims> test_ch_estimates;

  std::vector<float> test_noise_vars;

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

  void SetUp() override
  {
    ASSERT_NE(equalizer_factory, nullptr) << "Cannot create equalizer factory";
    ASSERT_NE(test_equalizer, nullptr) << "Cannot create channel equalizer";
  }

  void ReadData(const ChannelEqualizerParams& t_case)
  {
    // Read the test case symbols and estimates.
    ASSERT_NO_FATAL_FAILURE(ReadReMeasurement(rx_symbols, t_case.received_symbols));
    ASSERT_NO_FATAL_FAILURE(ReadReMeasurement(eq_symbols_expected, t_case.equalized_symbols));
    ASSERT_NO_FATAL_FAILURE(ReadReMeasurement(eq_noise_vars_expected, t_case.equalized_noise_vars));
    ASSERT_NO_FATAL_FAILURE(ReadChannelEstimates(test_ch_estimates, test_noise_vars, t_case.ch_estimates));

    // Resize the equalizer output data structures.
    eq_noise_vars_actual.resize(eq_symbols_expected.get_dimensions_size());
    eq_symbols_actual.resize(eq_symbols_expected.get_dimensions_size());
  }

private:
  template <typename T>
  static void ReadReMeasurement(tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), T, re_dims>& data,
                                const re_measurement_exploded<T>&                                       data_exploded)
  {
    // Resize the tensor holding the data.
    data.resize({data_exploded.nof_prb * NRE * data_exploded.nof_symbols, data_exploded.nof_slices});

    const std::vector<T> all_syms     = data_exploded.measurements.read();
    unsigned             slice_length = data.get_dimension_size(re_dims::re);
    unsigned             nof_slices   = data.get_dimension_size(re_dims::slice);
    ASSERT_EQ(slice_length * nof_slices, all_syms.size()) << "Wrong number of symbols.";

    span<const T> all_syms_span(all_syms);
    for (unsigned i_slice = 0, skip = 0; i_slice != nof_slices; ++i_slice) {
      // Get the slice from the tensor.
      span<T> slice = data.template get_view<>({i_slice});

      // Copy the test case data.
      srsvec::copy(slice, all_syms_span.subspan(skip, slice_length));

      skip += slice_length;
    }
  }

  static void ReadChannelEstimates(channel_equalizer::ch_est_list& ch_est,
                                   std::vector<float>&             noise_vars,
                                   const ch_estimates_exploded&    ch_est_expl)
  {
    // Number of Resource Elements for a single Tx-Rx path.
    unsigned nof_re = ch_est_expl.nof_prb * NRE * ch_est_expl.nof_symbols;

    unsigned nof_rx_ports  = ch_est_expl.nof_rx_ports;
    unsigned nof_tx_layers = ch_est_expl.nof_tx_layers;

    // Resize the tensor holding the channel estimation coefficients.
    ch_est.resize({nof_re, nof_rx_ports, nof_tx_layers});

    std::vector<cf_t> all_ests  = ch_est_expl.estimates.read();
    unsigned          nof_paths = nof_tx_layers * nof_rx_ports;
    ASSERT_EQ(nof_re * nof_paths, all_ests.size()) << "Wrong number of channel estimates.";

    // Read channel estimates.
    span<const cf_t> all_ests_span(all_ests);
    unsigned         skip = 0;
    for (unsigned i_tx = 0; i_tx != nof_tx_layers; ++i_tx) {
      for (unsigned i_rx = 0; i_rx != nof_rx_ports; ++i_rx) {
        span<cf_t> path = ch_est.get_view<>({i_rx, i_tx});
        srsvec::copy(path, all_ests_span.subspan(skip, nof_re));
        skip += nof_re;
      }
    }

    // Read noise vars. For now, the noise variance is the same for all receive ports.
    noise_vars.resize(nof_rx_ports);
    for (unsigned i_rx = 0; i_rx != nof_rx_ports; ++i_rx) {
      noise_vars[i_rx] = ch_est_expl.noise_var;
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
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.scaling);

  // Assert results.
  for (unsigned i_layer = 0; i_layer != t_case.ch_estimates.nof_tx_layers; ++i_layer) {
    span<const cf_t>  eq_subcs_actual         = eq_symbols_actual.get_view<>({i_layer});
    span<const cf_t>  eq_subcs_expected       = eq_symbols_expected.get_view<>({i_layer});
    span<const float> eq_nvars_subcs_expected = eq_noise_vars_expected.get_view<>({i_layer});
    span<const float> eq_nvars_subcs_actual   = eq_noise_vars_actual.get_view<>({i_layer});

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

INSTANTIATE_TEST_SUITE_P(ChannelEqualizerTest,
                         ChannelEqualizerFixture,
                         ::testing::ValuesIn(channel_equalizer_test_data),
                         ::testing::PrintToStringParamName());

} // namespace