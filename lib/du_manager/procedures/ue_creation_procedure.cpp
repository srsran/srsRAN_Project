/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_creation_procedure.h"
#include "../adapters/mac_adapters.h"
#include "../adapters/rlc_adapters.h"

using namespace srsgnb;
using namespace srsgnb::srs_du;

ue_creation_procedure::ue_creation_procedure(du_ue_index_t                     ue_index,
                                             const ul_ccch_indication_message& ccch_ind_msg,
                                             const du_manager_config_t&        cfg_,
                                             ue_manager_ctrl_configurer&       ue_mng_) :
  msg(ccch_ind_msg), cfg(cfg_), logger(cfg.logger), ue_mng(ue_mng_)
{
  ue_ctx.ue_index    = ue_index;
  ue_ctx.rnti        = ccch_ind_msg.crnti;
  ue_ctx.pcell_index = ccch_ind_msg.cell_index;
}

void ue_creation_procedure::operator()(coro_context<async_task<void>>& ctx)
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

  // 3. Create UE SRB0 bearer.
  create_srb0();

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

void ue_creation_procedure::create_srb0()
{
  ue_ctx.bearers.emplace(LCID_SRB0);
  du_logical_channel_context& srb0 = ue_ctx.bearers[LCID_SRB0];
  srb0.lcid                        = LCID_SRB0;

  // Create RLC -> F1AP adapter.
  srb0.rlc_ul_sdu_notif = std::make_unique<rlc_rx_rrc_sdu_adapter>(*cfg.f1ap_rrc);

  // Create RLC CTRL adapter.
  srb0.rlc_ctrl_notif = std::make_unique<rlc_tx_control_notifier>();

  // Create RLC entity.
  rlc_entity_creation_message rlc_msg{};
  rlc_msg.ue_index    = ue_ctx.ue_index;
  rlc_msg.lcid        = LCID_SRB0;
  rlc_msg.upper_dn    = srb0.rlc_ul_sdu_notif.get();
  rlc_msg.upper_cn    = srb0.rlc_ctrl_notif.get();
  rlc_msg.config.mode = rlc_mode::tm;
  srb0.rlc_bearer     = create_rlc_entity(rlc_msg);

  // Create MAC -> RLC adapters.
  srb0.mac_rx_notifier = std::make_unique<mac_sdu_rx_adapter>(*srb0.rlc_bearer->get_rx_pdu_handler());
  srb0.mac_tx_notifier = std::make_unique<mac_sdu_tx_adapter>(*srb0.rlc_bearer->get_tx_pdu_transmitter());
}

async_task<mac_ue_create_response_message> ue_creation_procedure::make_mac_ue_create_req()
{
  mac_ue_create_request_message mac_ue_create_msg = test_helpers::make_default_ue_creation_request(); // TODO: TEMP.
  mac_ue_create_msg.ue_index                      = ue_ctx.ue_index;
  mac_ue_create_msg.crnti                         = msg.crnti;
  mac_ue_create_msg.cell_index                    = msg.cell_index;
  for (du_logical_channel_context& bearer : ue_ctx.bearers) {
    mac_ue_create_msg.bearers.emplace_back();
    auto& lc     = mac_ue_create_msg.bearers.back();
    lc.lcid      = bearer.lcid;
    lc.ul_bearer = bearer.mac_rx_notifier.get();
    lc.dl_bearer = bearer.mac_tx_notifier.get();
  }
  mac_ue_create_msg.ul_ccch_msg = &msg.subpdu;
  return cfg.mac_ue_mng->handle_ue_create_request(mac_ue_create_msg);
}

async_task<f1ap_ue_create_response> ue_creation_procedure::make_f1_ue_create_req()
{
  f1ap_ue_create_request f1_msg{};
  f1_msg.ue_index = ue_ctx.ue_index;
  return cfg.f1ap_ue_ctx_mng->handle_ue_creation_request(f1_msg);
}
