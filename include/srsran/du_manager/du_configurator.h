/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

struct du_mac_sched_control_config {
  uint64_t           ue_id;
  optional<unsigned> num_harq_processes;
  optional<unsigned> num_harq_retransmissions;
  optional<unsigned> min_prb_alloc;
  optional<unsigned> max_prb_alloc;
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

} // namespace srsran