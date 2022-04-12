
#include "mac_impl.h"
#include "sched/sched.h"

namespace srsgnb {

mac_impl::mac_impl(mac_ul_ccch_notifier&     event_notifier,
                   du_l2_ul_executor_mapper& ul_exec_mapper_,
                   span<task_executor*>      dl_execs_,
                   task_executor&            ctrl_exec_,
                   mac_result_notifier&      phy_notifier_) :
  cfg(event_notifier, ul_exec_mapper_, dl_execs_, ctrl_exec_, phy_notifier_),
  sched_cfg_adapter(cfg),
  sched_obj(std::make_unique<sched>(sched_cfg_adapter.get_notifier())),
  dl_unit(cfg, sched_cfg_adapter, *sched_obj, rnti_table),
  ul_unit(cfg, *sched_obj, rnti_table),
  ctrl_unit(cfg, ul_unit, dl_unit, rnti_table),
  rach_hdl(*sched_obj, rnti_table)
{}

} // namespace srsgnb
