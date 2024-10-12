/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

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

class du_configurator
{
public:
  virtual ~du_configurator() = default;

  virtual async_task<du_mac_sched_control_config_response>
  configure_ue_mac_scheduler(du_mac_sched_control_config reconf) = 0;
};

} // namespace srs_du
} // namespace srsran
