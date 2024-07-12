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

#include "channel_equalizer_test_data.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/upper/equalization/dynamic_ch_est_list.h"
#include "srsran/phy/upper/equalization/equalization_factories.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <srsran/phy/constants.h>

using namespace srsran;

static constexpr float max_abs_eq_symbol_error = 1e-2;
static constexpr float max_abs_eq_nvar_error   = 1e-2;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "{}_{}Tx_{}Rx_{}_{}",
             test_case.context.equalizer_type,
             test_case.context.nof_layers,
             test_case.context.nof_rx_ports,
             static_cast<unsigned>(test_case.context.scaling * 1000),
             static_cast<unsigned>(test_case.context.noise_var * 1000));
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const float> data)
{
  fmt::print(os, "{}", data);
  return os;
}

bool operator==(span<const cf_t> left, span<const cf_t> right)
{
  return std::equal(left.begin(), left.end(), right.begin(), right.end(), [](cf_t value0, cf_t value1) {
    // If one of the parts is not normal, then all parts must be equal.
    if (!std::isnormal(value0.real()) || !std::isnormal(value0.imag()) || !std::isnormal(value1.real()) ||
        !std::isnormal(value1.imag())) {
      return value0 == value1;
    }
    float absolute_error = std::abs(value0 - value1);
    float relative_error = absolute_error / std::abs(value0);
    return (relative_error <= max_abs_eq_symbol_error);
  });
}

bool operator==(span<const float> left, span<const float> right)
{
  return std::equal(left.begin(), left.end(), right.begin(), right.end(), [](float value0, float value1) {
    if (std::isinf(value0) || std::isinf(value1) || std::isnan(value0) || std::isnan(value1)) {
      return value0 == value1;
    }
    float absolute_error = std::abs(value0 - value1);
    float relative_error = absolute_error / std::abs(value0);
    return (relative_error <= max_abs_eq_nvar_error);
  });
}

} // namespace srsran

namespace {

using ChannelEqualizerParams = test_case_t;

class ChannelEqualizerFixture : public ::testing::TestWithParam<ChannelEqualizerParams>
{
protected:
  dynamic_re_buffer<cbf16_t> rx_symbols;
  dynamic_ch_est_list        test_ch_estimates;
  std::vector<cf_t>          eq_symbols_expected;
  std::vector<cf_t>          eq_symbols_actual;
  std::vector<float>         eq_noise_vars_expected;

  std::vector<float> eq_noise_vars_actual;

  std::vector<float> test_noise_vars;

  std::shared_ptr<channel_equalizer_factory> equalizer_factory;
  std::unique_ptr<channel_equalizer>         test_equalizer;

  ChannelEqualizerFixture() : TestWithParam<srsran::test_case_t>(), rx_symbols(4, 1000) {}

  void SetUp() override
  {
    const test_case_t& t_case         = GetParam();
    const std::string& equalizer_type = t_case.context.equalizer_type;

    // For now, check only Zero Forcing equalizer or MMSE equalizer with one layer - multi-layer MMSE not implemented
    // yet.
    if ((t_case.context.equalizer_type == "MMSE") && (t_case.context.nof_layers > 1)) {
      GTEST_SKIP();
    }

    // Read test case data.
    ReadData(t_case);

    // Create channel equalizer factory.
    channel_equalizer_algorithm_type algorithm_type = channel_equalizer_algorithm_type::zf;
    if (equalizer_type == "MMSE") {
      algorithm_type = channel_equalizer_algorithm_type::mmse;
    }

    equalizer_factory = create_channel_equalizer_generic_factory(algorithm_type);
    ASSERT_NE(equalizer_factory, nullptr) << "Cannot create equalizer factory";

    // Create channel equalizer.
    if (!test_equalizer) {
      test_equalizer = equalizer_factory->create();
      ASSERT_NE(test_equalizer, nullptr) << "Cannot create channel equalizer";
    }

    ASSERT_NE(equalizer_factory, nullptr) << "Cannot create equalizer factory";
    ASSERT_NE(test_equalizer, nullptr) << "Cannot create channel equalizer";
  }

  void ReadData(const ChannelEqualizerParams& t_case)
  {
    unsigned nof_re       = t_case.context.nof_re;
    unsigned nof_rx_ports = t_case.context.nof_rx_ports;
    unsigned nof_layers   = t_case.context.nof_layers;

    // Resize the equalizer input data structures.
    rx_symbols.resize(nof_rx_ports, nof_re);
    test_ch_estimates.resize(nof_re, nof_rx_ports, nof_layers);
    test_noise_vars.resize(nof_rx_ports);

    // Read the test case symbols and estimates.
    const auto rx_symbols_vector = t_case.received_symbols.read();
    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      srsvec::copy(rx_symbols.get_slice(i_port), span<const cf_t>(rx_symbols_vector).subspan(nof_re * i_port, nof_re));
    }
    srsvec::copy(test_ch_estimates.get_data(), t_case.ch_estimates.read());

    // Prepare noise variance per receive port.
    std::fill(test_noise_vars.begin(), test_noise_vars.end(), t_case.context.noise_var);

    // Resize the equalizer output data structures.
    eq_noise_vars_actual.resize(nof_re * nof_layers);
    eq_symbols_actual.resize(nof_re * nof_layers);

    // Read expected equalizer outputs.
    eq_symbols_expected    = t_case.equalized_symbols.read();
    eq_noise_vars_expected = t_case.equalized_noise_vars.read();
  }
};

TEST_P(ChannelEqualizerFixture, ChannelEqualizerTest)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();

  // Equalize the symbols coming from the Rx ports.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.context.scaling);

  // Assert results.
  ASSERT_EQ(span<const cf_t>(eq_symbols_expected), span<const cf_t>(eq_symbols_actual));
  ASSERT_EQ(span<const float>(eq_noise_vars_expected), span<const float>(eq_noise_vars_actual));
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerZeroNvar)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();

  // Force noise variances set to zero.
  srsvec::zero(test_noise_vars);

  // Update expected equalizer outputs.
  std::fill(eq_symbols_expected.begin(), eq_symbols_expected.end(), cf_t());
  std::fill(eq_noise_vars_expected.begin(), eq_noise_vars_expected.end(), std::numeric_limits<float>::infinity());

  // Equalize the symbols coming from the Rx ports using the modified noise variances.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.context.scaling);

  // Assert results.
  ASSERT_EQ(span<const cf_t>(eq_symbols_expected), span<const cf_t>(eq_symbols_actual));
  ASSERT_EQ(span<const float>(eq_noise_vars_expected), span<const float>(eq_noise_vars_actual));
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerZeroEst)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();

  // Force some channel estimates to zero and recalculate their expected output.
  {
    static constexpr unsigned stride       = 5;
    unsigned                  nof_re       = t_case.context.nof_re;
    unsigned                  nof_rx_ports = t_case.context.nof_rx_ports;
    unsigned                  nof_layers   = t_case.context.nof_layers;

    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
        for (unsigned i_re = 0; i_re < nof_re; i_re += stride) {
          test_ch_estimates.get_channel(i_port, i_layer)[i_re] = cf_t();
          eq_symbols_expected[nof_layers * i_re + i_layer]     = cf_t();
          eq_noise_vars_expected[nof_layers * i_re + i_layer]  = std::numeric_limits<float>::infinity();
        }
      }
    }
  }

  // Equalize the symbols coming from the Rx ports with the modified channel estimates.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.context.scaling);

  // Assert results.
  ASSERT_EQ(span<const cf_t>(eq_symbols_expected), span<const cf_t>(eq_symbols_actual));
  ASSERT_EQ(span<const float>(eq_noise_vars_expected), span<const float>(eq_noise_vars_actual));
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerInfNvar)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();

  // Force noise variances set to infinity.
  std::fill(test_noise_vars.begin(), test_noise_vars.end(), std::numeric_limits<float>::infinity());

  // Update expected equalizer outputs.
  std::fill(eq_symbols_expected.begin(), eq_symbols_expected.end(), cf_t());
  std::fill(eq_noise_vars_expected.begin(), eq_noise_vars_expected.end(), std::numeric_limits<float>::infinity());

  // Equalize the symbols coming from the Rx ports using the modified noise variances.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.context.scaling);

  // Assert results.
  ASSERT_EQ(span<const cf_t>(eq_symbols_expected), span<const cf_t>(eq_symbols_actual));
  ASSERT_EQ(span<const float>(eq_noise_vars_expected), span<const float>(eq_noise_vars_actual));
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerInfEst)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();

  // Force some channel estimates to zero and recalculate their expected output.
  {
    static constexpr unsigned stride       = 5;
    unsigned                  nof_re       = t_case.context.nof_re;
    unsigned                  nof_rx_ports = t_case.context.nof_rx_ports;
    unsigned                  nof_layers   = t_case.context.nof_layers;

    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
        for (unsigned i_re = 0; i_re < nof_re; i_re += stride) {
          test_ch_estimates.get_channel(i_port, i_layer)[i_re] = std::numeric_limits<float>::infinity();
          eq_symbols_expected[nof_layers * i_re + i_layer]     = cf_t();
          eq_noise_vars_expected[nof_layers * i_re + i_layer]  = std::numeric_limits<float>::infinity();
        }
      }
    }
  }

  // Equalize the symbols coming from the Rx ports with the modified channel estimates.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.context.scaling);

  // Assert results.
  ASSERT_EQ(span<const cf_t>(eq_symbols_expected), span<const cf_t>(eq_symbols_actual));
  ASSERT_EQ(span<const float>(eq_noise_vars_expected), span<const float>(eq_noise_vars_actual));
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerNanNvar)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();

  // Force noise variances set to NaN.
  std::fill(test_noise_vars.begin(), test_noise_vars.end(), std::numeric_limits<float>::quiet_NaN());

  // Update expected equalizer outputs.
  std::fill(eq_symbols_expected.begin(), eq_symbols_expected.end(), cf_t());
  std::fill(eq_noise_vars_expected.begin(), eq_noise_vars_expected.end(), std::numeric_limits<float>::infinity());

  // Equalize the symbols coming from the Rx ports using the modified noise variances.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.context.scaling);

  // Assert results.
  ASSERT_EQ(span<const cf_t>(eq_symbols_expected), span<const cf_t>(eq_symbols_actual));
  ASSERT_EQ(span<const float>(eq_noise_vars_expected), span<const float>(eq_noise_vars_actual));
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerNanEst)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();

  // Force some channel estimates to zero and recalculate their expected output.
  {
    static constexpr unsigned stride       = 5;
    unsigned                  nof_re       = t_case.context.nof_re;
    unsigned                  nof_rx_ports = t_case.context.nof_rx_ports;
    unsigned                  nof_layers   = t_case.context.nof_layers;

    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
        for (unsigned i_re = 0; i_re < nof_re; i_re += stride) {
          test_ch_estimates.get_channel(i_port, i_layer)[i_re] = std::numeric_limits<float>::quiet_NaN();
          eq_symbols_expected[nof_layers * i_re + i_layer]     = cf_t();
          eq_noise_vars_expected[nof_layers * i_re + i_layer]  = std::numeric_limits<float>::infinity();
        }
      }
    }
  }

  // Equalize the symbols coming from the Rx ports with the modified channel estimates.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.context.scaling);

  // Assert results.
  ASSERT_EQ(span<const cf_t>(eq_symbols_expected), span<const cf_t>(eq_symbols_actual));
  ASSERT_EQ(span<const float>(eq_noise_vars_expected), span<const float>(eq_noise_vars_actual));
}

TEST_P(ChannelEqualizerFixture, ChannelEqualizerNegNvar)
{
  // Load the test case data.
  const test_case_t& t_case = GetParam();

  // Force noise variances set to negative values.
  std::fill(test_noise_vars.begin(), test_noise_vars.end(), -1.0F);

  // Update expected equalizer outputs.
  std::fill(eq_symbols_expected.begin(), eq_symbols_expected.end(), cf_t());
  std::fill(eq_noise_vars_expected.begin(), eq_noise_vars_expected.end(), std::numeric_limits<float>::infinity());

  // Equalize the symbols coming from the Rx ports using the modified noise variances.
  test_equalizer->equalize(
      eq_symbols_actual, eq_noise_vars_actual, rx_symbols, test_ch_estimates, test_noise_vars, t_case.context.scaling);

  // Assert results.
  ASSERT_EQ(span<const cf_t>(eq_symbols_expected), span<const cf_t>(eq_symbols_actual));
  ASSERT_EQ(span<const float>(eq_noise_vars_expected), span<const float>(eq_noise_vars_actual));
}

INSTANTIATE_TEST_SUITE_P(ChannelEqualizerTest,
                         ChannelEqualizerFixture,
                         ::testing::ValuesIn(channel_equalizer_test_data),
                         ::testing::PrintToStringParamName());

} // namespace
