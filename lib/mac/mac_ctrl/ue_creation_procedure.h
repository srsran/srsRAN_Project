
#ifndef SRSGNB_UE_CREATION_PROCEDURE_H
#define SRSGNB_UE_CREATION_PROCEDURE_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "../mac_dl/mac_dl_component.h"
#include "../mac_ul/mac_ul_component.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/async/schedule_on.h"

namespace srsgnb {

class mac_ue_create_request_procedure
{
public:
  explicit mac_ue_create_request_procedure(const mac_ue_create_request_message& req_,
                                           mac_common_config_t&                 cfg_,
                                           mac_ul_configurer&                   mac_ul_,
                                           mac_dl_configurer&                   mac_dl_) :
    req(req_), cfg(cfg_), logger(cfg.logger), ul_unit(mac_ul_), dl_unit(mac_dl_)
  {}

  void operator()(coro_context<async_task<void> >& ctx)
  {
    CORO_BEGIN(ctx);
    log_proc_started(logger, req.ue_index, req.crnti, "UE Create Request");

    // 1. Create UE UL context and channels
    CORO_AWAIT(ul_unit.add_ue(req));

    // 2. Create UE DL context and channels
    CORO_AWAIT(dl_unit.add_ue(req));

    // 3. After UE insertion in scheduler, send response to DU manager
    mac_ue_create_request_response_message resp{};
    resp.ue_index   = req.ue_index;
    resp.cell_index = req.cell_index;
    resp.result     = true;
    cfg.cfg_notifier.on_ue_create_request_complete(resp);

    log_proc_completed(logger, req.ue_index, req.crnti, "UE Create Request");
    CORO_RETURN();
  }

private:
  const mac_ue_create_request_message req;
  mac_common_config_t&                cfg;
  srslog::basic_logger&               logger;
  mac_ul_configurer&                  ul_unit;
  mac_dl_configurer&                  dl_unit;
};

} // namespace srsgnb

#endif // SRSGNB_UE_CREATION_PROCEDURE_H
