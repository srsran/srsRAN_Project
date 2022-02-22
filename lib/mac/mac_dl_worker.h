
#ifndef SRSGNB_MAC_DL_WORKER_H
#define SRSGNB_MAC_DL_WORKER_H

#include "../ran/gnb_format.h"
#include "mac_dl_mux.h"
#include "sched.h"
#include "srsgnb/mac/mac.h"

namespace srsgnb {

class mac_dl_worker
{
public:
  explicit mac_dl_worker(sched_cfg_notifier& notifier) : sched_obj(notifier) {}

  bool contains_ue(du_ue_index_t ue_index) const { return mux.contains_ue(ue_index); }

  void add_ue(const mac_ue_create_request_message& request)
  {
    // 1. Insert UE and DL bearers
    mux.add_ue(request.ue_index, request.crnti, request.bearers);
    // TODO

    // 2. Create UE in scheduler
    log_proc_started(logger, request.ue_index, request.crnti, "Sched UE Config");
    sched_obj.config_ue(request.crnti);
  }

  void remove_ue(du_ue_index_t ue_index)
  {
    // 1. Remove UE from scheduler
    sched_obj.delete_ue_request(mux.get_rnti(ue_index));
  }

  void on_sched_ue_remove_complete(du_ue_index_t ue_index)
  {
    // 2. Remove UE associated DL channels
    mux.remove_ue(ue_index);
  }

private:
  srslog::basic_logger& logger{srslog::fetch_basic_logger("MAC")};

  mac_dl_mux mux;
  sched      sched_obj;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_DL_WORKER_H
