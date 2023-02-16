/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/ssb_mapping.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void get_ssb_crbs()
{
  // Test case with subcarrier_spacing::kHz15 with k_SSB == 0.
  crb_interval expected_crbs = crb_interval{0, 20};
  TESTASSERT(expected_crbs == get_ssb_crbs(subcarrier_spacing::kHz15 /*ssb_scs*/,
                                           subcarrier_spacing::kHz15 /*scs_common*/,
                                           ssb_offset_to_pointA{0},
                                           ssb_subcarrier_offset{0}));

  // Test case with subcarrier_spacing::kHz15 with k_SSB > 0.
  expected_crbs = crb_interval{0, 21};
  TESTASSERT(expected_crbs == get_ssb_crbs(subcarrier_spacing::kHz15 /*ssb_scs*/,
                                           subcarrier_spacing::kHz15 /*scs_common*/,
                                           ssb_offset_to_pointA{0},
                                           ssb_subcarrier_offset{1}));

  expected_crbs = crb_interval{25, 45};
  TESTASSERT(expected_crbs == get_ssb_crbs(subcarrier_spacing::kHz15 /*ssb_scs*/,
                                           subcarrier_spacing::kHz15 /*scs_common*/,
                                           ssb_offset_to_pointA{25},
                                           ssb_subcarrier_offset{0}));

  expected_crbs = crb_interval{25, 46};
  TESTASSERT(expected_crbs == get_ssb_crbs(subcarrier_spacing::kHz15 /*ssb_scs*/,
                                           subcarrier_spacing::kHz15 /*scs_common*/,
                                           ssb_offset_to_pointA{25},
                                           ssb_subcarrier_offset{5}));

  // Test case with subcarrier_spacing::kHz30 with k_SSB == 0.
  expected_crbs = crb_interval{0, 20};
  TESTASSERT(expected_crbs == get_ssb_crbs(subcarrier_spacing::kHz30 /*ssb_scs*/,
                                           subcarrier_spacing::kHz30 /*scs_common*/,
                                           ssb_offset_to_pointA{0},
                                           ssb_subcarrier_offset{0}));

  // Test case with subcarrier_spacing::kHz30 with k_SSB > 0.
  expected_crbs = crb_interval{0, 21};
  TESTASSERT(expected_crbs == get_ssb_crbs(subcarrier_spacing::kHz30 /*ssb_scs*/,
                                           subcarrier_spacing::kHz30 /*scs_common*/,
                                           ssb_offset_to_pointA{0},
                                           ssb_subcarrier_offset{2}));

  expected_crbs = crb_interval{6, 26};
  TESTASSERT(expected_crbs == get_ssb_crbs(subcarrier_spacing::kHz30 /*ssb_scs*/,
                                           subcarrier_spacing::kHz30 /*scs_common*/,
                                           ssb_offset_to_pointA{12},
                                           ssb_subcarrier_offset{0}));

  expected_crbs = crb_interval{6, 27};
  TESTASSERT(expected_crbs == get_ssb_crbs(subcarrier_spacing::kHz30 /*ssb_scs*/,
                                           subcarrier_spacing::kHz30 /*scs_common*/,
                                           ssb_offset_to_pointA{12},
                                           ssb_subcarrier_offset{2}));
}

int main()
{
  srslog::init();

  get_ssb_crbs();

  return 0;
}
