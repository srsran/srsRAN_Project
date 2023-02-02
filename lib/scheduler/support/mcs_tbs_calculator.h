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

#include "../ue_scheduling/ue_configuration.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/sch_mcs.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

struct pusch_config_params;

/// Container for MCS and TBS results.
struct pusch_mcs_tbs {
  /// MCS to use for the UE's PUSCH.
  sch_mcs_index mcs;
  /// TBS to be allocated on the UE's PUSCH.
  unsigned tbs;
};

/// \brief Computes the PUSCH MCS, TBS and number of PRBs such that the effective code rate does not exceed 0.95.
///
/// This function computes the MCS, TBS required for the transmission of a given payload_size, so as to guarantee an
/// effective code rate <= 0.95.
///
/// \param[in] pusch_params PUSCH parameters needed to compute the MCS and TBS.
/// \param[in] ue_cell_cfg UE cell configuration.
/// \param[in] payload_size_bytes Number of UL-SCH buffer bytes available to be transmitted.
/// \param[in] max_mcs Initial value to be applied for the MCS; the final MCS might be lowered if the effective
/// code rate is above 0.95.
/// \param[in] nof_prbs Number of PRBs available for the PUSCH transmission.
/// \return The MCS and TBS, if for these values the effective code rate does not exceed 0.95; else, it returns an empty
/// optional object.
optional<pusch_mcs_tbs> compute_ul_mcs_tbs(const pusch_config_params&   pusch_params,
                                           const ue_cell_configuration& ue_cell_cfg,
                                           sch_mcs_index                max_mcs,
                                           unsigned                     nof_prbs);

} // namespace srsgnb
