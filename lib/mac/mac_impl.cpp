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

mac_impl::mac_impl(const mac_config& mac_cfg) :
  cfg(mac_cfg.ul_ccch_notifier,
      mac_cfg.ul_exec_mapper,
      mac_cfg.dl_exec_mapper,
      mac_cfg.ctrl_exec,
      mac_cfg.phy_notifier,
      mac_cfg.pcap),
  sched_cfg_adapter(cfg),
  sched_obj(create_scheduler(
      scheduler_config{mac_cfg.sched_cfg, sched_cfg_adapter.get_sched_notifier(), mac_cfg.metric_notifier})),
  dl_unit(cfg, *sched_obj, rnti_table),
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
