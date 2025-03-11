/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../support/resource_grid_test_doubles.h"
#include "prs_generator_test_data.h"
#include "srsran/phy/support/precoding_formatters.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/signal_processors/prs/factories.h"
#include "srsran/phy/upper/signal_processors/prs/formatters.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.config);
  return os;
}

} // namespace srsran

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

std::unique_ptr<prs_generator>           prsGeneratorFixture::generator = nullptr;
std::unique_ptr<prs_generator_validator> prsGeneratorFixture::validator = nullptr;

} // namespace

TEST_P(prsGeneratorFixture, FromTestVector)
{
  // Prepare resource grid and resource grid mapper spies.
  resource_grid_writer_spy grid(precoding_constants::MAX_NOF_PORTS, MAX_NSYMB_PER_SLOT, MAX_NOF_PRBS);

  const test_case_t&                 test_case = GetParam();
  const prs_generator_configuration& config    = test_case.config;

  // Make sure the configuration is valid.
  ASSERT_TRUE(validator->is_valid(config));

  // Generate signal.
  generator->generate(grid, config);

  // Load output golden data.
  const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

  // Assert resource grid entries.
  grid.assert_entries(testvector_symbols, std::sqrt(config.precoding.get_nof_ports()));
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(prsPdschGenerator, prsGeneratorFixture, ::testing::ValuesIn(prs_generator_test_data));
