/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UE_CREATION_PROCEDURE_H
#define SRSGNB_UE_CREATION_PROCEDURE_H

#include "../../ran/gnb_format.h"
#include "../converters/mac_cell_configuration_helpers.h"
#include "du_manager_config.h"
#include "du_manager_interfaces.h"
#include "du_ue_context.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/rlc/rlc_factory.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_du {

class mac_ul_ccch_adapter : public mac_sdu_rx_notifier
{
public:
  mac_ul_ccch_adapter(du_ue_index_t ue_index_, f1ap_rrc_message_transfer_procedure_handler& f1ap_rrc_) :
    ue_index(ue_index_), f1ap_rrc(f1ap_rrc_)
  {
  }
  void on_new_sdu(mac_rx_sdu sdu) override
  {
    f1_rx_pdu msg{};
    msg.ue_index = ue_index;
    msg.lcid     = LCID_SRB0;
    msg.pdu      = std::move(sdu.pdu);
    f1ap_rrc.handle_pdu(std::move(msg));
  }

private:
  du_ue_index_t                                ue_index;
  f1ap_rrc_message_transfer_procedure_handler& f1ap_rrc;
};

class mac_ul_dcch_adapter : public mac_sdu_rx_notifier
{
public:
  mac_ul_dcch_adapter(rlc_pdu_handler& bearer_) : bearer(bearer_) {}
  void on_new_sdu(mac_rx_sdu sdu) override { bearer.handle_pdu(std::move(sdu.pdu)); }

  rlc_pdu_handler& bearer;
};

/// \brief Handles the creation of a UE and respective bearers in the DU UE manager, MAC, F1.
///  \startuml
///    participant DM
///    participant MAC
///    participant F1
///    Note over MAC: UL CCCH subPDU decoded
///    MAC-->>DM: UL CCCH PDU Indication
///    Note over DM: Allocate UE index
///    Note over DM: Buffer UL CCCH Indication
///    DM->>F1: UE create
///    F1-->>DM: UE create completed
///    Note over DM: Create RLC SRB0 (Factory)
///    DM->>MAC: UE create
///    MAC-->>DM: UE create completed
///    Note over DM: Create UE in UE manager
///    DM->>MAC: Notify UL CCCH PDU
///    MAC->>F1: SRB0 - Notify UL CCCH PDU
///  \enduml
class ue_creation_procedure
{
public:
  ue_creation_procedure(du_ue_index_t                     ue_index,
                        const ul_ccch_indication_message& ccch_ind_msg,
                        const du_manager_config_t&        cfg_,
                        ue_manager_ctrl_configurer&       ue_mng_) :
    msg(ccch_ind_msg), cfg(cfg_), logger(cfg.logger), ue_mng(ue_mng_)
  {
    ue_ctx.ue_index    = ue_index;
    ue_ctx.rnti        = ccch_ind_msg.crnti;
    ue_ctx.pcell_index = ccch_ind_msg.cell_index;
  }

  void operator()(coro_context<async_task<void> >& ctx)
  {
    CORO_BEGIN(ctx);

    log_proc_started(logger, ue_ctx.ue_index, msg.crnti, "UE Create");

    // 1. Verify if UE index was successfully allocated and params are valid.
    if (ue_ctx.ue_index == MAX_NOF_DU_UES) {
      log_proc_failure(logger, MAX_NOF_DU_UES, msg.crnti, name(), "Failure to allocate DU UE index.");
      CORO_EARLY_RETURN();
    }
    if (ue_mng.find_rnti(ue_ctx.rnti) != nullptr) {
      log_proc_failure(logger, MAX_NOF_DU_UES, msg.crnti, name(), "Repeated RNTI.");
      CORO_EARLY_RETURN();
    }

    // 2. Initiate creation of F1 UE context and await result.
    CORO_AWAIT_VALUE(f1_resp, make_f1_ue_create_req());
    if (not f1_resp.result) {
      log_proc_failure(logger, ue_ctx.ue_index, msg.crnti, name(), "UE failed to be created in F1AP.");
      clear_ue();
      CORO_EARLY_RETURN();
    }

    // 3. Create UE RLC bearers.
    ue_ctx.bearers.emplace(0);
    ue_ctx.bearers[0].lcid            = LCID_SRB0;
    ue_ctx.bearers[0].mac_ul_notifier = std::make_unique<mac_ul_ccch_adapter>(ue_ctx.ue_index, *cfg.f1ap_rrc);

    // 4. Initiate MAC UE creation and await result.
    CORO_AWAIT_VALUE(mac_resp, make_mac_ue_create_req());
    if (not mac_resp.result) {
      log_proc_failure(logger, ue_ctx.ue_index, msg.crnti, "UE failed to be created in MAC.");
      clear_ue();
      CORO_EARLY_RETURN();
    }

    // 5. Create UE context in DU manager.
    if (ue_mng.add_ue(std::move(ue_ctx)) == nullptr) {
      log_proc_failure(logger, ue_ctx.ue_index, msg.crnti, "UE failed to be created in DU manager.");
      clear_ue();
      CORO_EARLY_RETURN();
    }

    // 6. Start Initial UL RRC Message Transfer by signalling MAC to notify CCCH to upper layers.
    if (not msg.subpdu.empty()) {
      cfg.mac_ue_mng->handle_ul_ccch_msg(ue_ctx.ue_index, std::move(msg.subpdu));
    }

    log_proc_completed(logger, ue_ctx.ue_index, msg.crnti, "UE Create");
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
  }

  async_task<mac_ue_create_response_message> make_mac_ue_create_req()
  {
    mac_ue_create_request_message mac_ue_create_msg = test_helpers::make_default_ue_creation_request(); // TODO: TEMP.
    mac_ue_create_msg.ue_index                      = ue_ctx.ue_index;
    mac_ue_create_msg.crnti                         = msg.crnti;
    mac_ue_create_msg.cell_index                    = msg.cell_index;
    for (du_logical_channel_context& bearer : ue_ctx.bearers) {
      mac_ue_create_msg.bearers.emplace_back();
      auto& lc     = mac_ue_create_msg.bearers.back();
      lc.lcid      = bearer.lcid;
      lc.ul_bearer = bearer.mac_ul_notifier.get();
      lc.dl_bearer = nullptr; // TODO
    }
    mac_ue_create_msg.ul_ccch_msg = &msg.subpdu;
    return cfg.mac_ue_mng->handle_ue_create_request(mac_ue_create_msg);
  }

  async_task<f1ap_ue_create_response> make_f1_ue_create_req()
  {
    f1ap_ue_create_request f1_msg{};
    f1_msg.ue_index = ue_ctx.ue_index;
    return cfg.f1ap_ue_ctx_mng->handle_ue_creation_request(f1_msg);
  }

  ul_ccch_indication_message  msg;
  const du_manager_config_t&  cfg;
  srslog::basic_logger&       logger;
  ue_manager_ctrl_configurer& ue_mng;

  du_ue_context                  ue_ctx{};
  mac_ue_create_response_message mac_resp{};
  f1ap_ue_create_response        f1_resp{};
};

} // namespace srs_du
} // namespace srsgnb

#endif // SRSGNB_UE_CREATION_PROCEDURE_H
