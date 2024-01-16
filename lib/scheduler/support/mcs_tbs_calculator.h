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

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/ran/sch_mcs.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

struct pusch_config_params;
struct pdsch_config_params;
class ue_cell_configuration;

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
/// \param[in] ue_cell_cfg UE cell configuration.
/// \param[in] max_mcs Initial value to be applied for the MCS; the final MCS might be lowered if the effective
/// code rate is above 0.95.
/// \param[in] nof_prbs Maximum number of PRBs available for the PUSCH transmission.
/// \return The MCS and TBS, if for these values the effective code rate does not exceed 0.95; else, it returns an empty
/// optional object.
optional<sch_mcs_tbs> compute_dl_mcs_tbs(const pdsch_config_params&   pdsch_params,
                                         const ue_cell_configuration& ue_cell_cfg,
                                         sch_mcs_index                max_mcs,
                                         unsigned                     nof_prbs);

/// \brief Computes the PUSCH MCS and TBS such that the effective code rate does not exceed 0.95.
///
/// \param[in] pusch_params PUSCH parameters needed to compute the MCS and TBS.
/// \param[in] ue_cell_cfg UE cell configuration.
/// \param[in] max_mcs Initial value to be applied for the MCS; the final MCS might be lowered if the effective
/// code rate is above 0.95.
/// \param[in] nof_prbs Maximum number of PRBs available for the PUSCH transmission.
/// \return The MCS and TBS, if for these values the effective code rate does not exceed 0.95; else, it returns an empty
/// optional object.
optional<sch_mcs_tbs> compute_ul_mcs_tbs(const pusch_config_params&   pusch_params,
                                         const ue_cell_configuration& ue_cell_cfg,
                                         sch_mcs_index                max_mcs,
                                         unsigned                     nof_prbs);

} // namespace srsran
