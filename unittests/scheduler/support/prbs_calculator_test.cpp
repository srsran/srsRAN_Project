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

// Pair of prbs_calculator_test_entry and corresponding number of prbs.
struct prbs_calculator_test_entry {
  prbs_calculator_pdsch_config params;
  unsigned                     nof_prbs;
  unsigned                     tbs;
};

// NOTE: payload size is given in bytes.
static const std::vector<prbs_calculator_test_entry> prbs_calculator_test_table = {
    // clang-format off
  {{39, 12, 36, 0, modulation_scheme::QPSK, 0.37012, 1, 0}, 4U, 40U},
  {{41, 12, 36, 0, modulation_scheme::QPSK, 0.37012, 1, 0}, 5U, 51U},
  {{53, 12, 36, 0, modulation_scheme::QPSK, 0.37012, 1, 0}, 6U, 60U},
  {{53, 12, 36, 0, modulation_scheme::QAM16, 0.60156, 1, 0}, 2U, 66U},
  {{54, 12, 36, 0, modulation_scheme::QAM16, 0.60156, 1, 0}, 2U, 66U},
  {{75, 12, 36, 0, modulation_scheme::QAM16, 0.60156, 1, 0}, 3U, 101},
  {{102, 12, 36, 0, modulation_scheme::QAM16, 0.60156, 1, 0}, 4U, 129U},
  {{133, 12, 36, 0, modulation_scheme::QAM64, 0.70215, 1, 0}, 3U, 177U},
  {{172, 12, 36, 0, modulation_scheme::QAM64, 0.70215, 1, 0}, 3U, 177U},
  {{209, 12, 36, 0, modulation_scheme::QAM64, 0.70215, 1, 0}, 4U, 233U},
  {{241, 12, 36, 0, modulation_scheme::QAM64, 0.70215, 1, 0}, 5U, 285U},
  {{275, 12, 36, 0, modulation_scheme::QAM64, 0.70215, 1, 0}, 5U, 285U},
  {{285, 12, 36, 0, modulation_scheme::QAM64, 0.70215, 1, 0}, 5U, 285U},
  {{303, 12, 36, 0, modulation_scheme::QAM64, 0.70215, 1, 0}, 6U, 341U},
  // The parameters below correspond to the max SIB1 payload size (2976 bits, or 372 bytes).
  {{372, 12, 36, 0, modulation_scheme::QPSK, 0.37012, 1, 0}, 37U, 372U},
  // This entry is to test the max payload size (3824 bits, or 478 bytes).
  // NOTE: This is an edge-case. The TBS corresponding to this combination of parameters would exceed the max value
  // (3824 bits, or 478 bytes); therefore the function returns the max num. of PRBs such that the corresponding TBS won't exceed the max value of
  // 478B.
  {{478, 12, 36, 0, modulation_scheme::QPSK, 0.37012, 1, 0}, 47U, 469U}
    // clang-format on
};

int main()
{
  // Run the test for all the values in the table.
  for (const auto& test_entry : prbs_calculator_test_table) {
    auto&          test_params  = test_entry.params;
    pdsch_prbs_tbs test_results = get_nof_prbs(test_params);
    TESTASSERT_EQ(test_entry.nof_prbs, test_results.nof_prbs);
    TESTASSERT_EQ(test_entry.tbs, test_results.tbs_bytes);
  }

  return 0;
}
