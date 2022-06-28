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

// Helper struct to pass parameters to the test.
struct prbs_calculator_test_paramters {
  unsigned payload_size_bytes;
  unsigned nof_symb_sh;
  unsigned nof_dmrs_prb;
  unsigned nof_oh_prb;
  unsigned mod_order;
  float    target_code_rate;
  unsigned nof_layers;
  unsigned tb_scaling_field;
};

// Pair of prbs_calculator_test_entry and corresponding number of prbs.
struct prbs_calculator_test_entry {
  prbs_calculator_test_paramters params;
  unsigned                       nof_prbs;
};

// NOTE: payload size is given in bytes.
static const std::vector<prbs_calculator_test_entry> prbs_calculator_test_table = {
    // clang-format off
  {{39, 12, 36, 0, 2, 0.37012, 1, 0}, 4U},
  {{41, 12, 36, 0, 2, 0.37012, 1, 0}, 5U},
  {{53, 12, 36, 0, 2, 0.37012, 1, 0}, 6U},
  {{53, 12, 36, 0, 4, 0.60156, 1, 0}, 2U},
  {{54, 12, 36, 0, 4, 0.60156, 1, 0}, 2U},
  {{75, 12, 36, 0, 4, 0.60156, 1, 0}, 3U},
  {{102, 12, 36, 0, 4, 0.60156, 1, 0}, 4U},
  {{133, 12, 36, 0, 6, 0.70215, 1, 0}, 3U},
  {{172, 12, 36, 0, 6, 0.70215, 1, 0}, 3U},
  {{209, 12, 36, 0, 6, 0.70215, 1, 0}, 4U},
  {{241, 12, 36, 0, 6, 0.70215, 1, 0}, 5U},
  {{275, 12, 36, 0, 6, 0.70215, 1, 0}, 5U},
  {{285, 12, 36, 0, 6, 0.70215, 1, 0}, 5U},
  {{303, 12, 36, 0, 6, 0.70215, 1, 0}, 6U},
  // The parameters below correspond to the max SIB1 payload size (2976 bits, or 372 bytes).
  {{372, 12, 36, 0, 2, 0.37012, 1, 0}, 37U},
  // This entry is to test the max payload size (3824 bits, or 478 bytes).
  {{478, 12, 36, 0, 2, 0.37012, 1, 0}, 48U}
    // clang-format on
};

int main()
{
  // Run the test for all the values in the table.
  for (const auto& test_entry : prbs_calculator_test_table) {
    auto& test_params = test_entry.params;
    TESTASSERT_EQ(test_entry.nof_prbs,
                  get_nof_prbs(test_params.payload_size_bytes,
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
