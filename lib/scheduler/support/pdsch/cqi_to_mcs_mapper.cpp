/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cqi_to_mcs_mapper.h"

using namespace srsgnb;

const unsigned CQI_TABLE_SIZE = 16;

/**
 * The table below performs the mapping of the CQI into the closest MCS, based on the corresponding spectral efficiency.
 * The mapping works as follows:
 * - select spectral efficiency from the CQI from tables Table 5.2.2.1-2, Table 5.2.2.1-3, or Table 5.2.2.1-4,
 *   TS 38.214 V15.14.0
 * - select MCS corresponding to same spectral efficiency from Table 5.1.3.1-1, Table 5.1.3.1-2, or Table 5.1.3.1-3,
 *   TS 38.214 V15.14.0
 *
 * The array ra_nr_cqi_to_mcs_table[CQI_table_idx][MCS_table_idx][CQI] contains the MCS corresponding to CQI, based on
 * the given CQI_table_idx and MCS_table_idx tables
 * CQI_table_idx: 1 -> Table 5.2.2.1-2; 2 -> Table 5.2.2.1-3, 3 -> Table 5.2.2.1-4
 * MCS_table_idx: 1 -> Table 5.1.3.1-1; 2 -> Table 5.1.3.1-2; 3 -> Table 5.1.3.1-3
 */

static const int ra_nr_cqi_to_mcs_table[3][CQI_TABLE_SIZE] = {
    /* ROW 1 - CQI Table 1 */
    /* MCS Table 1 */ {-1, 0, 0, 2, 4, 6, 8, 11, 13, 15, 18, 20, 22, 24, 26, 28},
    /* ROW 2 - CQI Table 2 */
    /* MCS Table 2 */ {-1, 0, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27},
    /* ROW 3 - CQI Table 3 */
    /* MCS Table 3 */ {-1, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28}};

optional<sch_mcs_index> srsgnb::map_cqi_to_mcs(unsigned cqi, pdsch_mcs_table mcs_table)
{
  optional<sch_mcs_index> mcs;
  if (cqi == 0 or cqi >= CQI_TABLE_SIZE) {
    return nullopt;
  }

  switch (mcs_table) {
    case pdsch_mcs_table::qam64:
      return mcs.emplace(ra_nr_cqi_to_mcs_table[0][static_cast<size_t>(cqi)]);
    case pdsch_mcs_table::qam256:
      return ra_nr_cqi_to_mcs_table[1][static_cast<size_t>(cqi)];
    case pdsch_mcs_table::qam64LowSe:
      return ra_nr_cqi_to_mcs_table[2][static_cast<size_t>(cqi)];
    default:
      break;
  }

  // NOTE: if the MCS needs to be tweaked to compensate for any UE poor performance, do it here.

  return mcs;
}
