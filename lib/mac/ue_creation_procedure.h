
#ifndef SRSGNB_UE_CREATION_PROCEDURE_H
#define SRSGNB_UE_CREATION_PROCEDURE_H

#include "../ran/gnb_format.h"
#include "mac_ctxt.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/async/schedule_on.h"

namespace srsgnb {

class mac_ue_create_request_procedure
{
public:
  explicit mac_ue_create_request_procedure(mac_context&                         mac_ctxt,
                                           manual_event_flag&                   sched_ue_create_ev_,
                                           const mac_ue_create_request_message& req_) :
    ctxt(mac_ctxt), req(req_), logger(ctxt.cfg.logger), sched_ue_create_ev(sched_ue_create_ev_)
  {}

  void operator()(coro_context<async_task<void> >& ctx)
  {
    CORO_BEGIN(ctx);
    log_proc_started(logger, req.ue_index, req.crnti, "UE Create Request");

    // 1. Change to UL execution context
    CORO_AWAIT(schedule_on(ctxt.cfg.ul_exec));

    // 2. Create UE UL context and channels
    ctxt.ul_worker.add_ue(req);

    // 3. Change to DL execution context
    CORO_AWAIT(schedule_on(*ctxt.cfg.dl_execs[req.cell_index]));

    // 4. Create UE DL context and channels
    ctxt.dl_worker.add_ue(req);

    // 5. Change to CTRL execution context
    CORO_AWAIT(schedule_on(ctxt.cfg.ctrl_exec));

    // 6. Await scheduler config notification
    CORO_AWAIT(sched_ue_create_ev);
    log_proc_completed(logger, req.ue_index, req.crnti, "Sched UE Config");

    // 7. After UE insertion in scheduler, send response to DU manager
    mac_ue_create_request_response_message resp{};
    resp.ue_index   = req.ue_index;
    resp.cell_index = req.cell_index;
    resp.result     = true;
    ctxt.cfg.cfg_notifier.on_ue_create_request_complete(resp);

    log_proc_completed(logger, req.ue_index, req.crnti, "UE Create Request");

    CORO_RETURN();
  }

private:
  mac_context&                        ctxt;
  const mac_ue_create_request_message req;
  srslog::basic_logger&               logger;
  manual_event_flag                   ul_ue_create_ev;
  manual_event_flag&                  sched_ue_create_ev;
};

} // namespace srsgnb

#endif // SRSGNB_UE_CREATION_PROCEDURE_H
