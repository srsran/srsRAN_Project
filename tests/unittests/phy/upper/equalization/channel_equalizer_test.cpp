/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "channel_equalizer_test_data.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/srsvec/zero.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "{}_{}Tx_{}Rx",
             test_case.equalizer_type,
             test_case.ch_estimates.nof_tx_layers,
             test_case.ch_estimates.nof_rx_ports);
  return os;
}

} // namespace srsran

namespace {

using ChannelEqualizerParams = test_case_t;

using re_dims = channel_equalizer::re_list::dims;
using ch_dims = channel_equalizer::ch_est_list::dims;

class ChannelEqualizerFixture : public ::testing::TestWithParam<ChannelEqualizerParams>
{
protected:
  static constexpr float MAX_ERROR_EQ = 1e-3;

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
    ASSERT_NO_FATAL_FAILURE(ReadResourceElements(rx_symbols, t_case.received_symbols));
    ASSERT_NO_FATAL_FAILURE(ReadResourceElements(eq_symbols_expected, t_case.equalized_symbols));
    ASSERT_NO_FATAL_FAILURE(ReadResourceElements(eq_noise_vars_expected, t_case.equalized_noise_vars));
    ASSERT_NO_FATAL_FAILURE(ReadChannelEstimates(test_ch_estimates, test_noise_vars, t_case.ch_estimates));

    // Resize the equalizer output data structures.
    eq_noise_vars_actual.resize(eq_symbols_expected.get_dimensions_size());
    eq_symbols_actual.resize(eq_symbols_expected.get_dimensions_size());
  }

private:
  template <typename T>
  static void ReadResourceElements(tensor<std::underlying_type_t<re_dims>(re_dims::nof_dims), T, re_dims>& data,
                                   const re_exploded<T>& data_exploded)
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

bool xnor(bool a, bool b)
{
  return ((a && b) || (!a && !b));
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerZeroNvar)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();
  ReadData(t_case);

  // For now, check only Zero Forcing equalizer, since MMSE equalizer is not implemented yet.
  if (t_case.equalizer_type != "ZF") {
    GTEST_SKIP();
  }

  ReadData(t_case);

  unsigned nof_tx_layers = t_case.transmitted_symbols.nof_slices;
  unsigned nof_rx_ports  = t_case.received_symbols.nof_slices;

  // Create noise variances set to zero.
  std::vector<float> zero_noise_vars(nof_rx_ports);
  srsvec::zero(zero_noise_vars);

  // Equalize the symbols coming from the Rx ports using the modified noise variances.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, zero_noise_vars, t_case.scaling);

  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    span<const float> noise_var  = eq_noise_vars_actual.get_view({i_layer});
    span<const cf_t>  eq_symbols = eq_symbols_actual.get_view({i_layer});

    // Assert that the noise variances are set to infinity when the input noise variance is zero.
    ASSERT_TRUE(std::all_of(noise_var.begin(), noise_var.end(), [](float nvar) { return std::isinf(nvar); }))
        << fmt::format("Noise variances are not set to infinity when input noise variance is zero for Tx layer {}",
                       i_layer);

    // Assert that the equalized symbols are set to zero when the input noise variance is zero.
    ASSERT_TRUE(std::all_of(eq_symbols.begin(), eq_symbols.end(), [](cf_t symb) { return symb == cf_t(0, 0); }))
        << fmt::format("Equalized symbols are not set to zero when input noise variance is zero for Tx layer {}",
                       i_layer);
  }
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerZeroEst)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();
  ReadData(t_case);

  // For now, check only Zero Forcing equalizer, since MMSE equalizer is not implemented yet.
  if (t_case.equalizer_type != "ZF") {
    GTEST_SKIP();
  }

  ReadData(t_case);

  // Set some channel estimates to zero.
  static constexpr unsigned stride        = 5;
  unsigned                  nof_tx_layers = t_case.transmitted_symbols.nof_slices;
  unsigned                  nof_rx_ports  = t_case.received_symbols.nof_slices;
  unsigned                  nof_res       = t_case.received_symbols.nof_prb * NRE * t_case.received_symbols.nof_symbols;

  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      for (unsigned i_re = 0; i_re < nof_res; i_re += stride) {
        test_ch_estimates[{i_re, i_port, i_layer}] = 0;
      }
    }
  }

  // Equalize the symbols coming from the Rx ports with the modified channel estimates.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.scaling);

  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    span<const cf_t>  reference  = test_ch_estimates.get_view({0, i_layer});
    span<const float> noise_var  = eq_noise_vars_actual.get_view({i_layer});
    span<const cf_t>  eq_symbols = eq_symbols_actual.get_view({i_layer});
    // Assert that the noise variances are set to infinity when the channel estimate is zero.
    ASSERT_TRUE(std::equal(reference.begin(), reference.end(), noise_var.begin(), [](cf_t a, float b) {
      return xnor(a == cf_t(0, 0), std::isinf(b));
    })) << fmt::format("Noise variances are not set to infinity when estimate is zero for Tx layer {}", i_layer);

    // Assert that the equalized symbols are set to zero when the channel estimate is zero.
    ASSERT_TRUE(std::equal(reference.begin(), reference.end(), eq_symbols.begin(), [](cf_t a, cf_t b) {
      return xnor(a == cf_t(0, 0), b == cf_t(0, 0));
    })) << fmt::format("Equalized symbols are not zero when estimate is zero for Tx layer {}", i_layer);
  }
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerInfNvar)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();
  ReadData(t_case);

  // For now, check only Zero Forcing equalizer, since MMSE equalizer is not implemented yet.
  if (t_case.equalizer_type != "ZF") {
    GTEST_SKIP();
  }

  ReadData(t_case);

  unsigned nof_tx_layers = t_case.transmitted_symbols.nof_slices;
  unsigned nof_rx_ports  = t_case.received_symbols.nof_slices;

  // Create noise variances set to infinity.
  std::vector<float> inf_noise_vars(nof_rx_ports);
  std::fill_n(inf_noise_vars.begin(), nof_rx_ports, std::numeric_limits<float>::infinity());

  // Equalize the symbols coming from the Rx ports using the modified noise variances.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, inf_noise_vars, t_case.scaling);

  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    span<const float> noise_var  = eq_noise_vars_actual.get_view({i_layer});
    span<const cf_t>  eq_symbols = eq_symbols_actual.get_view({i_layer});

    // Assert that the noise variances are set to infinity when the input noise variance is infinity.
    ASSERT_TRUE(std::all_of(noise_var.begin(), noise_var.end(), [](float nvar) { return std::isinf(nvar); }))
        << fmt::format("Noise variances are not set to infinity when input noise variance is infinity for Tx layer {}",
                       i_layer);

    // Assert that the equalized symbols are set to zero when the input noise variance is infinity.
    ASSERT_TRUE(std::all_of(eq_symbols.begin(), eq_symbols.end(), [](cf_t symb) { return symb == cf_t(0, 0); }))
        << fmt::format("Equalized symbols are not set to zero when input noise variance is infinity for Tx layer {}",
                       i_layer);
  }
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerInfEst)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();
  ReadData(t_case);

  // For now, check only Zero Forcing equalizer, since MMSE equalizer is not implemented yet.
  if (t_case.equalizer_type != "ZF") {
    GTEST_SKIP();
  }

  ReadData(t_case);

  // Set some channel estimates to infinity.
  static constexpr unsigned stride        = 5;
  unsigned                  nof_tx_layers = t_case.transmitted_symbols.nof_slices;
  unsigned                  nof_rx_ports  = t_case.received_symbols.nof_slices;
  unsigned                  nof_res       = t_case.received_symbols.nof_prb * NRE * t_case.received_symbols.nof_symbols;

  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      for (unsigned i_re = 0; i_re < nof_res; i_re += stride) {
        test_ch_estimates[{i_re, i_port, i_layer}] =
            cf_t(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
      }
    }
  }

  // Equalize the symbols coming from the Rx ports with the modified channel estimates.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.scaling);

  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    span<const cf_t>  reference  = test_ch_estimates.get_view({0, i_layer});
    span<const float> noise_var  = eq_noise_vars_actual.get_view({i_layer});
    span<const cf_t>  eq_symbols = eq_symbols_actual.get_view({i_layer});
    // Assert that the noise variances are set to infinity when the channel estimate is infinity.
    ASSERT_TRUE(std::equal(reference.begin(), reference.end(), noise_var.begin(), [](cf_t a, float b) {
      return xnor((std::isinf(a.real())) && (std::isinf(a.imag())), std::isinf(b));
    })) << fmt::format("Noise variances are not set to infinity when estimate is infinity for Tx layer {}", i_layer);

    // Assert that the equalized symbols are set to zero when the channel estimate is infinity.
    ASSERT_TRUE(std::equal(reference.begin(), reference.end(), eq_symbols.begin(), [](cf_t a, cf_t b) {
      return xnor((std::isinf(a.real())) && (std::isinf(a.imag())), b == cf_t(0, 0));
    })) << fmt::format("Equalized symbols are not zero when estimate is infinity for Tx layer {}", i_layer);
  }
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerNanNvar)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();
  ReadData(t_case);

  // For now, check only Zero Forcing equalizer, since MMSE equalizer is not implemented yet.
  if (t_case.equalizer_type != "ZF") {
    GTEST_SKIP();
  }

  ReadData(t_case);

  unsigned nof_tx_layers = t_case.transmitted_symbols.nof_slices;
  unsigned nof_rx_ports  = t_case.received_symbols.nof_slices;

  // Create noise vars set to NaN.
  std::vector<float> nan_noise_vars(nof_rx_ports);
  std::fill_n(nan_noise_vars.begin(), nof_rx_ports, std::numeric_limits<float>::quiet_NaN());

  // Equalize the symbols coming from the Rx ports using the modified noise variances.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, nan_noise_vars, t_case.scaling);

  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    span<const float> noise_var  = eq_noise_vars_actual.get_view({i_layer});
    span<const cf_t>  eq_symbols = eq_symbols_actual.get_view({i_layer});

    // Assert that the noise variances are set to infinity when the input noise variance is NaN.
    ASSERT_TRUE(std::all_of(noise_var.begin(), noise_var.end(), [](float nvar) { return std::isinf(nvar); }))
        << fmt::format("Noise variances are not set to infinity when input noise variance is NaN for Tx layer {}",
                       i_layer);

    // Assert that the equalized symbols are set to zero when the input noise variance is NaN.
    ASSERT_TRUE(std::all_of(eq_symbols.begin(), eq_symbols.end(), [](cf_t symb) { return symb == cf_t(0, 0); }))
        << fmt::format("Equalized symbols are not set to zero when input noise variance is NaN for Tx layer {}",
                       i_layer);
  }
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerNanEst)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();
  ReadData(t_case);

  // For now, check only Zero Forcing equalizer, since MMSE equalizer is not implemented yet.
  if (t_case.equalizer_type != "ZF") {
    GTEST_SKIP();
  }

  ReadData(t_case);

  // Set some channel estimates to NaN.
  static constexpr unsigned stride        = 5;
  unsigned                  nof_tx_layers = t_case.transmitted_symbols.nof_slices;
  unsigned                  nof_rx_ports  = t_case.received_symbols.nof_slices;
  unsigned                  nof_res       = t_case.received_symbols.nof_prb * NRE * t_case.received_symbols.nof_symbols;

  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      for (unsigned i_re = 0; i_re < nof_res; i_re += stride) {
        test_ch_estimates[{i_re, i_port, i_layer}] =
            cf_t(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
      }
    }
  }

  // Equalize the symbols coming from the Rx ports with the modified channel estimates.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.scaling);

  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    span<const cf_t>  reference  = test_ch_estimates.get_view({0, i_layer});
    span<const float> noise_var  = eq_noise_vars_actual.get_view({i_layer});
    span<const cf_t>  eq_symbols = eq_symbols_actual.get_view({i_layer});
    // Assert that the noise variances are set to infinity when the channel estimate is NaN.
    ASSERT_TRUE(std::equal(reference.begin(), reference.end(), noise_var.begin(), [](cf_t a, float b) {
      return xnor((std::isnan(a.real())) && (std::isnan(a.imag())), std::isinf(b));
    })) << fmt::format("Noise variances are not set to infinity when estimate is NaN for Tx layer {}", i_layer);

    // Assert that the equalized symbols are set to zero when the channel estimate is NaN.
    ASSERT_TRUE(std::equal(reference.begin(), reference.end(), eq_symbols.begin(), [](cf_t a, cf_t b) {
      return xnor((std::isnan(a.real())) && (std::isnan(a.imag())), b == cf_t(0, 0));
    })) << fmt::format("Equalized symbols are not zero when estimate is NaN for Tx layer {}", i_layer);
  }
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerNegNvar)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();
  ReadData(t_case);

  // For now, check only Zero Forcing equalizer, since MMSE equalizer is not implemented yet.
  if (t_case.equalizer_type != "ZF") {
    GTEST_SKIP();
  }

  ReadData(t_case);

  unsigned nof_tx_layers = t_case.transmitted_symbols.nof_slices;
  unsigned nof_rx_ports  = t_case.received_symbols.nof_slices;

  // Create noise vars set to a negative value.
  std::vector<float> neg_noise_vars(nof_rx_ports);
  std::fill_n(neg_noise_vars.begin(), nof_rx_ports, -1.0F);

  // Equalize the symbols coming from the Rx ports using the modified noise variances.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, neg_noise_vars, t_case.scaling);

  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    span<const float> noise_var  = eq_noise_vars_actual.get_view({i_layer});
    span<const cf_t>  eq_symbols = eq_symbols_actual.get_view({i_layer});

    // Assert that the noise variances are set to infinity when the input noise variance is negative.
    ASSERT_TRUE(std::all_of(noise_var.begin(), noise_var.end(), [](float nvar) { return std::isinf(nvar); }))
        << fmt::format("Noise variances are not set to infinity when input noise variance is negative for Tx layer {}",
                       i_layer);

    // Assert that the equalized symbols are set to zero when the input noise variance is negative.
    ASSERT_TRUE(std::all_of(eq_symbols.begin(), eq_symbols.end(), [](cf_t symb) { return symb == cf_t(0, 0); }))
        << fmt::format("Equalized symbols are not set to zero when input noise variance is negative for Tx layer {}",
                       i_layer);
  }
}

INSTANTIATE_TEST_SUITE_P(ChannelEqualizerTest,
                         ChannelEqualizerFixture,
                         ::testing::ValuesIn(channel_equalizer_test_data),
                         ::testing::PrintToStringParamName());

} // namespace
