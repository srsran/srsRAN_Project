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

#include "srsran/phy/support/precoding_configuration.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

// Pseudo-random number generator.
static std::mt19937 rgen;

namespace {

class PrecodingConfigFixture : public ::testing::Test
{
public:
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

  // Uniform distribution to generate precoding coefficients.
  std::uniform_real_distribution<float> weight_dist{-1, +1};
};

} // namespace

TEST_F(PrecodingConfigFixture, DefaultConstructor)
{
  precoding_configuration config;

  // Assert default constructor dimensions.
  ASSERT_EQ(config.get_nof_ports(), 0) << "Number of ports is not zero";
  ASSERT_EQ(config.get_nof_layers(), 0) << "Number of layers is not zero";
  ASSERT_EQ(config.get_nof_prg(), 0) << "Number of PRG is not zero";
  ASSERT_EQ(config.get_prg_size(), MAX_NOF_PRBS) << "PRG size does not cover the whole bandwidth";
}

TEST_F(PrecodingConfigFixture, ParametrizedConstructor)
{
  constexpr unsigned nof_layers = 1;
  constexpr unsigned nof_ports  = 2;
  constexpr unsigned nof_prg    = 3;
  constexpr unsigned prg_size   = 4;

  // Use parametrized constructor.
  precoding_configuration config(nof_layers, nof_ports, nof_prg, prg_size);

  // Assert dimensions.
  ASSERT_EQ(config.get_nof_ports(), nof_ports) << "Number of ports does not match";
  ASSERT_EQ(config.get_nof_layers(), nof_layers) << "Number of layers does not match";
  ASSERT_EQ(config.get_nof_prg(), nof_prg) << "Number of PRG does not match";
  ASSERT_EQ(config.get_prg_size(), prg_size) << "PRG size does not match";
}

TEST_F(PrecodingConfigFixture, WidebandConstructor)
{
  constexpr unsigned nof_layers = 2;
  constexpr unsigned nof_ports  = 3;

  precoding_weight_matrix precoding_matrix = generate_random_precoding(nof_layers, nof_ports);

  // Use wideband constructor.
  precoding_configuration config = precoding_configuration::make_wideband(precoding_matrix);

  // Assert dimensions.
  ASSERT_EQ(config.get_nof_ports(), nof_ports) << "Number of ports does not match";
  ASSERT_EQ(config.get_nof_layers(), nof_layers) << "Number of layers does not match";
  ASSERT_EQ(config.get_nof_prg(), 1) << "Number of PRG does not match";
  ASSERT_EQ(config.get_prg_size(), MAX_NOF_PRBS) << "PRG size does not match";

  // Assert that all weights match the original matrix.
  const precoding_weight_matrix& extracted_matrix = config.get_prg_coefficients(0);
  ASSERT_EQ(precoding_matrix, extracted_matrix) << "Extracted and original matrices don't match";
}

TEST_F(PrecodingConfigFixture, CopyConstructor)
{
  constexpr unsigned nof_layers = 4;
  constexpr unsigned nof_ports  = 4;
  constexpr unsigned nof_prg    = 5;
  constexpr unsigned prg_size   = 11;

  // Original configuration.
  precoding_configuration config(nof_layers, nof_ports, nof_prg, prg_size);

  // Set PRG matrices on the configuration.
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    config.set_prg_coefficients(generate_random_precoding(nof_layers, nof_ports), i_prg);
  }

  // Use the copy constructor.
  precoding_configuration config_copy(config);

  // Assert that all precoding weigths are copied.
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    ASSERT_EQ(config.get_prg_coefficients(i_prg), config_copy.get_prg_coefficients(i_prg))
        << "Copied and original matrices don't match";
  }

  // Assert dimensions.
  ASSERT_EQ(config_copy.get_nof_ports(), config.get_nof_ports()) << "Number of ports does not match";
  ASSERT_EQ(config_copy.get_nof_layers(), config.get_nof_layers()) << "Number of layers does not match";
  ASSERT_EQ(config_copy.get_nof_prg(), config.get_nof_prg()) << "Number of PRG does not match";
  ASSERT_EQ(config_copy.get_prg_size(), config.get_prg_size()) << "PRG size does not match";
}

TEST_F(PrecodingConfigFixture, AssignmentOperator)
{
  constexpr unsigned nof_layers = 4;
  constexpr unsigned nof_ports  = 4;
  constexpr unsigned nof_prg    = 5;
  constexpr unsigned prg_size   = 11;

  // Original precoding configuration.
  precoding_configuration config(nof_layers, nof_ports, nof_prg, prg_size);

  // Set PRG matrices on the configuration.
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    config.set_prg_coefficients(generate_random_precoding(nof_layers, nof_ports), i_prg);
  }

  // Use the assignment operator.
  precoding_configuration config_copy = config;

  // Assert that all precoding weigths are copied.
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    ASSERT_EQ(config.get_prg_coefficients(i_prg), config_copy.get_prg_coefficients(i_prg))
        << "Copied and original matrices don't match";
  }

  // Assert dimensions.
  ASSERT_EQ(config_copy.get_nof_ports(), config.get_nof_ports()) << "Number of ports does not match";
  ASSERT_EQ(config_copy.get_nof_layers(), config.get_nof_layers()) << "Number of layers does not match";
  ASSERT_EQ(config_copy.get_nof_prg(), config.get_nof_prg()) << "Number of PRG does not match";
  ASSERT_EQ(config_copy.get_prg_size(), config.get_prg_size()) << "PRG size does not match";
}

TEST_F(PrecodingConfigFixture, SetGetPrgCoefficients)
{
  constexpr unsigned nof_layers = 3;
  constexpr unsigned nof_ports  = 4;
  constexpr unsigned nof_prg    = 3;
  constexpr unsigned prg_size   = 4;

  std::array<precoding_weight_matrix, nof_prg> precoding_matrix_list;

  // Use parametrized constructor.
  precoding_configuration config(nof_layers, nof_ports, nof_prg, prg_size);

  // Set PRG matrices on the configuration.
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    precoding_matrix_list[i_prg] = generate_random_precoding(nof_layers, nof_ports);
    config.set_prg_coefficients(precoding_matrix_list[i_prg], i_prg);
  }

  // Assert that all extracted weights match the original matrices.
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    ASSERT_EQ(precoding_matrix_list[i_prg], config.get_prg_coefficients(i_prg))
        << "Extracted and original matrices don't match";
  }
}

TEST_F(PrecodingConfigFixture, Resize)
{
  constexpr unsigned nof_layers       = 4;
  constexpr unsigned nof_ports        = 4;
  constexpr unsigned prg_size         = 5;
  constexpr unsigned original_nof_prg = 17;
  constexpr unsigned shrinked_nof_prg = 7;
  constexpr unsigned grown_nof_prg    = 23;

  std::array<precoding_weight_matrix, original_nof_prg> precoding_matrix_list;

  // Use parametrized constructor.
  precoding_configuration config(nof_layers, nof_ports, original_nof_prg, prg_size);

  // Set PRG matrices on the configuration.
  for (unsigned i_prg = 0; i_prg != original_nof_prg; ++i_prg) {
    precoding_matrix_list[i_prg] = generate_random_precoding(nof_layers, nof_ports);
    config.set_prg_coefficients(precoding_matrix_list[i_prg], i_prg);
  }

  // Shrink the number of PRG.
  config.resize(shrinked_nof_prg, prg_size);

  // Assert that the remaining PRG matrices are not affected.
  for (unsigned i_prg = 0; i_prg != shrinked_nof_prg; ++i_prg) {
    ASSERT_EQ(precoding_matrix_list[i_prg], config.get_prg_coefficients(i_prg))
        << "Extracted and original matrices don't match";
  }

  // Grow the number of PRG.
  config.resize(grown_nof_prg, prg_size);
  unsigned i_prg = 0;
  for (; i_prg != shrinked_nof_prg; ++i_prg) {
    // Assert that the remaining PRG matrices are not affected.
    ASSERT_EQ(precoding_matrix_list[i_prg], config.get_prg_coefficients(i_prg))
        << "Extracted and original matrices don't match";
  }
  for (; i_prg != grown_nof_prg; ++i_prg) {
    // Assert that the newly added precoding matrices are available.
    precoding_weight_matrix precoding_matrix = generate_random_precoding(nof_layers, nof_ports);
    config.set_prg_coefficients(precoding_matrix, i_prg);
    ASSERT_EQ(precoding_matrix, config.get_prg_coefficients(i_prg)) << "Extracted and original matrices don't match";
  }
}

TEST_F(PrecodingConfigFixture, SetGetCoefficients)
{
  constexpr unsigned nof_layers = 4;
  constexpr unsigned nof_ports  = 4;
  constexpr unsigned nof_prg    = 5;
  constexpr unsigned prg_size   = 11;

  std::array<precoding_weight_matrix, nof_prg> precoding_matrix_list;

  // Use parametrized constructor.
  precoding_configuration config(nof_layers, nof_ports, nof_prg, prg_size);

  // Set specific coefficients.
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    precoding_matrix_list[i_prg] = generate_random_precoding(nof_layers, nof_ports);

    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        cf_t weight = precoding_matrix_list[i_prg].get_coefficient(i_layer, i_port);
        config.set_coefficient(weight, i_layer, i_port, i_prg);
      }
    }
  }

  // Assert that all extracted weights match the original matrices.
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        ASSERT_EQ(precoding_matrix_list[i_prg].get_coefficient(i_layer, i_port),
                  config.get_coefficient(i_layer, i_port, i_prg))
            << "Extracted and original coefficients( don't match";
      }
    }
  }
}

TEST_F(PrecodingConfigFixture, DifferentMatrixDimensionsNotAllowed)
{
  constexpr unsigned nof_layers = 3;
  constexpr unsigned nof_ports  = 3;
  constexpr unsigned nof_prg    = 5;
  constexpr unsigned prg_size   = 11;

  // Use parametrized constructor.
  precoding_configuration config(nof_layers, nof_ports, nof_prg, prg_size);

  precoding_weight_matrix bad_precoding_matrix(nof_layers + 1, nof_ports);

  // Check that the precoding configuration throws an assertion when setting a precoding matrix with invalid dimensions.
#ifdef ASSERTS_ENABLED
  ASSERT_DEATH(
      config.set_prg_coefficients(bad_precoding_matrix, 0),
      R"(The precoding matrix number of layers \(i\.e\.\, 4\) does not match the precoding configuration number of layers \(i\.e\.\, 3\)\.)");
  precoding_weight_matrix bad_precoding_matrix2(nof_layers, nof_ports + 1);
  ASSERT_DEATH(
      config.set_prg_coefficients(bad_precoding_matrix2, 0),
      R"(The precoding matrix number of ports \(i\.e\.\, 4\) does not match the precoding configuration number of ports \(i\.e\.\, 3\)\.)");
#endif
}

TEST_F(PrecodingConfigFixture, EqualityOperator)
{
  constexpr unsigned nof_layers = 3;
  constexpr unsigned nof_ports  = 3;
  constexpr unsigned nof_prg    = 5;
  constexpr unsigned prg_size   = 11;

  std::array<precoding_weight_matrix, nof_prg> precoding_matrix_list;

  precoding_configuration config(nof_layers, nof_ports, nof_prg, prg_size);
  precoding_configuration config_comp(nof_layers, nof_ports, nof_prg, prg_size);

  // Set PRG matrices on the configuration.
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    precoding_matrix_list[i_prg] = generate_random_precoding(nof_layers, nof_ports);
    config.set_prg_coefficients(precoding_matrix_list[i_prg], i_prg);
    config_comp.set_prg_coefficients(precoding_matrix_list[i_prg], i_prg);
  }

  // Both configs are equal, assert equality and inequality operators.
  ASSERT_TRUE(config == config_comp);
  ASSERT_FALSE(config != config_comp);

  // Set the PRG 0 weights equal to the PRG 1 weights in one of the configurations.
  config_comp.set_prg_coefficients(precoding_matrix_list[1], 0);

  // Configs are different, assert equality and inequality operators.
  ASSERT_TRUE(config != config_comp);
  ASSERT_FALSE(config == config_comp);

  // Check equality operators catch uneven sizes.
  precoding_configuration config2(nof_layers, nof_ports, nof_prg, prg_size);
  precoding_configuration config_comp2(config2);

  // Change number of PRG.
  config_comp2.resize(nof_prg - 1, prg_size);

  // Configs are different, assert equality and inequality operators.
  ASSERT_TRUE(config2 != config_comp2);
  ASSERT_FALSE(config2 == config_comp2);

  config_comp2 = config2;

  // Configs are equal, assert equality and inequality operators.
  ASSERT_TRUE(config2 == config_comp2);
  ASSERT_FALSE(config2 != config_comp2);

  // Change PRG size.
  config_comp2.resize(nof_prg, prg_size - 1);

  // Configs are different, assert equality and inequality operators.
  ASSERT_TRUE(config2 != config_comp2);
  ASSERT_FALSE(config2 == config_comp2);
}
