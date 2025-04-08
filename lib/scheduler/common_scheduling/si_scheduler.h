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

#include "../support/paging_helpers.h"
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
  si_scheduler(const cell_configuration&                       cfg_,
               pdcch_resource_allocator&                       pdcch_sch,
               const sched_cell_configuration_request_message& msg);

  void run_slot(cell_resource_allocator& res_alloc);

  void handle_si_update_request(const si_scheduling_update_request& req);

private:
  void handle_pending_request(cell_resource_allocator& res_alloc);

  void try_schedule_short_message(cell_slot_resource_allocator& slot_alloc);
  void allocate_short_message(cell_slot_resource_allocator& slot_alloc);

  const cell_configuration& cell_cfg;
  const subcarrier_spacing  scs_common;
  const paging_slot_helper  paging_helper;
  const unsigned            default_paging_cycle;
  const unsigned            si_change_mod_period;
  pdcch_resource_allocator& pdcch_sch;
  srslog::basic_logger&     logger;

  sib1_scheduler       sib1_sched;
  si_message_scheduler si_msg_sched;

  si_version_type                                      last_version = 0;
  lockfree_triple_buffer<si_scheduling_update_request> pending_req;

  std::optional<si_scheduling_update_request> on_going_req;
  unsigned                                    si_change_start_count = 0;

  // Note: We use counts instead of slot_points because SI periods can be longer that 1024 * 10 msec.
  unsigned   slot_count = 0;
  slot_point last_sl_tx;
};

} // namespace srsran
