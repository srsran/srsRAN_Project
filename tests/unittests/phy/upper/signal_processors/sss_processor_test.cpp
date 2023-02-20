/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"
#include <random>

using namespace srsran;

static const float  repetitions = 10;
static std::mt19937 rgen(0);

void generate_sequence_gold(std::array<cf_t, 127>& sequence, unsigned NID, float scale)
{
  // Initialize M sequence x0
  std::array<uint32_t, 127 + 7> x0 = {};
  x0[6]                            = 0;
  x0[5]                            = 0;
  x0[4]                            = 0;
  x0[3]                            = 0;
  x0[2]                            = 0;
  x0[1]                            = 0;
  x0[0]                            = 1;

  // Initialize M sequence x1
  std::array<uint32_t, 127 + 7> x1 = {};
  x1[6]                            = 0;
  x1[5]                            = 0;
  x1[4]                            = 0;
  x1[3]                            = 0;
  x1[2]                            = 0;
  x1[1]                            = 0;
  x1[0]                            = 1;

  // Generate M sequence x
  for (uint32_t i = 0; i < 127; i++) {
    x0[i + 7] = (x0[i + 4] + x0[i]) % 2;
    x1[i + 7] = (x1[i + 1] + x1[i]) % 2;
  }

  // Modulate M sequence d
  for (uint32_t n = 0; n != 127; ++n) {
    unsigned m0 = 15 * (phys_cell_id::NID_1(NID) / 112) + 5 * phys_cell_id::NID_2(NID);
    unsigned m1 = phys_cell_id::NID_1(NID) % 112;
    sequence[n] = (1.0F - 2.0F * (float)x0[(n + m0) % 127]) * (1.0F - 2.0F * (float)x1[(n + m1) % 127]) * scale;
  }
}

static void test_case(sss_processor& sss, const sss_processor::config_t& sss_args)
{
  // Create resource grid.
  resource_grid_writer_spy grid(
      MAX_PORTS, sss_args.ssb_first_symbol + SSB_DURATION_NSYMB, sss_args.ssb_first_subcarrier / NRE + SSB_BW_RB);

  // Map SSS.
  sss.map(grid, sss_args);

  // Generate golden sequence.
  std::array<cf_t, 127> sequence_gold;
  generate_sequence_gold(sequence_gold, sss_args.phys_cell_id, sss_args.amplitude);

  // Generate expected resource grid entries.
  std::vector<resource_grid_writer_spy::expected_entry_t> expected_grid_entries;
  for (unsigned i = 0; i != 127; ++i) {
    resource_grid_writer_spy::expected_entry_t entry = {};
    entry.port                                       = sss_args.ports[0];
    entry.symbol                                     = sss_args.ssb_first_symbol + 2;
    entry.subcarrier                                 = sss_args.ssb_first_subcarrier + 56 + i;
    entry.value                                      = sequence_gold[i];

    expected_grid_entries.emplace_back(entry);
  }

  // Assert grid entries.
  grid.assert_entries(expected_grid_entries);
}

int main()
{
  std::shared_ptr<sss_processor_factory> sss_factory = create_sss_processor_factory_sw();
  TESTASSERT(sss_factory);

  // Create SSS processor.
  std::unique_ptr<sss_processor> sss = sss_factory->create();
  TESTASSERT(sss);

  // Random distributions
  std::uniform_int_distribution<unsigned> dist_cell_id(0, phys_cell_id::NOF_NID - 1);
  std::uniform_int_distribution<unsigned> dist_ssb_first_subcarrier(0, 270 * 12);
  std::uniform_int_distribution<unsigned> dist_ssb_first_symbol(0, 13 - SSB_DURATION_NSYMB);
  std::uniform_int_distribution<uint8_t>  dist_port(0, MAX_PORTS - 1);

  for (unsigned rep = 0; rep != repetitions; ++rep) {
    // SSS arguments
    sss_processor::config_t sss_args = {};
    sss_args.phys_cell_id            = dist_cell_id(rgen);
    sss_args.ssb_first_subcarrier    = dist_ssb_first_subcarrier(rgen);
    sss_args.ssb_first_symbol        = dist_ssb_first_symbol(rgen);
    sss_args.amplitude               = 1.0F;
    sss_args.ports                   = {dist_port(rgen)};

    test_case(*sss, sss_args);
  }

  return 0;
}