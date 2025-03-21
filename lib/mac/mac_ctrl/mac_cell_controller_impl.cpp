/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_cell_controller_impl.h"
#include "../mac_config_interfaces.h"
#include "mac_scheduler_configurator.h"

using namespace srsran;

mac_cell_controller_impl::mac_cell_controller_impl(const mac_cell_creation_request& cell_cfg,
                                                   mac_scheduler_configurator&      sched_,
                                                   mac_dl_configurator&             dl_unit_) :
  cell_index(cell_cfg.cell_index), sched(sched_), dl_unit(dl_unit_)
{
  // Add cell to MAC DL.
  mac_dl_cell = &dl_unit.add_cell(cell_cfg);

  // Add cell to scheduler.
  sched.add_cell(cell_cfg);

  // SI modification request.
  si_change_request si_req;
  for (unsigned i = 0, e = cell_cfg.bcch_dl_sch_payloads.size(); i != e; ++i) {
    if (i == 0) {
      si_req.sib1 = cell_cfg.bcch_dl_sch_payloads[i].copy();
    } else {
      si_req.si_messages_to_addmod.push_back(std::make_pair(i - 1, cell_cfg.bcch_dl_sch_payloads[i].copy()));
    }
  }
  si_change_response si_resp = si_broker.handle_si_change_request(si_req);
  (void)si_resp;
}

mac_cell_controller_impl::~mac_cell_controller_impl()
{
  // Remove cell from scheduler.
  sched.remove_cell(cell_index);

  // Remove cell from MAC DL.
  dl_unit.remove_cell(cell_index);
}
