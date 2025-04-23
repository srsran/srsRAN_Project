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
#include "ptrs_pdsch_generator_test_data.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "srsran/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator_factory.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os,
             "slot={} rnti={} dmrs={} k_rb={} id={} n_scid={} amplitude={} dmrs={} time_allocation={} freq_density={} "
             "time_density={} re_offset={} nof_layers={} nof_ports={}",
             test_case.config.slot,
             to_value(test_case.config.rnti),
             test_case.config.dmrs_config_type.to_string(),
             test_case.config.reference_point_k_rb,
             test_case.config.scrambling_id,
             test_case.config.n_scid,
             test_case.config.amplitude,
             test_case.config.dmrs_symbols_mask,
             test_case.config.time_allocation,
             to_string(test_case.config.freq_density),
             to_string(test_case.config.time_density),
             to_string(test_case.config.re_offset),
             test_case.config.precoding.get_nof_layers(),
             test_case.config.precoding.get_nof_ports());
  return os;
}

} // namespace srsran

namespace {

class PtrsPdschGeneratorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static void SetUpTestSuite()
  {
    if (ptrs_pdsch_gen) {
      return;
    }

    std::shared_ptr<pseudo_random_generator_factory> pseudo_random_gen_factory =
        create_pseudo_random_generator_sw_factory();
    report_fatal_error_if_not(pseudo_random_gen_factory, "Failed to create factory.");

    std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
    report_fatal_error_if_not(precoding_factory, "Failed to create factory.");

    std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory_ =
        create_resource_grid_mapper_factory(precoding_factory);
    report_fatal_error_if_not(rg_mapper_factory_, "Failed to create factory.");

    rg_mapper_factory = rg_mapper_factory_;

    std::shared_ptr<ptrs_pdsch_generator_factory> ptrs_pdsch_gen_factory =
        create_ptrs_pdsch_generator_generic_factory(pseudo_random_gen_factory, rg_mapper_factory_);
    report_fatal_error_if_not(ptrs_pdsch_gen_factory, "Failed to create factory.");

    ptrs_pdsch_gen = ptrs_pdsch_gen_factory->create();
    report_fatal_error_if_not(ptrs_pdsch_gen, "Failed to create factory.");
  }

  static std::unique_ptr<ptrs_pdsch_generator>         ptrs_pdsch_gen;
  static std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory;
};

std::unique_ptr<ptrs_pdsch_generator>         PtrsPdschGeneratorFixture::ptrs_pdsch_gen    = nullptr;
std::shared_ptr<resource_grid_mapper_factory> PtrsPdschGeneratorFixture::rg_mapper_factory = nullptr;

} // namespace

TEST_P(PtrsPdschGeneratorFixture, FromTestVector)
{
  // Prepare resource grid and resource grid mapper spies.
  resource_grid_writer_spy grid(precoding_constants::MAX_NOF_PORTS, MAX_NSYMB_PER_SLOT, MAX_NOF_PRBS);

  const test_case_t&                         test_case = GetParam();
  const ptrs_pdsch_generator::configuration& config    = test_case.config;

  // Generate signal.
  ptrs_pdsch_gen->generate(grid, config);

  // Load output golden data.
  const std::vector<resource_grid_writer_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

  // Assert resource grid entries.
  grid.assert_entries(testvector_symbols, std::sqrt(config.precoding.get_nof_ports()));
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PtrsPdschGenerator,
                         PtrsPdschGeneratorFixture,
                         ::testing::ValuesIn(ptrs_pdsch_generator_test_data));
