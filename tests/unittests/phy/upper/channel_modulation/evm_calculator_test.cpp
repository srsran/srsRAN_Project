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

#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/support/complex_normal_random.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

using EvmCalculatorTestParams = std::tuple<srsran::modulation_scheme, float, unsigned>;

namespace srsran {

static std::ostream& operator<<(std::ostream& os, const modulation_scheme& modulation)
{
  fmt::print(os, "{}", srsran::to_string(modulation));
  return os;
}

} // namespace srsran

using namespace srsran;

std::mt19937 rgen(0);

class EvmCalculatorFixture : public ::testing::TestWithParam<EvmCalculatorTestParams>
{
protected:
  std::unique_ptr<evm_calculator>      calculator;
  std::unique_ptr<demodulation_mapper> demapper;
  std::unique_ptr<modulation_mapper>   mapper;

  void SetUp() override
  {
    std::shared_ptr<channel_modulation_factory> factory = create_channel_modulation_sw_factory();
    ASSERT_NE(factory, nullptr);

    calculator = factory->create_evm_calculator();
    ASSERT_TRUE(calculator);

    demapper = factory->create_demodulation_mapper();
    ASSERT_TRUE(demapper);

    mapper = factory->create_modulation_mapper();
    ASSERT_TRUE(mapper);
  }
};

TEST_P(EvmCalculatorFixture, AWGN)
{
  modulation_scheme modulation    = std::get<0>(GetParam());
  float             noise_var     = std::get<1>(GetParam());
  unsigned          nof_symbols   = std::get<2>(GetParam());
  unsigned          nof_bits      = nof_symbols * get_bits_per_symbol(modulation);
  float             expected_evm  = std::sqrt(noise_var);
  float             max_evm_error = expected_evm / 10.0f;

  complex_normal_distribution<cf_t> dist(0.0f, std::sqrt(noise_var));

  // Fill buffer with random data.
  dynamic_bit_buffer data(nof_bits);
  for (unsigned i_byte = 0, i_byte_end = nof_bits / 8; i_byte != i_byte_end; ++i_byte) {
    data.set_byte(rgen() & mask_lsb_ones<unsigned>(8), i_byte);
  }
  if (nof_bits % 8 != 0) {
    data.insert((rgen() & mask_lsb_ones<unsigned>(nof_bits % 8)), 8 * (nof_bits / 8), nof_bits % 8);
  }

  // Create modulated data.
  std::vector<cf_t> modulated(nof_symbols);
  mapper->modulate(modulated, data, modulation);

  // Add noise.
  for (cf_t& symbol : modulated) {
    symbol += dist(rgen);
  }

  // Fill noise variance.
  std::vector<float> noise_vars(nof_symbols);
  std::fill(noise_vars.begin(), noise_vars.end(), noise_var);

  // Soft demapper.
  std::vector<log_likelihood_ratio> soft_bits(nof_bits);
  demapper->demodulate_soft(soft_bits, modulated, noise_vars, modulation);

  // Calculate EVM.
  float evm = calculator->calculate(soft_bits, modulated, modulation);

  // Assert EVM is within a fifth of the noise variance.
  ASSERT_NEAR(evm, expected_evm, max_evm_error);
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(ChannelModulation,
                         EvmCalculatorFixture,
                         ::testing::Combine(::testing::Values(modulation_scheme::BPSK,
                                                              modulation_scheme::PI_2_BPSK,
                                                              modulation_scheme::QPSK,
                                                              modulation_scheme::QAM16,
                                                              modulation_scheme::QAM64,
                                                              modulation_scheme::QAM256),
                                            ::testing::Values(0.001f, 0.0001f, 0.00001f),
                                            ::testing::Values(100, 1000, 10000)));
