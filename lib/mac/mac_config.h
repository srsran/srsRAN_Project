
#ifndef SRSGNB_LIB_MAC_MAC_CONFIG_H
#define SRSGNB_LIB_MAC_MAC_CONFIG_H

#include "sched/sched.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/du_l2_ul_executor_mapper.h"

namespace srsgnb {

struct mac_common_config_t {
  srslog::basic_logger&       logger;
  mac_event_notifier&         event_notifier;
  du_l2_ul_executor_mapper&   ul_exec_mapper;
  std::vector<task_executor*> dl_execs;
  task_executor&              ctrl_exec;

  mac_common_config_t(mac_event_notifier&       event_notifier_,
                      du_l2_ul_executor_mapper& ul_exec_,
                      span<task_executor*>      dl_execs_,
                      task_executor&            ctrl_exec_) :
    logger(srslog::fetch_basic_logger("MAC")),
    event_notifier(event_notifier_),
    ul_exec_mapper(ul_exec_),
    dl_execs(dl_execs_.begin(), dl_execs_.end()),
    ctrl_exec(ctrl_exec_)
  {}
};

} // namespace srsgnb

#endif // SRSGNB_LIB_MAC_MAC_CONFIG_H
