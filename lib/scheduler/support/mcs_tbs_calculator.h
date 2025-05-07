/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/ran/sch/sch_mcs.h"
#include <optional>

namespace srsran {

struct pusch_config_params;
struct pdsch_config_params;
struct bwp_config;

/// Container for MCS and TBS results.
struct sch_mcs_tbs {
  /// MCS to use for the UE's PDSCH and PUSCH.
  sch_mcs_index mcs;
  /// TBS to be allocated on the UE's PDSCH and PUSCH.
  unsigned tbs;
};

/// \brief Computes the PDSCH MCS and TBS such that the effective code rate does not exceed 0.95.
///
/// \param[in] pdsch_params PDSCH parameters needed to compute the MCS and TBS.
/// \param[in] max_mcs Initial value to be applied for the MCS; the final MCS might be lowered if the effective
/// code rate is above 0.95.
/// \param[in] nof_prbs Maximum number of PRBs available for the PUSCH transmission.
/// \param[in] contains_dc Set to true if the transmission overlaps with the position of the DC.
/// \return The MCS and TBS, if for these values the effective code rate does not exceed 0.95; else, it returns an empty
/// optional object.
std::optional<sch_mcs_tbs>
compute_dl_mcs_tbs(const pdsch_config_params& pdsch_params, sch_mcs_index max_mcs, unsigned nof_prbs, bool contains_dc);

/// Uplink modulation code scheme calculator errors.
enum class compute_ul_mcs_tbs_error {
  /// No error.
  none = 0,
  /// The effective code rate exceeds the maximum.
  effective_code_rate_exceeds_maximum,
  /// The RE reserved for DC and HARQ-ACK feedback exceed the PUSCH capacity.
  two_bit_harq_ack_and_dc_overhead,
  /// Insufficient number of rate-matched bits for HARQ-ACK feedback.
  insufficient_nof_rm_harq_ack,
  /// Insufficient number of rate-matched bits for CSI-Part1 report.
  insufficient_nof_rm_csi1,
  /// Insufficient bandwidth for multiplexing UL-SCH and UCI.
  invalid_ulsch_information,
  /// Undefined.
  undefined
};

/// Converts the uplink modulation code scheme calculator error to a string.
inline const char* to_string(compute_ul_mcs_tbs_error error)
{
  switch (error) {
    case compute_ul_mcs_tbs_error::none:
      return "None";
    case compute_ul_mcs_tbs_error::effective_code_rate_exceeds_maximum:
      return "The effective code rate exceeds the maximum";
    case compute_ul_mcs_tbs_error::two_bit_harq_ack_and_dc_overhead:
      return "The RE reserved for DC and HARQ-ACK feedback exceed the PUSCH capacity";
    case compute_ul_mcs_tbs_error::insufficient_nof_rm_harq_ack:
      return "Insufficient number of rate-matched bits for HARQ-ACK feedback";
    case compute_ul_mcs_tbs_error::insufficient_nof_rm_csi1:
      return "Insufficient number of rate-matched bits for CSI-Part1 report";
    case compute_ul_mcs_tbs_error::invalid_ulsch_information:
      return "Insufficient bandwidth for multiplexing UL-SCH and UCI";
    case compute_ul_mcs_tbs_error::undefined:
    default:
      return "Undefined";
  }
}

/// \brief Computes the PUSCH MCS and TBS such that the effective code rate does not exceed 0.95.
///
/// \param[in] pusch_params PUSCH parameters needed to compute the MCS and TBS.
/// \param[in] active_bwp_cfg Active BWP config.
/// \param[in] max_mcs Initial value to be applied for the MCS; the final MCS might be lowered if the effective
/// code rate is above 0.95.
/// \param[in] nof_prbs Maximum number of PRBs available for the PUSCH transmission.
/// \param[in] contains_dc Set to true if the transmission overlaps with the position of the DC.
/// \return The MCS and TBS, if the function converges into a valid code rate, otherwise an error.
expected<sch_mcs_tbs, compute_ul_mcs_tbs_error> compute_ul_mcs_tbs(const pusch_config_params& pusch_params,
                                                                   const bwp_config&          active_bwp_cfg,
                                                                   sch_mcs_index              max_mcs,
                                                                   unsigned                   nof_prbs,
                                                                   bool                       contains_dc);

/// \brief Determines the PUSCH TBS such that the effective code rate does not exceed 0.95.
/// \param[in] pusch_params PUSCH parameters needed to compute the TBS.
/// \param[in] active_bwp_cfg Active BWP config.
/// \param[in] mcs Value to be applied for the MCS.
/// \param[in] nof_prbs Number of PRBs available for the PUSCH transmission.
/// \param[in] contains_dc Set to true if the transmission overlaps with the position of the DC.
/// \return TBS in bytes, in case the PUSCH code rate and paramters are valid; else, std::nullopt.
std::optional<unsigned> compute_ul_tbs(const pusch_config_params& pusch_params,
                                       const bwp_config&          active_bwp_cfg,
                                       sch_mcs_index              mcs,
                                       unsigned                   nof_prbs,
                                       bool                       contains_dc);

/// \brief Determines the PUSCH TBS without checking if the coderate is valid.
/// \param[in] pusch_params PUSCH parameters needed to compute the TBS.
/// \param[in] mcs Value to be applied for the MCS.
/// \param[in] nof_prbs Number of PRBs available for the PUSCH transmission.
/// \return TBS in bytes.
unsigned compute_ul_tbs_unsafe(const pusch_config_params& pusch_params, sch_mcs_index mcs, unsigned nof_prbs);

/// \brief Determines if the selected MCS, TBS and number of PRBs leads to a valid effective code rate and UCI.
bool is_pusch_effective_rate_valid(const pusch_config_params& pusch_cfg,
                                   const bwp_config&          active_bwp_cfg,
                                   sch_mcs_index              mcs,
                                   unsigned                   nof_prbs,
                                   bool                       contains_dc);

} // namespace srsran
