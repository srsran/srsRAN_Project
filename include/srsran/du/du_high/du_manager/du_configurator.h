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

#include "srsran/adt/byte_buffer.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/rrm.h"
#include "srsran/ran/slot_point.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

/// UE-level parameters of a cell MAC scheduler that need to be configured during the DU cell operation.
struct du_mac_sched_control_config {
  /// DU-UE-F1AP-ID of the UE being reconfigured.
  gnb_du_ue_f1ap_id_t ue_id;
  /// Sets the number of HARQ processes to be used.
  std::optional<unsigned> num_harq_processes;
  /// Sets the number of HARQ retransmissions to be used.
  std::optional<unsigned> num_harq_retransmissions;
  /// Set the radio resource management policy list.
  std::vector<rrm_policy_ratio_group> rrm_policy_ratio_list;
};

/// Result of a UE-level cell MAC scheduler parameter configuration procedure.
struct du_mac_sched_control_config_response {
  bool harq_processes_result;
  bool min_prb_alloc_result;
  bool max_prb_alloc_result;
};

/// Parameters of a cell that need to be configured during the DU cell operation.
struct du_cell_param_config_request {
  du_cell_param_config_request() = default;
  du_cell_param_config_request(nr_cell_global_id_t nr_cgi_, std::optional<int> ssb_pwr_mod_) :
    nr_cgi(nr_cgi_), ssb_pwr_mod(ssb_pwr_mod_)
  {
  }

  /// NR Cell Global ID of the cell being configured.
  nr_cell_global_id_t nr_cgi;
  /// SSB power modification to be applied to the cell. If absent, no modification is applied.
  std::optional<int> ssb_pwr_mod;
  /// Radio resource management policy list to be applied to the cell, If empty, no modification is applied.
  std::vector<rrm_policy_ratio_group> rrm_policy_ratio_list;
};

/// Parameters of the DU that need to be configured during operation.
struct du_param_config_request {
  /// Cells for which parameters need to be altered.
  std::vector<du_cell_param_config_request> cells;
};

/// Result of a DU dynamic parameter configuration procedure.
struct du_param_config_response {
  bool success;
};

/// Structure used to update SI PDU messages, without SI change notifications nor in a modification of valueTag in SIB1.
struct du_si_pdu_update_request {
  using time_point = std::chrono::system_clock::time_point;
  /// NR Cell Global ID of the cell being configured.
  nr_cell_global_id_t nr_cgi;
  /// SI message index.
  unsigned si_msg_idx;
  /// SIB index (e.g., sib2 => value 2).
  unsigned sib_idx;
  /// Slot at which the first SI is transmitted.
  slot_point slot;
  /// SI period in nof slots, required if more than one are SI PDU passed.
  std::optional<unsigned> si_slot_period;
  /// Packed content of SIB messages.
  span<byte_buffer> si_messages;
};

struct du_si_pdu_update_response {
  bool success;
};

/// Interface to configure parameters of an already operational DU.
class du_configurator
{
public:
  virtual ~du_configurator() = default;

  virtual async_task<du_mac_sched_control_config_response>
  configure_ue_mac_scheduler(du_mac_sched_control_config reconf) = 0;

  /// Apply new config updates requested from outside the DU.
  virtual du_param_config_response handle_operator_config_request(const du_param_config_request& req) = 0;

  /// Apply new SI PDU requested from outside the DU.
  virtual void handle_si_pdu_update(const du_si_pdu_update_request& req) = 0;
};

} // namespace srs_du
} // namespace srsran
