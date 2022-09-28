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

mac_impl::mac_impl(mac_ul_ccch_notifier&       event_notifier,
                   du_high_ue_executor_mapper& ul_exec_mapper_,
                   du_l2_dl_executor_mapper&   dl_exec_mapper_,
                   task_executor&              ctrl_exec_,
                   mac_result_notifier&        phy_notifier_) :
  cfg(event_notifier, ul_exec_mapper_, dl_exec_mapper_, ctrl_exec_, phy_notifier_),
  sched_cfg_adapter(cfg),
  sched_obj(create_scheduler(sched_cfg_adapter.get_sched_notifier())),
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
