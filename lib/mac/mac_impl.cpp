/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_impl.h"
#include "mac_ctrl/srs_sched_config_adapter.h"
#include "srsgnb/scheduler/scheduler_factory.h"

namespace srsgnb {

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

} // namespace srsgnb
