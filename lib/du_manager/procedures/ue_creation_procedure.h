
#ifndef SRSGNB_UE_CREATION_PROCEDURE_H
#define SRSGNB_UE_CREATION_PROCEDURE_H

#include "../../ran/gnb_format.h"
#include "du_manager_config.h"
#include "du_manager_context.h"
#include "du_manager_interfaces.h"
#include "du_ue_context.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/rlc/rlc_factory.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

class mac_ul_ccch_adapter : public mac_sdu_rx_notifier
{
public:
  mac_ul_ccch_adapter(du_ue_index_t ue_index_, f1ap_du_ul_interface& f1ap_) : ue_index(ue_index_), f1ap(f1ap_) {}
  void on_new_sdu(mac_rx_sdu sdu) override
  {
    f1_rx_pdu msg{};
    msg.ue_index = ue_index;
    msg.lcid     = 0;
    msg.pdu      = std::move(sdu.pdu);
    f1ap.handle_pdu(std::move(msg));
  }

private:
  du_ue_index_t         ue_index;
  f1ap_du_ul_interface& f1ap;
};

class mac_ul_dcch_adapter : public mac_sdu_rx_notifier
{
public:
  mac_ul_dcch_adapter(rlc_pdu_handler& bearer_) : bearer(bearer_) {}
  void on_new_sdu(mac_rx_sdu sdu) override { bearer.handle_pdu(sdu.pdu); }

  rlc_pdu_handler& bearer;
};

class ue_creation_procedure
{
public:
  ue_creation_procedure(du_ue_context*                    ue_ptr,
                        const ul_ccch_indication_message& ccch_ind_msg,
                        const du_manager_config_t&        cfg_,
                        ue_manager_ctrl_configurer&       ue_mng_) :
    ue_ctx(ue_ptr), msg(ccch_ind_msg), cfg(cfg_), logger(cfg.logger), ue_mng(ue_mng_)
  {}

  void operator()(coro_context<async_task<void> >& ctx)
  {
    CORO_BEGIN(ctx);

    log_proc_started(logger, ue_ctx != nullptr ? ue_ctx->ue_index : MAX_NOF_UES, msg.crnti, "UE Create");

    // 1. Verify if UE context was succesfully created.
    if (ue_ctx == nullptr) {
      log_proc_failure(logger, MAX_NOF_UES, msg.crnti, name(), "Failure to allocate DU UE context.");
      clear_ue();
      CORO_EARLY_RETURN();
    }

    // 2. Initiate creation of F1 UE context and await result.
    CORO_AWAIT_VALUE(f1_resp, make_f1_ue_create_req());
    if (not f1_resp.result) {
      log_proc_failure(logger, ue_ctx->ue_index, msg.crnti, name(), "UE failed to be created in F1AP.");
      clear_ue();
      CORO_EARLY_RETURN();
    }

    // 3. Create UE RLC bearers.
    ue_ctx->bearers.emplace(0);
    ue_ctx->bearers[0].lcid            = 0;
    ue_ctx->bearers[0].mac_ul_notifier = std::make_unique<mac_ul_ccch_adapter>(ue_ctx->ue_index, *cfg.f1ap_ul);
    for (const auto& lc : msg.logical_channels_to_add) {
      ue_ctx->bearers.emplace(lc.lcid);
      auto& bearer = ue_ctx->bearers[lc.lcid];
      bearer.lcid  = 1;
      // Create UL RLC bearer
      bearer.ul_bearer = create_rlc_ul_bearer(ue_ctx->ue_index, lc.lcid, *cfg.rlc_ul_notifier);
      // Create UL RLC bearer adapter that is going to be used by MAC to notify Rx SDUs
      bearer.mac_ul_notifier = std::make_unique<mac_ul_dcch_adapter>(*bearer.ul_bearer);
      // Create DL RLC bearer
      // TODO
    }

    // 4. Initiate MAC UE creation and await result.
    CORO_AWAIT_VALUE(mac_resp, make_mac_ue_create_req());
    if (not mac_resp.result) {
      log_proc_failure(logger, ue_ctx->ue_index, msg.crnti, "UE failed to be created in MAC.");
      clear_ue();
      CORO_EARLY_RETURN();
    }

    // 5. Start Initial UL RRC Message Transfer by signalling MAC to notify CCCH to upper layers.
    cfg.mac->flush_ul_ccch_msg(msg.crnti);

    log_proc_completed(logger, ue_ctx->ue_index, msg.crnti, "UE Create");
    CORO_RETURN();
  }

  static const char* name() { return "UE Create"; }

private:
  void clear_ue()
  {
    if (f1_resp.result) {
      // TODO: F1 UE removal.
    }
    // TODO: MAC RNTI needs to be cleared.
    if (ue_ctx != nullptr) {
      ue_mng.remove_ue(ue_ctx->ue_index);
    }
  }

  async_task<mac_ue_create_response_message> make_mac_ue_create_req()
  {
    mac_ue_create_request_message mac_ue_create_msg{};
    mac_ue_create_msg.ue_index   = ue_ctx->ue_index;
    mac_ue_create_msg.crnti      = msg.crnti;
    mac_ue_create_msg.cell_index = msg.cell_index;
    for (du_logical_channel_context& bearer : ue_ctx->bearers) {
      mac_ue_create_msg.bearers.emplace_back();
      auto& lc     = mac_ue_create_msg.bearers.back();
      lc.lcid      = bearer.lcid;
      lc.ul_bearer = bearer.mac_ul_notifier.get();
      lc.dl_bearer = nullptr; // TODO
    }
    return cfg.mac->ue_create_request(mac_ue_create_msg);
  }

  async_task<f1ap_du_ue_create_response> make_f1_ue_create_req()
  {
    f1ap_du_ue_create_request f1_msg{};
    f1_msg.ue_index = ue_ctx->ue_index;
    return cfg.f1ap->handle_ue_creation_request(f1_msg);
  }

  du_ue_context*              ue_ctx = nullptr;
  ul_ccch_indication_message  msg;
  const du_manager_config_t&  cfg;
  srslog::basic_logger&       logger;
  ue_manager_ctrl_configurer& ue_mng;

  mac_ue_create_response_message mac_resp{};
  f1ap_du_ue_create_response     f1_resp{};
};

} // namespace srsgnb

#endif // SRSGNB_UE_CREATION_PROCEDURE_H
