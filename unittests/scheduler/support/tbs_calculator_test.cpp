/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/srsgnb_test.h"
#include "tbs_calculator_test_data.h"

using namespace srsgnb;

int main()
{
  for (const test_case_t& test_case : tbs_calculator_test_data) {
    unsigned tbs = tbs_calculator_pdsch_calculate(test_case.config);

    TESTASSERT_EQ(tbs, test_case.tbs);
  }
  return 0;
}