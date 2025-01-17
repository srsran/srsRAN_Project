/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../../../support/resource_grid_test_doubles.h"
#include "srsran/adt/to_array.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/signal_processors/prs/factories.h"
#include "srsran/phy/upper/signal_processors/prs/formatters.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>

using namespace srsran;

namespace {

// Valid PDSCH configuration used as a base for the test cases.
const prs_generator_configuration base_config = {
    .slot            = slot_point(),
    .cp              = cyclic_prefix::NORMAL,
    .n_id_prs        = 0,
    .comb_size       = prs_comb_size::twelve,
    .comb_offset     = 0,
    .duration        = prs_num_symbols::twelve,
    .start_symbol    = 0,
    .prb_start       = 0,
    .freq_alloc      = {0, 24},
    .power_offset_dB = 0.0F,
    .precoding       = precoding_configuration::make_wideband(make_identity(1)),
};

struct test_case_t {
  std::function<prs_generator_configuration()> get_config;
  std::string                                  expr;
};

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  const prs_generator_configuration& config = test_case.get_config();
  fmt::print(os, "{}", config);
  return os;
}

static const auto prs_generator_validator_test_data = to_array<test_case_t>({
    {[] {
       prs_generator_configuration config = base_config;
       config.n_id_prs                    = 4096;
       return config;
     },
     fmt::format("The sequence identifier (i.e., 4096) is out of the range [0..4095].")},
    {[] {
       prs_generator_configuration config = base_config;
       config.comb_offset                 = 13;
       return config;
     },
     fmt::format("The comb offset (i.e., 13) is out of the range [0..12).")},
    {[] {
       prs_generator_configuration config = base_config;
       config.start_symbol                = 3;
       return config;
     },
     fmt::format("Start symbol (i.e., 3) plus duration (i.e., 12) exceed the slot duration (i.e., 14).")},
    {[] {
       prs_generator_configuration config = base_config;
       config.duration                    = prs_num_symbols::two;
       return config;
     },
     fmt::format("Invalid combination of duration (i.e., 2) and comb size (i.e., 12).")},
    {[] {
       prs_generator_configuration config = base_config;
       config.freq_alloc                  = {0, 4};
       return config;
     },
     fmt::format("The bandwidth (i.e., 4 PRB) is out of the range [24..276].")},
    {[] {
       prs_generator_configuration config = base_config;
       config.freq_alloc                  = {0, 25};
       return config;
     },
     fmt::format("The bandwidth (i.e., 25 PRB) must be multiple of 4.")},
    {[] {
       prs_generator_configuration config = base_config;
       config.prb_start                   = 3300;
       return config;
     },
     fmt::format("The PRB start (i.e., 3300) is out of the range [0..2176].")},
    {[] {
       prs_generator_configuration config = base_config;
       config.precoding                   = precoding_configuration::make_wideband(make_identity(2));
       return config;
     },
     fmt::format("The number of layers (i.e., 2) must be one.")},
    {[] {
       prs_generator_configuration config = base_config;
       config.precoding                   = precoding_configuration::make_wideband(make_identity(2));
       return config;
     },
     fmt::format("The number of layers (i.e., 2) must be one.")},
    {[] {
       prs_generator_configuration config = base_config;
       config.precoding                   = precoding_configuration(1, 1, 2, 32);
       config.precoding.set_prg_coefficients(make_identity(1), 0);
       config.precoding.set_prg_coefficients(make_identity(1), 1);
       return config;
     },
     fmt::format("The number of PRG (i.e., 2) must be one.")},
});

namespace {

class prsGeneratorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static void SetUpTestSuite()
  {
    if (generator) {
      return;
    }

    std::shared_ptr<pseudo_random_generator_factory> pseudo_random_gen_factory =
        create_pseudo_random_generator_sw_factory();
    report_fatal_error_if_not(pseudo_random_gen_factory, "Failed to create factory.");

    std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
    report_fatal_error_if_not(precoding_factory, "Failed to create factory.");

    std::shared_ptr<prs_generator_factory> prs_gen_factory =
        create_prs_generator_generic_factory(pseudo_random_gen_factory, precoding_factory);
    report_fatal_error_if_not(prs_gen_factory, "Failed to create factory.");

    generator = prs_gen_factory->create();
    report_fatal_error_if_not(generator, "Failed to create generator.");

    validator = prs_gen_factory->create_validator();
    report_fatal_error_if_not(validator, "Failed to create validator.");
  }

  static std::unique_ptr<prs_generator>           generator;
  static std::unique_ptr<prs_generator_validator> validator;
};

} // namespace

std::unique_ptr<prs_generator>           prsGeneratorFixture::generator = nullptr;
std::unique_ptr<prs_generator_validator> prsGeneratorFixture::validator = nullptr;

TEST_P(prsGeneratorFixture, prsGeneratorValidator)
{
  // Use thread safe death test.
  ::testing::GTEST_FLAG(death_test_style) = "threadsafe";

  ASSERT_NE(generator, nullptr);
  ASSERT_NE(validator, nullptr);

  const test_case_t& param = GetParam();

  // Make sure the configuration is invalid.
  error_type<std::string> validator_out = validator->is_valid(param.get_config());
  ASSERT_FALSE(validator_out.has_value()) << "Validation should fail.";
  ASSERT_EQ(validator_out.error(), param.expr) << "The assertion message doesn't match the expected pattern.";
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(prsGeneratorValidator,
                         prsGeneratorFixture,
                         ::testing::ValuesIn(prs_generator_validator_test_data));

} // namespace
