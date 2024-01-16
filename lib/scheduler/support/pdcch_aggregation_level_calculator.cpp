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

#include "pdcch_aggregation_level_calculator.h"
#include "mcs_calculator.h"
#include "srsran/ran/csi_rs/csi_report_config.h"
#include "srsran/ran/pdcch/pdcch_constants.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"

using namespace srsran;

// Returns target code rate corresponding to CQI reported by UE.
// [Implementation-defined] If the reported CQI is greater than size of the mcs_index_to_normalized_code_rate_table1 or
// mcs_index_to_normalized_code_rate_table2 or mcs_index_to_normalized_code_rate_table3, then target code rate
// corresponding to last element in the respective array is returned.
static double get_target_code_rate(cqi_value cqi, cqi_table_t cqi_table)
{
  // Mapping between MCS index and corresponding target code rate for MCS table 1 where modulation is QPSK. See
  // TS 38.214, Table 5.1.3.1-1.
  // NOTE: Other MCS indexes are ignored since PDCCH uses only QPSK modulation.
  static const std::array<double, 10> mcs_index_to_normalized_code_rate_table1 = {
      // clang-format off
      /* MCS 0       1        2        3        4       5        6        7       8   */
           0.117,  0.153,   0.188,   0.245,   0.301,  0.370,   0.438,   0.514,  0.588,
      /* MCS 9   */
           0.663,
      // clang-format on
  };

  // Mapping between MCS index and corresponding target code rate for MCS table 2 where modulation is QPSK. See
  // TS 38.214, Table 5.1.3.1-2.
  // NOTE: Other MCS indexes are ignored since PDCCH uses only QPSK modulation.
  static const std::array<double, 5> mcs_index_to_normalized_code_rate_table2 = {
      // clang-format off
      /* MCS 0       1        2        3        4    */
           0.117,  0.188,   0.301,   0.438,   0.588,
      // clang-format on
  };

  // Mapping between MCS index and corresponding target code rate for MCS table 3 where modulation is QPSK. See
  // TS 38.214, Table 5.1.3.1-3.
  // NOTE: Other MCS indexes are ignored since PDCCH uses only QPSK modulation.
  static const std::array<double, 15> mcs_index_to_normalized_code_rate_table3 = {
      // clang-format off
      /* MCS 0       1        2        3        4       5        6        7       8   */
           0.029,  0.039,   0.049,   0.063,   0.076,  0.097,   0.117,   0.153,  0.188,
      /* MCS 9       10       11       12       13      14  */
           0.245,  0.301,   0.370,   0.438,   0.514,  0.588,
      // clang-format on
  };

  sch_mcs_index mcs_idx = 0;
  switch (cqi_table) {
    case cqi_table_t::table1: {
      const auto computed_mcs = map_cqi_to_mcs(cqi.to_uint(), srsran::pdsch_mcs_table::qam64);
      mcs_idx                 = computed_mcs.has_value() ? computed_mcs.value() : 0;
      return mcs_index_to_normalized_code_rate_table1[std::min(static_cast<size_t>(mcs_idx.to_uint()),
                                                               mcs_index_to_normalized_code_rate_table1.size() - 1)];
    }
    case cqi_table_t::table2: {
      const auto computed_mcs = map_cqi_to_mcs(cqi.to_uint(), srsran::pdsch_mcs_table::qam256);
      mcs_idx                 = computed_mcs.has_value() ? computed_mcs.value() : 0;
      return mcs_index_to_normalized_code_rate_table2[std::min(static_cast<size_t>(mcs_idx.to_uint()),
                                                               mcs_index_to_normalized_code_rate_table2.size() - 1)];
    }
    case cqi_table_t::table3: {
      const auto computed_mcs = map_cqi_to_mcs(cqi.to_uint(), srsran::pdsch_mcs_table::qam64LowSe);
      mcs_idx                 = computed_mcs.has_value() ? computed_mcs.value() : 0;
      return mcs_index_to_normalized_code_rate_table3[std::min(static_cast<size_t>(mcs_idx.to_uint()),
                                                               mcs_index_to_normalized_code_rate_table3.size() - 1)];
    }
    default:
      report_fatal_error("Unsupported CQI table={}", cqi_table);
  }
}

// Computes PDCCH code rate for a given aggregation level.
static double compute_pdcch_code_rate(unsigned nof_dci_bits, aggregation_level lvl)
{
  static const unsigned pdcch_crc_bits            = 24U;
  static const unsigned nof_bits_per_symbols_qpsk = 2U;
  return ((double)nof_dci_bits + pdcch_crc_bits) /
         (double)(pdcch_constants::NOF_REG_PER_CCE * NRE * nof_bits_per_symbols_qpsk * to_nof_cces(lvl));
}

// Return whether DCI fits in given aggregation level (in turn nof. CCEs).
static bool does_dci_bits_fit_in_cces(unsigned nof_dci_bits, aggregation_level lvl)
{
  static const unsigned pdcch_crc_bits = 24U;
  return (nof_dci_bits + pdcch_crc_bits) < (pdcch_constants::NOF_BITS_PER_CCE * to_nof_cces(lvl));
}

aggregation_level srsran::map_cqi_to_aggregation_level(cqi_value           cqi,
                                                       cqi_table_t         cqi_table,
                                                       span<const uint8_t> pdcch_candidates,
                                                       unsigned            nof_dci_bits)
{
  for (unsigned aggr_lvl_index = 0; aggr_lvl_index < pdcch_candidates.size(); ++aggr_lvl_index) {
    // Check whether PDCCH candidates are configured for aggregation level.
    if (pdcch_candidates[aggr_lvl_index] == 0) {
      continue;
    }
    const aggregation_level aggr_lvl = aggregation_index_to_level(aggr_lvl_index);
    // Check whether DCI fits in nof. CCEs of an aggregation level.
    if (not does_dci_bits_fit_in_cces(nof_dci_bits, aggr_lvl)) {
      continue;
    }
    // Check whether code rate is less than code rate at CQI reported by UE.
    if (compute_pdcch_code_rate(nof_dci_bits, aggr_lvl) < get_target_code_rate(cqi, cqi_table)) {
      return aggr_lvl;
    }
  }

  // No valid aggregation level found.
  // NOTE: This could be due to low CQI.
  for (int aggr_lvl_index = static_cast<int>(pdcch_candidates.size() - 1); aggr_lvl_index >= 0; --aggr_lvl_index) {
    if (pdcch_candidates[aggr_lvl_index] > 0) {
      return aggregation_index_to_level(aggr_lvl_index);
    }
  }
  report_fatal_error("Invalid PDCCH candidates configuration");
}
