/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/support/prbs_calculator.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

// Helper struct to pass parameters to the test
struct prbs_calulcator_test_paramters {
  unsigned payload_size;
  unsigned nof_symb_sh;
  unsigned nof_dmrs_prb;
  unsigned nof_oh_prb;
  unsigned mod_order;
  float    target_code_rate;
  unsigned nof_layers;
  unsigned tb_scaling_field;
};

// Pair of prbs_calulcator_test_entry and corresponding number of prbs.
struct prbs_calulcator_test_entry {
  prbs_calulcator_test_paramters params;
  unsigned                       nof_prbs;
};

static const std::vector<prbs_calulcator_test_entry> prbs_calulcator_test_table = {
    // clang-format off
  {{312, 12, 36, 0, 2, 0.37012, 1, 0}, 5U},
  {{324, 12, 36, 0, 2, 0.37012, 1, 0}, 5U},
  {{420, 12, 36, 0, 2, 0.37012, 1, 0}, 6U},
  {{420, 12, 36, 0, 4, 0.60156, 1, 0}, 2U},
  {{430, 12, 36, 0, 4, 0.60156, 1, 0}, 2U},
  {{600, 12, 36, 0, 4, 0.60156, 1, 0}, 3U},
  {{812, 12, 36, 0, 4, 0.60156, 1, 0}, 4U},
  {{1068, 12, 36, 0, 6, 0.70215, 1, 0}, 3U},
  {{1370, 12, 36, 0, 6, 0.70215, 1, 0}, 4U},
  {{1675, 12, 36, 0, 6, 0.70215, 1, 0}, 4U},
  {{1930, 12, 36, 0, 6, 0.70215, 1, 0}, 5U},
  {{2200, 12, 36, 0, 6, 0.70215, 1, 0}, 5U},
  {{2280, 12, 36, 0, 6, 0.70215, 1, 0}, 6U},
  {{2420, 12, 36, 0, 6, 0.70215, 1, 0}, 6U},
  // The parameters below correspond to the max SIB1 payload size.
  {{2976, 12, 36, 0, 2, 0.37012, 1, 0}, 38U},
  // This entry is to test the max payload size (3824).
  {{3824, 12, 36, 0, 2, 0.37012, 1, 0}, 48U}
    // clang-format on
};

int main()
{
  // Run the test for all the values in the table.
  for (const auto& test_entry : prbs_calulcator_test_table) {
    auto& test_params = test_entry.params;
    TESTASSERT_EQ(test_entry.nof_prbs,
                  get_nof_prbs(test_params.payload_size,
                               test_params.nof_symb_sh,
                               test_params.nof_dmrs_prb,
                               test_params.nof_oh_prb,
                               test_params.mod_order,
                               test_params.target_code_rate,
                               test_params.nof_layers,
                               test_params.tb_scaling_field));
  }

  return 0;
}
