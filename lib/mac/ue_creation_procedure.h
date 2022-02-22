
#ifndef SRSGNB_UE_CREATION_PROCEDURE_H
#define SRSGNB_UE_CREATION_PROCEDURE_H

#include "../ran/gnb_format.h"
#include "mac_ctxt.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/manual_event.h"

namespace srsgnb {

class mac_ue_create_request_procedure final : public async_procedure<void>
{
public:
  explicit mac_ue_create_request_procedure(mac_context&                         mac_ctxt,
                                           manual_event_flag&                   sched_ue_create_ev_,
                                           const mac_ue_create_request_message& req_) :
    ctxt(mac_ctxt), req(req_), logger(ctxt.cfg.logger), sched_ue_create_ev(sched_ue_create_ev_)
  {}

  void start() override
  {
    log_proc_started(logger, req.ue_index, req.crnti, "UE Create Request");

    async_await(ul_ue_create_ev, &mac_ue_create_request_procedure::ue_ul_create_complete);

    // 1. Create UE associated UL channels
    ctxt.cfg.ul_exec.execute([this]() {
      ctxt.ul_worker.add_ue(req);

      // Return result back to CTRL execution context
      ctxt.cfg.ctrl_exec.execute([this]() { ul_ue_create_ev.set(); });
    });
  }

private:
  void ue_ul_create_complete()
  {
    async_await(sched_ue_create_ev, &mac_ue_create_request_procedure::sched_ue_config_response);

    // 2. Dispatch UE DL channels and sched UE creation to DL executors
    ctxt.cfg.dl_execs[req.cell_index]->execute([this]() { ctxt.dl_worker.add_ue(req); });
  }

  void sched_ue_config_response()
  {
    log_proc_completed(logger, req.ue_index, req.crnti, "Sched UE Config");

    // 4. After UE insertion in scheduler, send response to DU manager
    mac_ue_create_request_response_message resp{};
    resp.ue_index   = req.ue_index;
    resp.cell_index = req.cell_index;
    resp.result     = true;
    ctxt.cfg.cfg_notifier.on_ue_create_request_complete(resp);

    log_proc_completed(logger, req.ue_index, req.crnti, "UE Create Request");

    async_return();
  }

  mac_context&                        ctxt;
  const mac_ue_create_request_message req;
  srslog::basic_logger&               logger;
  manual_event_flag                   ul_ue_create_ev;
  manual_event_flag&                  sched_ue_create_ev;
};

} // namespace srsgnb

#endif // SRSGNB_UE_CREATION_PROCEDURE_H
