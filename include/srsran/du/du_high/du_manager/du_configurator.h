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

#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/rrm.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

struct control_config_params {
  // Sets the number of HARQ processes to be used.
  std::optional<unsigned> num_harq_processes;
  // Sets the number of HARQ retransmissions to be used.
  std::optional<unsigned> num_harq_retransmissions;
  // Set the radio resource management policy.
  std::optional<rrm_policy_ratio_group> rrm_policy_group;
};

struct du_mac_sched_control_config {
  uint64_t                           ue_id;
  std::vector<control_config_params> param_list;
};

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

/// Interface to configure parameters of an already operational DU.
class du_configurator
{
public:
  virtual ~du_configurator() = default;

  virtual async_task<du_mac_sched_control_config_response>
  configure_ue_mac_scheduler(du_mac_sched_control_config reconf) = 0;

  /// Apply new config updates requested from outside the DU.
  virtual du_param_config_response handle_operator_config_request(const du_param_config_request& req) = 0;
};

} // namespace srs_du
} // namespace srsran
