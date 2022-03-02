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

#include "pbch_modulator_test_data.h"
#include "srsgnb/phy/upper/channel_processors/pbch_modulator.h"

using namespace srsgnb;

static constexpr float assert_max_error = 1e-6;

int main()
{
  std::unique_ptr<pbch_modulator> modulator = create_pbch_modulator();

  for (const test_case_t& test_case : pbch_modulator_test_data) {
    resource_grid_spy grid;

    modulator->put(test_case.data, grid, test_case.args);

    // Make sure the
    assert(test_case.symbols.size() == grid.get_nof_put_entries());

    // Assert encoded data
    std::vector<resource_grid_spy::entry_t> symbols = grid.get_put_entries();
    for (unsigned i = 0; i != pbch_modulator::M_symb; ++i) {
      float err = std::abs(test_case.symbols[i].value - symbols[i].value);
      assert(test_case.symbols[i].port == symbols[i].port);
      assert(test_case.symbols[i].l == symbols[i].l);
      assert(test_case.symbols[i].k == symbols[i].k);
      assert(err < assert_max_error);
    }
  }
  return 0;
}