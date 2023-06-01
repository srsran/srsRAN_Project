/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "mac_impl.h"
#include "mac_ctrl/srs_sched_config_adapter.h"
#include "srsran/scheduler/scheduler_factory.h"

namespace srsran {

mac_impl::mac_impl(const mac_config& params) :
  cfg(params.ul_ccch_notifier,
      params.ue_exec_mapper,
      params.cell_exec_mapper,
      params.ctrl_exec,
      params.phy_notifier,
      params.pcap),
  sched_cfg_adapter(cfg),
  sched_obj(create_scheduler(
      scheduler_config{params.sched_cfg, sched_cfg_adapter.get_sched_notifier(), params.metric_notifier})),
  dl_unit(mac_dl_config{cfg.ue_exec_mapper,
                        cfg.cell_exec_mapper,
                        cfg.ctrl_exec,
                        cfg.phy_notifier,
                        params.mac_cfg,
                        params.pcap},
          *sched_obj,
          rnti_table),
  ul_unit(cfg, *sched_obj, rnti_table),
  rach_hdl(*sched_obj, rnti_table),
  ctrl_unit(cfg, ul_unit, dl_unit, rach_hdl, rnti_table, sched_cfg_adapter)
{
  sched_cfg_adapter.set_sched(*sched_obj);
}

void mac_impl::handle_dl_buffer_state_update_required(const mac_dl_buffer_state_indication_message& dl_bs)
{
  dl_buffer_state_indication_message bs{};
  bs.ue_index = dl_bs.ue_index;
  bs.lcid     = dl_bs.lcid;
  bs.bs       = dl_bs.bs;
  sched_obj->handle_dl_buffer_state_indication(bs);
}

} // namespace srsran
