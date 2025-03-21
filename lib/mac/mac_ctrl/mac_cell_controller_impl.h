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

#include "bcch_dl_sch_message_broker.h"
#include "srsran/mac/mac_cell_manager.h"

namespace srsran {

class mac_dl_configurator;
class mac_scheduler_configurator;

class mac_cell_controller_impl final : public mac_cell_controller
{
public:
  mac_cell_controller_impl(const mac_cell_creation_request& cell_cfg,
                           mac_scheduler_configurator&      sched,
                           mac_dl_configurator&             dl_unit);
  ~mac_cell_controller_impl() override;

  async_task<void>                       start() override { return mac_dl_cell->start(); }
  async_task<void>                       stop() override { return mac_dl_cell->stop(); }
  async_task<mac_cell_reconfig_response> reconfigure(const mac_cell_reconfig_request& request) override
  {
    return mac_dl_cell->reconfigure(request);
  }

private:
  const du_cell_index_t       cell_index;
  mac_scheduler_configurator& sched;
  mac_dl_configurator&        dl_unit;

  mac_cell_controller* mac_dl_cell = nullptr;

  bcch_dl_sch_message_broker si_broker;
};

} // namespace srsran
