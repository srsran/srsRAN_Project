/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_aggregation_level_calculator.h"
#include "srsran/ran/csi_rs/csi_report_config.h"
#include "srsran/ran/pdcch/pdcch_constants.h"

using namespace srsran;

// Returns effective code rate corresponding to CQI reported by UE.
// [Implementation-defined] If the reported CQI is greater than size of the cqi_to_normalized_code_rate_table1 or
// cqi_to_normalized_code_rate_table2 or cqi_to_normalized_code_rate_table3, then code rate corresponding to last
// element in the respective array is returned.
double get_effective_code_rate(unsigned cqi, cqi_table_t cqi_table)
{
  // Mapping between CQI index and corresponding effective code rate for CQI table 1 where modulation is QPSK. See
  // TS 38.214, Table 5.2.2.1-2.
  // NOTE: Other CQI indexes are ignored since PDCCH uses only QPSK modulation.
  static const std::array<double, 7> cqi_to_normalized_code_rate_table1 = {
      // clang-format off
      /* CQI 0      1        2        3        4       5        6  */
             0,    0.08,   0.12,    0.19,     0.3,   0.44,    0.59,
      // clang-format on
  };

  // Mapping between CQI index and corresponding effective code rate for CQI table 2 where modulation is QPSK. See
  // TS 38.214, Table 5.2.2.1-3.
  // NOTE: Other CQI indexes are ignored since PDCCH uses only QPSK modulation.
  static const std::array<double, 4> cqi_to_normalized_code_rate_table2 = {
      // clang-format off
      /* CQI 0      1        2        3  */
             0,    0.08,   0.19,    0.44,
      // clang-format on
  };

  // Mapping between CQI index and corresponding effective code rate for CQI table 3 where modulation is QPSK. See
  // TS 38.214, Table 5.2.2.1-4.
  // NOTE: Other CQI indexes are ignored since PDCCH uses only QPSK modulation.
  static const std::array<double, 9> cqi_to_normalized_code_rate_table3 = {
      // clang-format off
      /* CQI 0      1        2        3        4       5        6         7        8  */
             0,   0.03,    0.05,    0.08,    0.12,   0.19,     0.3,     0.44,    0.59,
      // clang-format on
  };

  switch (cqi_table) {
    case cqi_table_t::table1:
      return cqi >= cqi_to_normalized_code_rate_table1.size()
                 ? cqi_to_normalized_code_rate_table1.at(cqi_to_normalized_code_rate_table1.size() - 1)
                 : cqi_to_normalized_code_rate_table1.at(cqi);
    case cqi_table_t::table2:
      return cqi >= cqi_to_normalized_code_rate_table2.size()
                 ? cqi_to_normalized_code_rate_table2.at(cqi_to_normalized_code_rate_table2.size() - 1)
                 : cqi_to_normalized_code_rate_table2.at(cqi);
    case cqi_table_t::table3:
      return cqi >= cqi_to_normalized_code_rate_table3.size()
                 ? cqi_to_normalized_code_rate_table3.at(cqi_to_normalized_code_rate_table3.size() - 1)
                 : cqi_to_normalized_code_rate_table3.at(cqi);
    default:
      report_fatal_error("Unsupported CQI table={}", cqi_table);
  }
}

// Computes PDCCH code rate for a given aggregation level.
double compute_pdcch_code_rate(unsigned nof_dci_bits, aggregation_level lvl)
{
  static const unsigned pdcch_crc_bits = 24U;
  return ((double)nof_dci_bits + pdcch_crc_bits) /
         (double)(pdcch_constants::NOF_REG_PER_CCE * NRE * 2 * to_nof_cces(lvl));
}

// Return whether DCI fits in given aggregation level (in turn nof. CCEs).
bool does_dci_bits_fit_in_cces(unsigned nof_dci_bits, aggregation_level lvl)
{
  static const unsigned pdcch_crc_bits = 24U;
  // Maximum number of PDCCH data bits per aggregation level. The aggregation level for the array element with index "x"
  // is L=1U << x.
  static const std::array<unsigned, 5> max_nof_informational_bits_per_aggr_lvl = {
      pdcch_constants::NOF_BITS_PER_CCE,
      pdcch_constants::NOF_BITS_PER_CCE * to_nof_cces(static_cast<aggregation_level>(2)),
      pdcch_constants::NOF_BITS_PER_CCE * to_nof_cces(static_cast<aggregation_level>(4)),
      pdcch_constants::NOF_BITS_PER_CCE * to_nof_cces(static_cast<aggregation_level>(8)),
      pdcch_constants::NOF_BITS_PER_CCE * to_nof_cces(static_cast<aggregation_level>(16))};

  return (nof_dci_bits + pdcch_crc_bits) < max_nof_informational_bits_per_aggr_lvl.at(to_aggregation_level_index(lvl));
}

optional<aggregation_level> srsran::map_cqi_to_aggregation_level(unsigned          cqi,
                                                                 cqi_table_t       cqi_table,
                                                                 aggregation_level min_aggr_lvl,
                                                                 aggregation_level max_aggr_lvl,
                                                                 unsigned          nof_dci_bits)
{
  srsran_assert(min_aggr_lvl <= max_aggr_lvl,
                "Minimum aggregation level must be less than or equal to maximum aggregation level");

  for (unsigned aggr_lvl_index = static_cast<unsigned>(min_aggr_lvl);
       aggr_lvl_index < static_cast<unsigned>(max_aggr_lvl);
       ++aggr_lvl_index) {
    const aggregation_level aggr_lvl = to_aggregation_level(1U << static_cast<uint8_t>(aggr_lvl_index));
    // Check whether DCI fits in nof. CCEs of an aggregation level.
    if (not does_dci_bits_fit_in_cces(nof_dci_bits, aggr_lvl)) {
      continue;
    }
    // Check whether code rate is less than effective code rate at CQI reported by UE.
    if (compute_pdcch_code_rate(nof_dci_bits, aggr_lvl) < get_effective_code_rate(cqi, cqi_table)) {
      return aggr_lvl;
    }
  }
  return {};
}
