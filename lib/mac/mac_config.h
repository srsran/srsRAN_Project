
#ifndef SRSGNB_MAC_CONFIG_H
#define SRSGNB_MAC_CONFIG_H

#include "sched/sched.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/task_executor.h"

namespace srsgnb {

struct mac_common_config_t {
  srslog::basic_logger&       logger;
  task_executor&              ul_exec;
  std::vector<task_executor*> dl_execs;
  task_executor&              ctrl_exec;
  mac_config_notifier&        cfg_notifier;

  mac_common_config_t(mac_config_notifier& notifier,
                      task_executor&       ul_exec_,
                      span<task_executor*> dl_execs_,
                      task_executor&       ctrl_exec_) :
    logger(srslog::fetch_basic_logger("MAC")),
    ul_exec(ul_exec_),
    dl_execs(dl_execs_.begin(), dl_execs_.end()),
    ctrl_exec(ctrl_exec_),
    cfg_notifier(notifier)
  {}
};


} // namespace srsgnb

#endif // SRSGNB_MAC_CONFIG_H
