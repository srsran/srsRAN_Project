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
    // Number of RB.
    unsigned,
    // Number of antenna ports.
    unsigned>;

namespace srsran {

static float ASSERT_MAX_ERROR = 1e-5;

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

  static void SetUpTestSuite()
  {
    // Create channel precoder factory.
    if (!precoder_factory) {
      precoder_factory = create_channel_precoder_factory("auto");
      ASSERT_NE(precoder_factory, nullptr) << "Cannot create channel precoder factory";
    }
  }

  void SetUp() override
  {
    ASSERT_NE(precoder_factory, nullptr) << "Cannot create channel precoder factory";

    // Create channel precoder.
    precoder = precoder_factory->create();
    ASSERT_NE(precoder, nullptr) << "Cannot create channel precoder";
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

  // Generates the golden RE sequence, with the dimensions specified by the input buffer and precoding configuration.
  const re_buffer_reader& generate_golden(const re_buffer_reader& input, const precoding_configuration& configuration)
  {
    // Get dimensions.
    unsigned nof_re     = input.get_nof_re();
    unsigned nof_layers = configuration.get_nof_layers();
    unsigned nof_ports  = configuration.get_nof_ports();
    unsigned nof_prg    = configuration.get_nof_prg();
    unsigned nof_re_prg = configuration.get_prg_size() * NRE;

    // Resize buffer.
    golden_data.resize(nof_ports, nof_re);

    EXPECT_EQ(nof_layers, input.get_nof_slices()) << "Input and precoding matrix number of layers don't match.";

    unsigned prg_re_offset = 0;
    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      // Number of data RE that belong to the current PRG.
      unsigned re_count = std::min(nof_re_prg, nof_re - prg_re_offset);

      // View over the input data for a single PRG.
      re_buffer_reader_view input_prg(input, prg_re_offset, re_count);

      // View over the golden sequence for a single PRG.
      re_buffer_writer_view golden_prg(golden_data, prg_re_offset, re_count);

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        // View over the golden sequence for a single PRG and antenna port.
        span<cf_t> golden_re_port = golden_prg.get_slice(i_port);

        // Set the output REs to zero.
        srsvec::zero(golden_re_port);

        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          // View over the input data for a single PRG and layer.
          span<const cf_t> input_re_layer = input_prg.get_slice(i_layer);

          for (unsigned i_re = 0; i_re != re_count; ++i_re) {
            // Accumulate the contributions of all layers, scaled by each respective precoding weight.
            golden_re_port[i_re] += configuration.get_coefficient(i_layer, i_port, i_prg) * input_re_layer[i_re];
          }
        }
      }

      // Advance buffers.
      prg_re_offset += re_count;
    }

    EXPECT_EQ(nof_re, prg_re_offset)
        << "The number of RE of the golden sequence does not match the number of input RE.";

    return golden_data;
  }

  // Generates a precoding configuration with the specified dimensions and random precoding coefficients.
  precoding_configuration generate_random_precoding(unsigned nof_layers, unsigned nof_ports, unsigned nof_rb)
  {
    // Create configuration.
    precoding_configuration config(nof_layers, nof_ports, 1, nof_rb);

    // Fill the precoding matrix with random values.
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        cf_t weight = {weight_dist(rgen), weight_dist(rgen)};
        config.set_coefficient(weight, i_layer, i_port, 0);
      }
    }

    return config;
  }

private:
  // Buffer for holding randomly generated data.
  static_re_buffer<precoding_constants::MAX_NOF_LAYERS, NRE * MAX_RB * MAX_NSYMB_PER_SLOT> random_data;

  // Buffer for holding the golden sequence.
  static_re_buffer<precoding_constants::MAX_NOF_PORTS, NRE * MAX_RB * MAX_NSYMB_PER_SLOT> golden_data;

  // Uniform distribution to generate data samples.
  std::uniform_real_distribution<float> data_dist{-10.0, +10.0};

  // Uniform distribution to generate precoding coefficients.
  std::uniform_real_distribution<float> weight_dist{-1, +1};
};

std::shared_ptr<channel_precoder_factory> PrecodingFixture::precoder_factory = nullptr;
std::unique_ptr<channel_precoder>         PrecodingFixture::precoder         = nullptr;

// Tests a precoding configuration with parametrized dimensions and random weights.
TEST_P(PrecodingFixture, RandomWeights)
{
  const MultiplePRGParams& test_case = GetParam();

  unsigned nof_rb    = std::get<0>(test_case);
  unsigned nof_ports = std::get<1>(test_case);

  // Number of resource elements per layer.
  unsigned nof_re = nof_rb * NRE;

  // Buffer to hold the precoded RE.
  static_re_buffer<precoding_constants::MAX_NOF_PORTS, NRE * MAX_RB * MAX_NSYMB_PER_SLOT> precoding_buffer(nof_ports,
                                                                                                           nof_re);
  for (unsigned nof_layers = 1; nof_layers <= nof_ports; ++nof_layers) {
    // Generate random RE arranged by layers.
    const re_buffer_reader& input_data = generate_random_data(nof_layers, nof_re);

    // Create a random precoding configuration
    precoding_configuration precoding_config = generate_random_precoding(nof_layers, nof_ports, nof_rb);

    // Generate the golden precoded data.
    const re_buffer_reader& golden = generate_golden(input_data, precoding_config);

    // Apply precoding.
    precoder->apply_precoding(precoding_buffer, input_data, precoding_config.get_prg_coefficients(0));

    // For each antenna port, compare the precoded RE with the golden sequence for all RE and PRG.
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      ASSERT_EQ(span<const cf_t>(golden.get_slice(i_port)), span<const cf_t>(precoding_buffer.get_slice(i_port)));
    }
  }
}

INSTANTIATE_TEST_SUITE_P(RandomWeights,
                         PrecodingFixture,
                         ::testing::Combine(
                             // Number of RB.
                             ::testing::Values(13, 51, MAX_RB),
                             // Number of antenna ports.
                             ::testing::Values(1, 2, 4)));
} // namespace
