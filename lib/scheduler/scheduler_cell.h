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

#include "cell/cell_configuration.h"
#include "cell/resource_grid.h"
#include "common_scheduling/csi_rs_scheduler.h"
#include "common_scheduling/paging_scheduler.h"
#include "common_scheduling/prach_scheduler.h"
#include "common_scheduling/ra_scheduler.h"
#include "common_scheduling/sib_scheduler.h"
#include "common_scheduling/ssb_scheduler.h"
#include "pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "pucch_scheduling/pucch_allocator_impl.h"
#include "pucch_scheduling/pucch_guardbands_scheduler.h"
#include "uci_scheduling/uci_allocator_impl.h"
#include "ue_scheduling/ue_scheduler.h"
#include "srsran/scheduler/config/scheduler_config.h"

namespace srsran {

class scheduler_metrics_handler;

/// \brief This class holds all the resources that are specific to a cell.
/// This includes the SIB and RA scheduler objects, PDCCH scheduler object, the cell resource grid, etc.
class scheduler_cell
{
public:
  explicit scheduler_cell(const scheduler_expert_config&                  sched_cfg,
                          const sched_cell_configuration_request_message& msg,
                          ue_scheduler&                                   ue_sched,
                          scheduler_event_logger&                         ev_logger);

  void run_slot(slot_point sl_tx);

  const cell_configuration cell_cfg;
  cell_resource_allocator  res_grid;

  ssb_scheduler                 ssb_sch;
  pdcch_resource_allocator_impl pdcch_sch;
  csi_rs_scheduler              csi_sch;
  ra_scheduler                  ra_sch;
  prach_scheduler               prach_sch;
  pucch_allocator_impl          pucch_alloc;
  uci_allocator_impl            uci_alloc;
  sib1_scheduler                sib1_sch;
  pucch_guardbands_scheduler    pucch_guard_sch;
  paging_scheduler              pg_sch;

  ue_scheduler& ue_sched;
};

} // namespace srsran
