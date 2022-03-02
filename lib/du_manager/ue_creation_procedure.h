
#ifndef SRSGNB_UE_CREATION_PROCEDURE_H
#define SRSGNB_UE_CREATION_PROCEDURE_H

#include "../ran/gnb_format.h"
#include "du_manager_config.h"
#include "du_manager_context.h"
#include "du_ue_context.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/rlc/rlc_factory.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

class mac_ul_dcch_adapter : public mac_ul_sdu_notifier
{
public:
  mac_ul_dcch_adapter(rlc_ul_bearer& bearer_) : bearer(bearer_) {}
  void on_ul_sdu(mac_ul_sdu sdu) override { bearer.push_pdu(sdu.pdu); }

  rlc_ul_bearer& bearer;
};

class ue_creation_procedure
{
public:
  ue_creation_procedure(const du_ue_create_message& ue_create_msg,
                        const du_manager_config_t&  cfg_,
                        du_ue_context&              ue_ctxt_,
                        ue_manager_ctrl_configurer& ue_mng_) :
    msg(ue_create_msg), cfg(cfg_), logger(cfg.logger), ue_ctxt(ue_ctxt_), ue_mng(ue_mng_)
  {}

  void operator()(coro_context<async_task<void> >& ctx)
  {
    CORO_BEGIN(ctx);

    log_proc_started(logger, msg.ue_index, msg.crnti, "UE Create");

    // 1. Create RLC Bearers
    for (auto& lc : msg.logical_channels_to_add) {
      ue_ctxt.bearers.emplace(lc.lcid);
      auto& bearer = ue_ctxt.bearers[lc.lcid];
      // Create UL RLC bearer
      bearer.ul_bearer = create_rlc_ul_bearer(ue_ctxt.ue_index, lc.lcid, *cfg.rlc_ul_notifier);
      // Create UL RLC bearer adapter that is going to be used by MAC to notify Rx SDUs
      bearer.mac_ul_notifier = std::make_unique<mac_ul_dcch_adapter>(*bearer.ul_bearer);
      // Create DL RLC bearer
      // TODO
    }

    // 2. Initiate MAC UE creation and await result
    CORO_AWAIT_VALUE(mac_ue_create_response_message resp, make_mac_request());

    if (resp.result) {
      log_proc_completed(logger, msg.ue_index, msg.crnti, "UE Create");
    } else {
      log_proc_failure(logger, msg.ue_index, msg.crnti, "UE Create");
    }

    // 3. Signal F1AP the UE creation outcome
    du_ue_create_response_message f1ap_resp{};
    f1ap_resp.ue_index = resp.ue_index;
    f1ap_resp.result   = resp.result;
    cfg.f1ap_cfg_notifier->on_du_ue_create_response(f1ap_resp);

    // 4. If UE creation failed, remove UE object from DU UE manager
    if (not f1ap_resp.result) {
      ue_mng.remove_ue(msg.ue_index);
    }

    CORO_RETURN();
  }

  static const char* name() { return "UE Create"; }

private:
  async_task<mac_ue_create_response_message> make_mac_request()
  {
    mac_ue_create_request_message mac_ue_create_msg{};
    mac_ue_create_msg.ue_index   = msg.ue_index;
    mac_ue_create_msg.crnti      = msg.crnti;
    mac_ue_create_msg.cell_index = msg.cell_index;
    for (du_logical_channel_context& bearer : ue_ctxt.bearers) {
      mac_ue_create_msg.bearers.emplace_back();
      auto& lc     = mac_ue_create_msg.bearers.back();
      lc.lcid      = 1;
      lc.ul_bearer = bearer.mac_ul_notifier.get();
      lc.dl_bearer = nullptr; // TODO
    }
    return cfg.mac->ue_create_request(mac_ue_create_msg);
  }

  du_ue_create_message        msg;
  const du_manager_config_t&  cfg;
  srslog::basic_logger&       logger;
  du_ue_context&              ue_ctxt;
  ue_manager_ctrl_configurer& ue_mng;
};

} // namespace srsgnb

#endif // SRSGNB_UE_CREATION_PROCEDURE_H
