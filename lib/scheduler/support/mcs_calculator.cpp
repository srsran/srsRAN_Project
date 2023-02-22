/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "mcs_calculator.h"

using namespace srsran;

const unsigned CQI_TABLE_SIZE = 16;

// The table below performs the mapping of the CQI into the closest MCS, based on the corresponding spectral efficiency.
// The mapping works as follows:
// - With respect to TS 38.214 V15.14.0,
//   CQI_table_idx:
//                1 -> Table 5.2.2.1-2.
//                2 -> Table 5.2.2.1-3.
//                3 -> Table 5.2.2.1-4.
// - With respect to TS 38.214 V15.14.0,
//   MCS_table_idx:
//                1 -> Table 5.1.3.1-1.
//                2 -> Table 5.1.3.1-2.
//                3 -> Table 5.1.3.1-3
// - it is assumed that MCS_table_idx == CQI_table_idx, which is given by \c mcs_table, as per TS 38.331.
// - select spectral efficiency from the CQI from tables Table 5.2.2.1-2, Table 5.2.2.1-3, or Table 5.2.2.1-4.
// - select MCS corresponding to same spectral efficiency from Table 5.1.3.1-1, Table 5.1.3.1-2, or Table 5.1.3.1-3..
//
// The array cqi_to_mcs_table[MCS_table_idx][CQI] provides the mapping of the CQI to the MCS corresponding, based on
// MCS_table_idx.
static const int cqi_to_mcs_table[3][CQI_TABLE_SIZE] = {
    // clang-format off
    // CQI Table 1 and MCS_table_idx 1
    {-1, 0, 0, 2, 4, 6, 8, 11, 13, 15, 18, 20, 22, 24, 26, 28},
    // CQI Table 2 and MCS_table_idx 2
    {-1, 0, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27},
    // CQI Table 3 and MCS_table_idx 3
    {-1, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28}
    // clang-format on
};

// TODO: This table is temporary. Note that we might eventually change this table for a SNR vs Spectral Efficiency
//       table.
// This table contains the minimum required SNR for a given MCS index; the n-th (n = {0, ..., size - 1}) element of the
// table is the minimum SNR for MCS index n.
static const std::array<double, 29> ul_snr_mcs_table = {
    // clang-format off
    /* MCS 0      1        2        3        4       5        6         7        8        9  */
     -4.0998, -3.0500, -2.1125, -1.8625, -0.0500,  0.98266,  1.6250,  2.5425,  3.4175,  4.3548,
    /* MCS 10    11       12       13       14      15       16        17       18       19  */
      5.3695,  5.8250,  6.6375,  7.6375,  8.5875,   9.4000, 10.1540, 10.9070,  11.1250, 12.0625,
    /* MCS 20    21       22       23       24      25       26        27       28 */
     13.0250, 13.8375, 14.7160, 15.6525, 16.3725,  17.1450,  17.9175, 18.7425, 20.191
    // clang-format on
};

optional<sch_mcs_index> srsran::map_cqi_to_mcs(unsigned cqi, pdsch_mcs_table mcs_table)
{
  optional<sch_mcs_index> mcs;
  if (cqi == 0 or cqi >= CQI_TABLE_SIZE) {
    return nullopt;
  }

  switch (mcs_table) {
    case pdsch_mcs_table::qam64:
      return mcs.emplace(cqi_to_mcs_table[0][static_cast<size_t>(cqi)]);
    case pdsch_mcs_table::qam256:
      return cqi_to_mcs_table[1][static_cast<size_t>(cqi)];
    case pdsch_mcs_table::qam64LowSe:
      return cqi_to_mcs_table[2][static_cast<size_t>(cqi)];
    default:
      break;
  }

  // NOTE: if the MCS needs to be tweaked to compensate for any low BLER, do it here.
  return mcs;
}

sch_mcs_index srsran::map_snr_to_mcs_ul(double snr)
{
  // The objective of this function is to find the maximum MCS that can be used for a given SNR. A possible approach to
  // this problem would be to get the iterator to the biggest element of the SNR vector not greater than the target SNR.
  // Instead, we do the following: (i) we rely on the built-in function std::upper_bound(), which returns an iterator to
  // the smallest element greater than the target SNR; (ii) we need to subtract 1 from the iterator returned by
  // std::upper_bound(), to obtain wanted MCS.
  const unsigned MAX_MCS = 28;
  const unsigned MIN_MCS = 0;

  // Check of the SNR is lower than the minimum, or greater than the maximum. If so, return the min or max MCS.
  if (snr <= ul_snr_mcs_table.front()) {
    return MIN_MCS;
  }
  // NOTE: The sign > is not sufficient, as, with an input snr == ul_snr_mcs_table.back(), the std::upper_bound()
  // below would return ul_snr_mcs_table.end(). To prevent this, we need the sign >= in the comparison.
  if (snr >= ul_snr_mcs_table.back()) {
    return MAX_MCS;
  }

  auto it_ub = std::upper_bound(ul_snr_mcs_table.begin(), ul_snr_mcs_table.end(), snr);

  // NOTE: By design, it_ub > ul_snr_mcs_table.begin(). All SNR values such it_ub == ul_snr_mcs_table.begin() are
  // handled above, in the "if (snr <= ul_snr_mcs_table.front())" statement above.
  return it_ub - ul_snr_mcs_table.begin() - 1;
}
