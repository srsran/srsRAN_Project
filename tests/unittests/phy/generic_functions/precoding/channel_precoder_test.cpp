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

#include "srsran/phy/constants.h"
#include "srsran/phy/generic_functions/precoding/precoding_factories.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/srsvec/zero.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

// Pseudo-random number generator.
static std::mt19937 rgen;

using MultiplePRGParams = std::tuple<
    // Precoder type: generic, avx2, avx512
    std::string,
    // Number of RB.
    unsigned,
    // Number of antenna ports.
    unsigned>;

namespace srsran {

static float ASSERT_MAX_ERROR = 1e-4;

static std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

static bool operator==(span<const cf_t> lhs, span<const cf_t> rhs)
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](cf_t lhs_val, cf_t rhs_val) {
    return (std::abs(lhs_val - rhs_val) < ASSERT_MAX_ERROR);
  });
}

} // namespace srsran

namespace {
class PrecodingFixture : public ::testing::TestWithParam<MultiplePRGParams>
{
protected:
  static std::shared_ptr<channel_precoder_factory> precoder_factory;
  static std::unique_ptr<channel_precoder>         precoder;

  static void SetUpTestSuite() {}

  void SetUp() override
  {
    const std::string& type = std::get<0>(GetParam());

    // Create channel precoder factory.
    precoder_factory = create_channel_precoder_factory(type);
    ASSERT_NE(precoder_factory, nullptr) << "Cannot create channel precoder factory";

    ASSERT_NE(precoder_factory, nullptr) << "Cannot create channel precoder factory";

    // Create channel precoder.
    precoder = precoder_factory->create();

    // If the precoder type is not supported, skip the test.
    if (!precoder) {
      GTEST_SKIP();
    }
  }

  // Generates and returns random RE values, as many as the specified number of layers and RE.
  const re_buffer_reader& generate_random_data(unsigned nof_layers, unsigned nof_re)
  {
    // Resize buffer.
    random_data.resize(nof_layers, nof_re);

    // Fill buffer with random values.
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (cf_t& val : random_data.get_slice(i_layer)) {
        val = {data_dist(rgen), data_dist(rgen)};
      }
    }

    return random_data;
  }

  // Generates and returns random RE values, as many as the specified number of layers and RE.
  span<const ci8_t> generate_random_data_ci8(unsigned nof_layers, unsigned nof_re)
  {
    // Resize buffer.
    random_data_ci8.resize(nof_layers * nof_re);

    // Fill buffer with random values.
    std::generate(random_data_ci8.begin(), random_data_ci8.end(), [this]() {
      return ci8_t(data_dist_i8(rgen), data_dist_i8(rgen));
    });

    return random_data_ci8;
  }

  // Generates the golden RE sequence, with the dimensions specified by the input buffer and precoding matrix.
  const re_buffer_reader& generate_golden(const re_buffer_reader&        input,
                                          const precoding_weight_matrix& precoding_matrix)
  {
    // Get dimensions.
    unsigned nof_re     = input.get_nof_re();
    unsigned nof_layers = precoding_matrix.get_nof_layers();
    unsigned nof_ports  = precoding_matrix.get_nof_ports();

    // Resize buffer.
    golden_data.resize(nof_ports, nof_re);

    EXPECT_EQ(nof_layers, input.get_nof_slices()) << "Input and precoding matrix number of layers don't match.";

    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      // View over the golden sequence for a single PRG and antenna port.
      span<cf_t> golden_re_port = golden_data.get_slice(i_port);

      // Set the output REs to zero.
      srsvec::zero(golden_re_port);

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        // View over the input data for a single layer.
        span<const cf_t> input_re_layer = input.get_slice(i_layer);

        for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
          // Accumulate the contributions of all layers, scaled by each respective precoding weight.
          golden_re_port[i_re] += precoding_matrix.get_coefficient(i_layer, i_port) * input_re_layer[i_re];
        }
      }
    }

    return golden_data;
  }

  // Generates the golden RE sequence, with the dimensions specified by the input buffer and precoding matrix.
  const re_buffer_reader& generate_golden(span<const ci8_t> input, const precoding_weight_matrix& precoding_matrix)
  {
    // Get dimensions.
    unsigned nof_layers = precoding_matrix.get_nof_layers();
    unsigned nof_ports  = precoding_matrix.get_nof_ports();
    unsigned nof_re     = input.size() / nof_layers;

    // Resize buffer.
    golden_data.resize(nof_ports, nof_re);

    EXPECT_EQ(input.size() % nof_layers, 0) << "Input and precoding matrix number of layers are not consistent.";

    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      // View over the golden sequence for a single PRG and antenna port.
      span<cf_t> golden_re_port = golden_data.get_slice(i_port);

      // Set the output REs to zero.
      srsvec::zero(golden_re_port);

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
          // Accumulate the contributions of all layers, scaled by each respective precoding weight.
          golden_re_port[i_re] +=
              precoding_matrix.get_coefficient(i_layer, i_port) * to_cf(input[nof_layers * i_re + i_layer]);
        }
      }
    }

    return golden_data;
  }

  // Generates a precoding configuration with the specified dimensions and random precoding coefficients.
  precoding_weight_matrix generate_random_precoding(unsigned nof_layers, unsigned nof_ports)
  {
    // Create configuration.
    precoding_weight_matrix config(nof_layers, nof_ports);

    // Fill the precoding matrix with random values.
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        cf_t weight = {weight_dist(rgen), weight_dist(rgen)};
        config.set_coefficient(weight, i_layer, i_port);
      }
    }

    return config;
  }

private:
  // Buffer for holding floating point randomly generated data.
  static_re_buffer<precoding_constants::MAX_NOF_LAYERS, NRE * MAX_RB * MAX_NSYMB_PER_SLOT> random_data;

  // Buffer for holding ci8_t randomly generated data.
  std::vector<ci8_t> random_data_ci8;

  // Buffer for holding the golden sequence.
  static_re_buffer<precoding_constants::MAX_NOF_PORTS, NRE * MAX_RB * MAX_NSYMB_PER_SLOT> golden_data;

  // Uniform real distribution to generate data samples.
  std::uniform_real_distribution<float> data_dist{-10.0, +10.0};

  // Uniform integer distribution to generate data samples.
  std::uniform_int_distribution<int8_t> data_dist_i8{-120, 120};

  // Uniform distribution to generate precoding coefficients.
  std::uniform_real_distribution<float> weight_dist{-1, +1};
};

std::shared_ptr<channel_precoder_factory> PrecodingFixture::precoder_factory = nullptr;
std::unique_ptr<channel_precoder>         PrecodingFixture::precoder         = nullptr;

// Tests a precoding configuration with parametrized dimensions and random weights.
TEST_P(PrecodingFixture, RandomWeightsCft)
{
  const MultiplePRGParams& test_case = GetParam();

  unsigned nof_rb    = std::get<1>(test_case);
  unsigned nof_ports = std::get<2>(test_case);

  // Number of resource elements per layer.
  unsigned nof_re = nof_rb * NRE;

  // Buffer to hold the precoded RE.
  static_re_buffer<precoding_constants::MAX_NOF_PORTS, NRE * MAX_RB * MAX_NSYMB_PER_SLOT> precoding_buffer(nof_ports,
                                                                                                           nof_re);
  for (unsigned nof_layers = 1; nof_layers <= nof_ports; ++nof_layers) {
    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_re);

    // Create a random precoding configuration
    precoding_weight_matrix precoding_matrix = generate_random_precoding(nof_layers, nof_ports);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, precoding_matrix);

    // Apply precoding.
    precoder->apply_precoding(precoding_buffer, input_data, precoding_matrix);

    // For each antenna port, compare the precoded RE with the golden sequence for all RE and PRG.
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      ASSERT_EQ(span<const cf_t>(golden.get_slice(i_port)), span<const cf_t>(precoding_buffer.get_slice(i_port)));
    }
  }
}

// Tests a precoding configuration with parametrized dimensions and random weights.
TEST_P(PrecodingFixture, RandomWeightsCi8)
{
  const MultiplePRGParams& test_case = GetParam();

  unsigned nof_rb    = std::get<1>(test_case);
  unsigned nof_ports = std::get<2>(test_case);

  // Number of resource elements per layer.
  unsigned nof_re = nof_rb * NRE;

  // Buffer to hold the precoded RE.
  static_re_buffer<precoding_constants::MAX_NOF_PORTS, NRE * MAX_RB * MAX_NSYMB_PER_SLOT> precoding_buffer(nof_ports,
                                                                                                           nof_re);
  for (unsigned nof_layers = 1; nof_layers <= nof_ports; ++nof_layers) {
    // Generate random RE arranged by layers.
    span<const ci8_t> input_data = generate_random_data_ci8(nof_layers, nof_re);

    // Create a random precoding configuration
    precoding_weight_matrix precoding_matrix = generate_random_precoding(nof_layers, nof_ports);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, precoding_matrix);

    // Apply precoding.
    precoder->apply_layer_map_and_precoding(precoding_buffer, input_data, precoding_matrix);

    // For each antenna port, compare the precoded RE with the golden sequence for all RE and PRG.
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      ASSERT_EQ(span<const cf_t>(golden.get_slice(i_port)), span<const cf_t>(precoding_buffer.get_slice(i_port)))
          << " nof_ports=" << nof_ports << " nof_layers=" << nof_layers;
    }
  }
}

INSTANTIATE_TEST_SUITE_P(RandomWeights,
                         PrecodingFixture,
                         ::testing::Combine(
                             //
                             ::testing::Values("generic"
#ifdef __x86_64__
                                               // Precoder types for x86.
                                               ,
                                               "avx2",
                                               "avx512"
#endif // __x86_64__
#ifdef __aarch64__
                                               // Precoder types for ARM.
                                               ,
                                               "neon"
#endif // __aarch64__
                                               ),
                             // Number of RB.
                             ::testing::Values(13, 51, MAX_RB),
                             // Number of antenna ports.
                             ::testing::Values(1, 2, 4)));
} // namespace
