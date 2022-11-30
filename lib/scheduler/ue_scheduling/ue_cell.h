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

#include "harq_process.h"
#include "ue_configuration.h"
#include "srsgnb/scheduler/config/scheduler_expert_config.h"

namespace srsgnb {

/// \brief Context respective to a UE serving cell.
class ue_cell
{
public:
  ue_cell(du_ue_index_t                                ue_index_,
          rnti_t                                       crnti_val,
          const scheduler_ue_expert_config&            expert_cfg_,
          const cell_configuration&                    cell_cfg_common_,
          const serving_cell_ue_configuration_request& ue_serv_cell);

  const du_ue_index_t   ue_index;
  const du_cell_index_t cell_index;

  harq_entity harqs;

  rnti_t rnti() const { return crnti_; }

  bwp_id_t active_bwp_id() const { return to_bwp_id(0); }
  bool     is_active() const { return true; }

  const ue_cell_configuration& cfg() const { return ue_cfg; }

  /// \brief Estimate the number of required DL PRBs to allocate the given number of bytes.
  unsigned required_dl_prbs(unsigned time_resource, unsigned pending_bytes) const;

  /// \brief Estimate the number of required UL PRBs to allocate the given number of bytes.
  unsigned required_ul_prbs(unsigned time_resource, unsigned pending_bytes) const;

private:
  rnti_t                            crnti_;
  const scheduler_ue_expert_config& expert_cfg;
  ue_cell_configuration             ue_cfg;
};

} // namespace srsgnb