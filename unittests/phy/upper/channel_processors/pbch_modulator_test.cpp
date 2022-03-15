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

int main()
{
  std::unique_ptr<pbch_modulator> modulator = create_pbch_modulator();

  for (const test_case_t& test_case : pbch_modulator_test_data) {
    resource_grid_writer_spy grid;

    modulator->put(test_case.data, grid, test_case.args);

    // Assert resource grid entries.
    grid.assert_put_entries(test_case.symbols);
  }

  return 0;
}
