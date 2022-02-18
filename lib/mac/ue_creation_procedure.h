
#ifndef SRSGNB_UE_CREATION_PROCEDURE_H
#define SRSGNB_UE_CREATION_PROCEDURE_H

#include "../ran/gnb_format.h"
#include "mac_ctxt.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/mac/mac.h"

namespace srsgnb {

class mac_ue_create_request_procedure
{
public:
  explicit mac_ue_create_request_procedure(mac_context& mac_ctxt, const mac_ue_create_request_message& req_) :
    ctxt(mac_ctxt), req(req_), logger(ctxt.cfg.logger)
  {
    log_proc_started(logger, req.ue_index, req.crnti, "UE Create Request");

    // 1. Create UE associated UL channels
    ctxt.cfg.ul_exec.execute([this]() {
      ctxt.ul_worker.add_ue(req);

      // Return result back to CTRL execution context
      ctxt.cfg.ctrl_exec.execute([this]() { ue_ul_create_complete(); });
    });
  }

  void ue_ul_create_complete()
  {
    // 2. Dispatch UE DL channels and sched UE creation to DL executors
    ctxt.cfg.dl_execs[req.cell_index]->execute([this]() {
      // Create UE associated DL channels
      ctxt.dl_entities.insert(req.ue_index, req.crnti);

      // 3. Create UE in scheduler
      log_proc_started(logger, req.ue_index, req.crnti, "Sched UE Config");
      ctxt.sched_itf.config_ue(req.crnti);
    });
  }

  void sched_ue_config_response()
  {
    log_proc_completed(logger, req.ue_index, req.crnti, "Sched UE Config");

    // 4. After UE insertion in scheduler, send response to DU manager
    ctxt.cfg.ctrl_exec.execute([this]() {
      // 5. Send response back to DU manager
      mac_ue_create_request_response_message resp{};
      resp.ue_index   = req.ue_index;
      resp.cell_index = req.cell_index;
      resp.result     = true;
      ctxt.cfg.cfg_notifier.on_ue_create_request_complete(resp);

      log_proc_completed(logger, req.ue_index, req.crnti, "UE Create Request");
    });
  }

private:
  mac_context&                  ctxt;
  mac_ue_create_request_message req;
  srslog::basic_logger&         logger;
};

} // namespace srsgnb

#endif // SRSGNB_UE_CREATION_PROCEDURE_H
