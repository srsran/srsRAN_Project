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

#include "srsgnb/mac/mac.h"
#include "srsgnb/scheduler/scheduler_configurator.h"

namespace srsgnb {

/// Convert MAC UE Create Request Message to Scheduler UE Create Request.
inline sched_ue_creation_request_message make_ue_creation_request(const mac_ue_create_request_message& in)
{
  sched_ue_creation_request_message out{};
  out.ue_index      = in.ue_index;
  out.pcell_index   = in.cell_index;
  out.crnti         = in.crnti;
  out.serv_cell_cfg = in.serv_cell_cfg;
  return out;
}

/// Convert MAC UE Reconfiguration Request Message to Scheduler UE Reconfiguration Request.
inline sched_ue_reconfiguration_message
make_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& in)
{
  sched_ue_reconfiguration_message out{};
  out.ue_index    = in.ue_index;
  out.pcell_index = in.pcell_index;
  out.crnti       = in.crnti;
  return out;
}

} // namespace srsgnb
