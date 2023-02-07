/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/support/pdsch/cqi_to_mcs_mapper.h"
#include <gtest/gtest.h>

using namespace srsgnb;

const unsigned CQI_TABLE_SIZE = 16;

// Duplicate of the SNR vs MCS table in \ref lib/scheduler/support/mcs_calculator.cpp, for test purposes.
static const int ra_nr_cqi_to_mcs_table[3][CQI_TABLE_SIZE] = {
    /* ROW 1 - CQI Table 1 */
    /* MCS Table 1 */ {-1, 0, 0, 2, 4, 6, 8, 11, 13, 15, 18, 20, 22, 24, 26, 28},
    /* ROW 2 - CQI Table 2 */
    /* MCS Table 2 */ {-1, 0, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27},
    /* ROW 3 - CQI Table 3 */
    /* MCS Table 3 */ {-1, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28}};

TEST(test_manual_values, test)
{
  ASSERT_EQ(0, map_cqi_to_mcs(1, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(0, map_cqi_to_mcs(2, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(2, map_cqi_to_mcs(3, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(4, map_cqi_to_mcs(4, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(6, map_cqi_to_mcs(5, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(8, map_cqi_to_mcs(6, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(11, map_cqi_to_mcs(7, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(13, map_cqi_to_mcs(8, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(15, map_cqi_to_mcs(9, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(18, map_cqi_to_mcs(10, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(20, map_cqi_to_mcs(11, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(22, map_cqi_to_mcs(12, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(24, map_cqi_to_mcs(13, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(26, map_cqi_to_mcs(14, srsgnb::pdsch_mcs_table::qam64).value());
  ASSERT_EQ(28, map_cqi_to_mcs(15, srsgnb::pdsch_mcs_table::qam64).value());
}
