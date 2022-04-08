
#ifndef SRSGNB_UE_DELETE_PROCEDURE_H
#define SRSGNB_UE_DELETE_PROCEDURE_H

#include "../mac_config.h"
#include "../mac_config_interfaces.h"

namespace srsgnb {

class mac_ue_delete_procedure
{
public:
  explicit mac_ue_delete_procedure(const mac_ue_delete_request_message& msg,
                                   mac_common_config_t&                 cfg_,
                                   mac_ctrl_configurer&                 mac_ctrl_,
                                   mac_ul_configurer&                   mac_ul_,
                                   mac_dl_configurer&                   mac_dl_) :
    req(msg), cfg(cfg_), logger(cfg.logger), ctrl_mac(mac_ctrl_), ul_mac(mac_ul_), dl_mac(mac_dl_)
  {}

  void operator()(coro_context<async_task<mac_ue_delete_response_message> >& ctx)
  {
    CORO_BEGIN(ctx);

    log_proc_started(logger, req.ue_index, req.rnti, name());

    // 1. Remove UE from MAC DL worker
    CORO_AWAIT(dl_mac.remove_ue(req));

    // 2. Remove UE associated UL channels
    CORO_AWAIT(ul_mac.remove_ue(req));

    // 3. Enqueue UE deletion
    ctrl_mac.remove_ue(req.ue_index);

    log_proc_completed(logger, req.ue_index, req.rnti, "UE Delete Request");

    // 4. Signal end of procedure and pass response
    CORO_RETURN(mac_ue_delete_response_message{true});
  }

  static const char* name() { return "UE Delete Request"; }

private:
  mac_ue_delete_request_message req;
  mac_common_config_t&          cfg;
  srslog::basic_logger&         logger;
  mac_ctrl_configurer&          ctrl_mac;
  mac_ul_configurer&            ul_mac;
  mac_dl_configurer&            dl_mac;
};

} // namespace srsgnb

#endif // SRSGNB_UE_DELETE_PROCEDURE_H
