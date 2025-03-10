/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_aggregation_level_calculator.h"
#include "mcs_calculator.h"
#include "srsran/ran/csi_rs/csi_report_config.h"
#include "srsran/ran/pdcch/pdcch_constants.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"

using namespace srsran;

// Returns Modulation and Coding scheme corresponding to effective CQI.
// \remark Effective CQI = CQI reported by UE + OLLA offset.
static sch_mcs_description get_mcs_config(float cqi, pdsch_mcs_table mcs_table)
{
  // There are fewer CQIs than MCS values, so we perform a linear interpolation.
  const float   cqi_lb = std::floor(cqi);
  const float   cqi_ub = std::ceil(cqi);
  const float   coeff  = cqi - cqi_lb;
  const float   mcs_lb = static_cast<float>(map_cqi_to_mcs(static_cast<unsigned>(cqi_lb), mcs_table).value().value());
  const float   mcs_ub = static_cast<float>(map_cqi_to_mcs(static_cast<unsigned>(cqi_ub), mcs_table).value().value());
  sch_mcs_index mcs{static_cast<uint8_t>(std::floor(mcs_lb * (1 - coeff) + mcs_ub * coeff))};

  return pdsch_mcs_get_config(mcs_table, mcs.value());
}

// Computes PDCCH code rate for a given aggregation level.
static float compute_pdcch_code_rate(unsigned nof_dci_bits, aggregation_level lvl)
{
  static constexpr unsigned pdcch_crc_bits = 24U;
  return static_cast<float>(nof_dci_bits + pdcch_crc_bits) /
         static_cast<float>(pdcch_constants::NOF_BITS_PER_CCE * to_nof_cces(lvl));
}

// Return whether DCI fits in given aggregation level (in turn nof. CCEs).
static bool does_dci_bits_fit_in_cces(unsigned nof_dci_bits, aggregation_level lvl)
{
  static const unsigned pdcch_crc_bits = 24U;
  return (nof_dci_bits + pdcch_crc_bits) < (pdcch_constants::NOF_BITS_PER_CCE * to_nof_cces(lvl));
}

// Fetch MCS table to use corresponding to the CQI table provided as input.
static pdsch_mcs_table fetch_mcs_table(cqi_table_t cqi_table)
{
  switch (cqi_table) {
    case cqi_table_t::table1:
      return pdsch_mcs_table::qam64;
    case cqi_table_t::table2:
      return pdsch_mcs_table::qam256;
    case cqi_table_t::table3:
      return pdsch_mcs_table::qam64LowSe;
    default:
      report_fatal_error("Unsupported CQI table={}", fmt::underlying(cqi_table));
  }
}

aggregation_level srsran::map_cqi_to_aggregation_level(float               cqi,
                                                       cqi_table_t         cqi_table,
                                                       span<const uint8_t> pdcch_candidates,
                                                       unsigned            nof_dci_bits)
{
  if (cqi > 0.0F) {
    const pdsch_mcs_table     mcs_table = fetch_mcs_table(cqi_table);
    const sch_mcs_description mcs_cfg   = get_mcs_config(cqi, mcs_table);
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
      // There is no point in comparing code rate for higher modulation schemes.
      if (mcs_cfg.modulation > modulation_scheme::QPSK) {
        return aggr_lvl;
      }
      // Check whether code rate is not exceeding the target code rate at effective CQI.
      if (compute_pdcch_code_rate(nof_dci_bits, aggr_lvl) > mcs_cfg.get_normalised_target_code_rate()) {
        continue;
      }
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
