/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_impl.h"
#include "srsran_scheduler_adapter.h"
#include "srsran/scheduler/scheduler_factory.h"

namespace srsran {

mac_impl::mac_impl(const mac_config& params) :
  cfg(params.ul_ccch_notifier,
      params.ue_exec_mapper,
      params.cell_exec_mapper,
      params.ctrl_exec,
      params.phy_notifier,
      params.pcap),
  rlf_handler(params.mac_cfg.max_consecutive_dl_kos, params.mac_cfg.max_consecutive_ul_kos),
  sched_cfg_adapter(std::make_unique<srsran_scheduler_adapter>(cfg, rnti_table, rlf_handler)),
  sched_obj(create_scheduler(
      scheduler_config{params.sched_cfg, sched_cfg_adapter->get_sched_notifier(), params.metric_notifier})),
  dl_unit(mac_dl_config{cfg.ue_exec_mapper,
                        cfg.cell_exec_mapper,
                        cfg.ctrl_exec,
                        cfg.phy_notifier,
                        params.mac_cfg,
                        params.pcap},
          *sched_cfg_adapter,
          rnti_table),
  ul_unit(cfg, *sched_cfg_adapter, rnti_table),
  rach_hdl(*sched_obj, rnti_table),
  ctrl_unit(cfg, ul_unit, dl_unit, rach_hdl, rnti_table, *sched_cfg_adapter)
{
  sched_cfg_adapter->set_sched(*sched_obj);
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
