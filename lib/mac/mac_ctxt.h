
#ifndef SRSGNB_MAC_CTXT_H
#define SRSGNB_MAC_CTXT_H

#include "mac_dl_worker.h"
#include "mac_logical_channel.h"
#include "mac_ul_worker.h"
#include "sched.h"
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

struct mac_context {
  mac_common_config_t cfg;
  mac_dl_worker&      dl_worker;
  mac_ul_worker&      ul_worker;

  mac_context(mac_config_notifier& notifier,
              task_executor&       ul_exec_,
              span<task_executor*> dl_execs_,
              task_executor&       ctrl_exec_,
              mac_dl_worker&       dl_worker_,
              mac_ul_worker&       ul_worker_) :
    cfg(notifier, ul_exec_, dl_execs_, ctrl_exec_), dl_worker(dl_worker_), ul_worker(ul_worker_)
  {}
};

} // namespace srsgnb

#endif // SRSGNB_MAC_CTXT_H
