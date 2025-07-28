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

#include "pdsch_modulator_test_data.h"
#include "srsran/phy/support/precoding_formatters.h"
#include "srsran/phy/support/re_pattern_formatters.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/pdsch/factories.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/srsvec/bit.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(
      os,
      "rnti={} bwp={} mod1={} mod2={} freq={} time={} dmrs={}/{} ncgwd={} n_id={} scaling={:.1f} rvd={} precoding={}",
      to_rnti(test_case.config.rnti),
      test_case.config.bwp,
      to_string(test_case.config.modulation1),
      to_string(test_case.config.modulation2),
      test_case.config.freq_allocation,
      test_case.config.time_alloc,
      test_case.config.dmrs_symb_pos,
      test_case.config.dmrs_config_type.to_string(),
      test_case.config.nof_cdm_groups_without_data,
      test_case.config.n_id,
      test_case.config.scaling,
      test_case.config.reserved.get_re_patterns(),
      test_case.config.precoding);
  return os;
}

} // namespace srsran

namespace {

class PdschModulatorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static void SetUpTestSuite()
  {
    std::shared_ptr<modulation_mapper_factory> modulator_factory = create_modulation_mapper_factory();
    report_fatal_error_if_not(modulator_factory, "Failed to create modulation mapper factory.");

    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    report_fatal_error_if_not(prg_factory, "Failed to create pseudo-random sequence generator factory.");

    std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
    report_fatal_error_if_not(precoding_factory, "Failed to create precoder factory.");

    std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory =
        create_resource_grid_mapper_factory(precoding_factory);
    report_fatal_error_if_not(rg_mapper_factory, "Failed to create RG mapper factory.");

    std::shared_ptr<pdsch_modulator_factory> pdsch_factory =
        create_pdsch_modulator_factory_sw(modulator_factory, prg_factory, rg_mapper_factory);
    report_fatal_error_if_not(rg_mapper_factory, "Failed to create PDSCH modulator factory.");

    pdsch = pdsch_factory->create();
    report_fatal_error_if_not(pdsch, "Failed to create PDSCH modulator.");
  }

  static std::unique_ptr<pdsch_modulator> pdsch;
};

std::unique_ptr<pdsch_modulator> PdschModulatorFixture::pdsch = nullptr;

TEST_P(PdschModulatorFixture, VectorTest)
{
  const test_case_t& test_case = GetParam();
  unsigned           max_prb   = test_case.config.bwp.stop();
  unsigned           max_symb  = get_nsymb_per_slot(cyclic_prefix::NORMAL);
  unsigned           max_ports = test_case.config.precoding.get_nof_ports();

  // Prepare resource grid spy.
  resource_grid_writer_spy grid(max_ports, max_symb, max_prb);

  // Read codeword.
  std::vector<uint8_t> data = test_case.data.read();

  // Pack data.
  dynamic_bit_buffer packed_data(data.size());
  srsvec::bit_pack(packed_data, data);

  // Prepare codewords.
  static_vector<bit_buffer, pdsch_constants::MAX_NOF_CODEWORDS> codewords;
  codewords.emplace_back(packed_data);

  // Modulate.
  pdsch->modulate(grid, codewords, test_case.config);

  // Read resource grid data.
  std::vector<resource_grid_writer_spy::expected_entry_t> rg_entries = test_case.symbols.read();

  // Assert resource grid entries.
  grid.assert_entries(rg_entries, std::sqrt(max_ports));
}

INSTANTIATE_TEST_SUITE_P(PdschProcessorVectortest,
                         PdschModulatorFixture,
                         ::testing::ValuesIn(pdsch_modulator_test_data));

} // namespace
