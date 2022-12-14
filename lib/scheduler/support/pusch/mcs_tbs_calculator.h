/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../ue_scheduling/ue_configuration.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/sch_mcs.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

struct pusch_config_params;

/// Container for MCS, TBS and number of PRBs results.
struct pusch_mcs_tbs_params {
  /// MCS to use for the UE's PUSCH.
  sch_mcs_index mcs;
  /// TBS to be allocated on the UE's PUSCH.
  unsigned tbs;
  /// Number of PRBs to be allocated for the UE's PUSCH.
  unsigned n_prbs;
};

/// \brief Computes the PUSCH MCS and TBS such that the effective code rate does not exceed 0.95.
///
/// \param[in] pusch_params PUSCH parameters needed to compute the MCS and TBS.
/// \param[in] ue_cell_cfg UE cell configuration.
/// \param[in] payload_size_bytes Number of UL-SCH buffer bytes available to be transmitted.
/// \param[in] starting_mcs Initial value to be applied for the MCS; the final MCS might be lowered if the effective
/// code rate is above 0.95.
/// \param[in] max_nof_prbs Maximum number of PRBs available for the PUSCH transmission.
/// \return The MCS, TBS, and number of PRBs used, if for these values the effective code rate does not exceed 0.95;
/// else, it returns an empty optional object.
optional<pusch_mcs_tbs_params> compute_ul_mcs_tbs(const pusch_config_params&   pusch_params,
                                                  const ue_cell_configuration& ue_cell_cfg,
                                                  unsigned                     payload_size_bytes,
                                                  sch_mcs_index                starting_mcs,
                                                  unsigned                     max_nof_prbs);

} // namespace srsgnb
