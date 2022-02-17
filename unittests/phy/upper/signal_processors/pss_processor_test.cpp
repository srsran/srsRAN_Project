/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../resource_grid_test_doubles.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/upper/signal_processors/pss_processor.h"
#include <cassert>
#include <random>

using namespace srsgnb;

static const float  repetitions      = 10;
static const float  assert_max_error = 1e-6;
static std::mt19937 rgen(0);

void generate_sequence_gold(std::array<cf_t, 127>& sequence, unsigned NID, float scale)
{
  // Initialise M sequence x
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

static void test_case(pss_processor& pss, const pss_processor::args_t& pss_args)
{
  // Create resource grid
  resource_grid_spy grid;

  // Map PSS
  pss.map(grid, pss_args);

  // Generate golden sequence
  std::array<cf_t, 127> sequence_gold;
  generate_sequence_gold(sequence_gold, pss_args.phys_cell_id, pss_args.amplitude);

  // Assert number of elements
  assert(grid.get_nof_entries() == 127);

  // Assert grid
  unsigned k_gold           = pss_args.ssb_first_subcarrier + 56;
  auto     sequence_gold_it = sequence_gold.begin();
  for (const resource_grid_spy::entry_t& e : grid.get_entries()) {
    float err = std::abs(*sequence_gold_it - e.value);
    assert(e.l == pss_args.ssb_first_symbol);
    assert(e.k == k_gold);
    assert(err < assert_max_error);
    k_gold++;
    sequence_gold_it++;
  }
}

int main()
{
  // Create PSS processor
  std::unique_ptr<pss_processor> pss = create_pss_processor();

  // Random distributions
  std::uniform_int_distribution<unsigned> dist_cell_id(0, phys_cell_id::NOF_NID - 1);
  std::uniform_int_distribution<unsigned> dist_ssb_first_subcarrier(0, 270 * 12);
  std::uniform_int_distribution<unsigned> dist_ssb_first_symbol(0, 13);

  for (unsigned rep = 0; rep != repetitions; ++rep) {
    // PSS arguments
    pss_processor::args_t pss_args = {};
    pss_args.phys_cell_id          = dist_cell_id(rgen);
    pss_args.ssb_first_subcarrier  = dist_ssb_first_subcarrier(rgen);
    pss_args.ssb_first_symbol      = dist_ssb_first_symbol(rgen);
    pss_args.amplitude             = 1.0F;

    test_case(*pss, pss_args);
  }

  return 0;
}