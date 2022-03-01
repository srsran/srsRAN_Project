
#ifndef SRSGNB_UE_RECONFIGURATION_PROCEDURE_H
#define SRSGNB_UE_RECONFIGURATION_PROCEDURE_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

class mac_ue_reconfiguration_procedure
{
public:
  explicit mac_ue_reconfiguration_procedure(const mac_ue_reconfiguration_request_message& req_,
                                            mac_common_config_t&                          cfg_,
                                            mac_ul_configurer&                            mac_ul_,
                                            mac_dl_configurer&                            mac_dl_) :
    req(req_), cfg(cfg_), logger(cfg.logger), ul_unit(mac_ul_), dl_unit(mac_dl_)
  {}

  void operator()(coro_context<async_task<void> >& ctx)
  {
    CORO_BEGIN(ctx);
    log_proc_started(logger, req.ue_index, req.crnti, "UE Create Request");

    // 1. Reconfigure UE UL context and channels
    // Note: We may need to delete bearers first from DL to avoid concurrent scheduling in erased bearers.
    CORO_AWAIT_VALUE(add_ue_result, ul_unit.reconfigure_ue(req));
    if (not add_ue_result) {
      send_mac_ue_reconfiguration_response(false);
      CORO_EARLY_RETURN();
    }

    // 2. Reconfigure UE DL context and channels
    CORO_AWAIT_VALUE(add_ue_result, dl_unit.reconfigure_ue(req));

    // 3. After UE insertion in scheduler, send response to DU manager
    send_mac_ue_reconfiguration_response(add_ue_result);
    CORO_RETURN();
  }

  static const char* name() { return "UE Reconfiguration Request"; }

private:
  void send_mac_ue_reconfiguration_response(bool result)
  {
    if (result) {
      log_proc_completed(logger, req.ue_index, req.crnti, name());
    } else {
      log_proc_failure(logger, req.ue_index, req.crnti, name());
    }

    // Respond back to DU manager with result
    mac_ue_reconfiguration_response_message resp{};
    resp.ue_index = req.ue_index;
    resp.result   = result;
    cfg.cfg_notifier.on_ue_reconfiguration_complete(resp);
  }

  mac_ue_reconfiguration_request_message req;
  mac_common_config_t&                   cfg;
  srslog::basic_logger&                  logger;
  mac_ul_configurer&                     ul_unit;
  mac_dl_configurer&                     dl_unit;

  bool add_ue_result = false;
};

} // namespace srsgnb

#endif // SRSGNB_UE_RECONFIGURATION_PROCEDURE_H
