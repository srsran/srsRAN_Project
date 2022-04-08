
#ifndef SRSGNB_UE_CREATION_PROCEDURE_H
#define SRSGNB_UE_CREATION_PROCEDURE_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/async/lazy_task.h"

namespace srsgnb {

class mac_ue_create_request_procedure
{
public:
  explicit mac_ue_create_request_procedure(const mac_ue_create_request_message& req_,
                                           mac_common_config_t&                 cfg_,
                                           mac_ctrl_configurer&                 mac_ctrl_,
                                           mac_ul_configurer&                   mac_ul_,
                                           mac_dl_configurer&                   mac_dl_) :
    req(req_), cfg(cfg_), logger(cfg.logger), ctrl_unit(mac_ctrl_), ul_unit(mac_ul_), dl_unit(mac_dl_)
  {}

  void operator()(coro_context<lazy_task<mac_ue_create_response_message> >& ctx)
  {
    CORO_BEGIN(ctx);
    log_proc_started(logger, req.ue_index, req.crnti, "UE Create Request");

    // 1. Create UE in MAC CTRL
    ctrl_ue_created = ctrl_unit.add_ue(req.ue_index, req.crnti, req.cell_index);
    if (not ctrl_ue_created) {
      CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
    }

    // 2. Create UE UL context and channels
    CORO_AWAIT_VALUE(add_ue_result, ul_unit.add_ue(req));
    if (not add_ue_result) {
      CORO_EARLY_RETURN(handle_mac_ue_create_result(false));
    }

    // 3. Create UE DL context and channels
    CORO_AWAIT_VALUE(add_ue_result, dl_unit.add_ue(req));

    // 4. After UE insertion in scheduler, send response to DU manager
    CORO_RETURN(handle_mac_ue_create_result(add_ue_result));
  }

  static const char* name() { return "UE Create Request"; }

private:
  mac_ue_create_response_message handle_mac_ue_create_result(bool result)
  {
    if (result) {
      log_proc_completed(logger, req.ue_index, req.crnti, "UE Create Request");
    } else {
      log_proc_failure(logger, req.ue_index, req.crnti, "UE Create Request");
    }

    if (not result and ctrl_ue_created) {
      // Remove created UE object
      ctrl_unit.remove_ue(req.ue_index);
    }

    // Respond back to DU manager with result
    mac_ue_create_response_message resp{};
    resp.ue_index   = req.ue_index;
    resp.cell_index = req.cell_index;
    resp.result     = result;
    return resp;
  }

  const mac_ue_create_request_message req;
  mac_common_config_t&                cfg;
  srslog::basic_logger&               logger;
  mac_ctrl_configurer&                ctrl_unit;
  mac_ul_configurer&                  ul_unit;
  mac_dl_configurer&                  dl_unit;

  bool ctrl_ue_created = false;
  bool add_ue_result   = false;
};

} // namespace srsgnb

#endif // SRSGNB_UE_CREATION_PROCEDURE_H
