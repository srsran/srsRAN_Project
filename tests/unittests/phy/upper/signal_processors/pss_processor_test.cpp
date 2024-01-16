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

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"
#include <random>

using namespace srsran;

static const float  repetitions = 10;
static std::mt19937 rgen(0);

void generate_sequence_gold(std::array<cf_t, 127>& sequence, unsigned NID, float scale)
{
  // Initialize M sequence x
  std::array<uint32_t, 127 + 7> x = {};

  x[6] = 1;
  x[5] = 1;
  x[4] = 1;
  x[3] = 0;
  x[2] = 1;
  x[1] = 1;
  x[0] = 0;

  // Generate M sequence x
  for (uint32_t i = 0; i != 127; ++i) {
    x[i + 7] = (x[i + 4] + x[i]) % 2;
  }

  // Modulate M sequence d
  for (uint32_t n = 0; n != 127; ++n) {
    unsigned m  = (n + 43 * phys_cell_id::NID_2(NID)) % 127;
    sequence[n] = (1.0F - 2.0F * (float)x[m]) * scale;
  }
}

static void test_case(pss_processor& pss, const pss_processor::config_t& pss_args)
{
  // Create resource grid.
  resource_grid_writer_spy grid(
      MAX_PORTS, pss_args.ssb_first_symbol + SSB_DURATION_NSYMB, pss_args.ssb_first_subcarrier / NRE + SSB_BW_RB);

  // Map PSS.
  pss.map(grid, pss_args);

  // Generate golden sequence.
  std::array<cf_t, 127> sequence_gold;
  generate_sequence_gold(sequence_gold, pss_args.phys_cell_id, pss_args.amplitude);

  // Generate expected resource grid entries.
  std::vector<resource_grid_writer_spy::expected_entry_t> expected_grid_entries;
  for (unsigned i = 0; i != 127; ++i) {
    resource_grid_writer_spy::expected_entry_t entry = {};
    entry.port                                       = pss_args.ports[0];
    entry.symbol                                     = pss_args.ssb_first_symbol + 0;
    entry.subcarrier                                 = pss_args.ssb_first_subcarrier + 56 + i;
    entry.value                                      = sequence_gold[i];

    expected_grid_entries.emplace_back(entry);
  }

  // Assert grid entries.
  grid.assert_entries(expected_grid_entries);
}

int main()
{
  std::shared_ptr<pss_processor_factory> pss_factory = create_pss_processor_factory_sw();
  TESTASSERT(pss_factory);

  // Create PSS processor.
  std::unique_ptr<pss_processor> pss = pss_factory->create();
  TESTASSERT(pss);

  // Random distributions.
  std::uniform_int_distribution<unsigned> dist_cell_id(0, phys_cell_id::NOF_NID - 1);
  std::uniform_int_distribution<unsigned> dist_ssb_first_subcarrier(0, 270 * 12);
  std::uniform_int_distribution<unsigned> dist_ssb_first_symbol(0, 13);
  std::uniform_int_distribution<uint8_t>  dist_port(0, MAX_PORTS - 1);

  for (unsigned rep = 0; rep != repetitions; ++rep) {
    // PSS arguments.
    pss_processor::config_t pss_args = {};
    pss_args.phys_cell_id            = dist_cell_id(rgen);
    pss_args.ssb_first_subcarrier    = dist_ssb_first_subcarrier(rgen);
    pss_args.ssb_first_symbol        = dist_ssb_first_symbol(rgen);
    pss_args.amplitude               = 1.0F;
    pss_args.ports                   = {dist_port(rgen)};

    test_case(*pss, pss_args);
  }

  return 0;
}