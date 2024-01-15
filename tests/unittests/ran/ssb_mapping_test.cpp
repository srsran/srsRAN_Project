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

#include "srsran/ran/ssb_mapping.h"
#include "srsran/support/test_utils.h"

using namespace srsran;

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
