/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "si_message_scheduler.h"
#include "sib1_scheduler.h"
#include "srsran/adt/lockfree_triple_buffer.h"
#include "srsran/scheduler/scheduler_sys_info_handler.h"

namespace srsran {

struct si_scheduling_update_request;

/// Class responsible for scheduling SIB1 and SI-messages given the cell configuration.
class si_scheduler
{
public:
  si_scheduler(const cell_configuration&                  cfg_,
               pdcch_resource_allocator&                  pdcch_sch,
               units::bytes                               sib1_payload_size,
               const std::optional<si_scheduling_config>& si_sched_cfg);

  void run_slot(cell_resource_allocator& res_alloc);

  void handle_si_update_request(const si_scheduling_update_request& req);

private:
  void handle_pending_request(cell_resource_allocator& res_alloc);

  const cell_configuration& cell_cfg;
  const subcarrier_spacing  scs_common;
  const unsigned            default_paging_cycle;
  const unsigned            si_change_mod_period;

  sib1_scheduler       sib1_sched;
  si_message_scheduler si_msg_sched;

  lockfree_triple_buffer<si_scheduling_update_request> pending_req;

  std::optional<si_scheduling_update_request> on_going_req;
  slot_point                                  si_change_mod_start;
  unsigned                                    last_version = 0;
};

} // namespace srsran
