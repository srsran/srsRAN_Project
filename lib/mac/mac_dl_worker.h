
#ifndef SRSGNB_MAC_DL_WORKER_H
#define SRSGNB_MAC_DL_WORKER_H

#include "../ran/gnb_format.h"
#include "mac_logical_channel.h"
#include "sched.h"
#include "srsgnb/mac/mac.h"

namespace srsgnb {

class mac_dl_worker
{
public:
  explicit mac_dl_worker(sched_cfg_notifier& notifier) : sched_obj(notifier) {}

  void add_ue(const mac_ue_create_request_message& request)
  {
    // 1. Insert UE
    // TODO

    // 2. Add UE DL Bearers
    dl_entities.insert(request.ue_index, request.crnti);

    // 3. Create UE in scheduler
    log_proc_started(logger, request.ue_index, request.crnti, "Sched UE Config");
    sched_obj.config_ue(request.crnti);
  }

  void remove_ue(rnti_t rnti)
  {
    // 1. Remove UE from scheduler
    sched_obj.delete_ue_request(rnti);
  }

  void on_sched_ue_remove_complete(rnti_t rnti)
  {
    // 2. Remove UE associated DL channels
    dl_entities.erase(rnti);
  }

private:
  srslog::basic_logger& logger{srslog::fetch_basic_logger("MAC")};

  mac_dl_dcch_manager dl_entities;
  sched               sched_obj;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_DL_WORKER_H
