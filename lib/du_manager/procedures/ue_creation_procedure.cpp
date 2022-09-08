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
  ue_ctx.bearers.emplace(LCID_SRB0);
  ue_ctx.bearers[LCID_SRB0].lcid = LCID_SRB0;
}

void ue_creation_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  log_proc_started(logger, ue_ctx.ue_index, msg.crnti, "UE Create");

  // > Verify if UE index was successfully allocated and params are valid.
  if (ue_ctx.ue_index == MAX_NOF_DU_UES) {
    log_proc_failure(logger, MAX_NOF_DU_UES, msg.crnti, name(), "Failure to allocate DU UE index.");
    CORO_EARLY_RETURN();
  }
  if (ue_mng.find_rnti(ue_ctx.rnti) != nullptr) {
    log_proc_failure(logger, MAX_NOF_DU_UES, msg.crnti, name(), "Repeated RNTI.");
    CORO_EARLY_RETURN();
  }

  // > Initiate creation of F1 UE context and await result.
  create_f1_ue();
  if (not f1_resp.result) {
    log_proc_failure(logger, ue_ctx.ue_index, msg.crnti, name(), "UE failed to be created in F1AP.");
    clear_ue();
    CORO_EARLY_RETURN();
  }

  // > Initiate creation of RLC SRB0.
  create_rlc_srb0();

  // > Initiate MAC UE creation and await result.
  CORO_AWAIT_VALUE(mac_resp, make_mac_ue_create_req());
  if (not mac_resp.result) {
    log_proc_failure(logger, ue_ctx.ue_index, msg.crnti, "UE failed to be created in MAC.");
    clear_ue();
    CORO_EARLY_RETURN();
  }

  // > Create UE context in DU manager.
  if (ue_mng.add_ue(std::move(ue_ctx)) == nullptr) {
    log_proc_failure(logger, ue_ctx.ue_index, msg.crnti, "UE failed to be created in DU manager.");
    clear_ue();
    CORO_EARLY_RETURN();
  }

  // > Start Initial UL RRC Message Transfer by signalling MAC to notify CCCH to upper layers.
  if (not msg.subpdu.empty()) {
    cfg.mac_ue_mng->handle_ul_ccch_msg(ue_ctx.ue_index, std::move(msg.subpdu));
  }

  log_proc_completed(logger, ue_ctx.ue_index, msg.crnti, "UE Create");
  CORO_RETURN();
}

void ue_creation_procedure::create_rlc_srb0()
{
  du_logical_channel_context& srb0 = ue_ctx.bearers[LCID_SRB0];

  // Create RLC TX data adapter.
  srb0.rlc_tx_ul_data_notif = std::make_unique<rlc_tx_data_notifier>();

  // Create RLC TX control adapter.
  srb0.rlc_tx_ul_ctrl_notif = std::make_unique<rlc_tx_control_notifier>();

  // Create RLC BSR update notification adapter.
  srb0.rlc_tx_ll_data_notif =
      std::make_unique<rlc_tx_mac_buffer_state_updater>(ue_ctx.ue_index, LCID_SRB0, *cfg.mac_ue_ctrl);

  // Create RLC RX -> F1AP adapter.
  srb0.rlc_rx_ul_sdu_notif = std::make_unique<rlc_rx_rrc_sdu_adapter>(*f1_resp.bearers_added[0]);

  // Create RLC entity.
  rlc_entity_creation_message rlc_msg{};
  rlc_msg.ue_index    = ue_ctx.ue_index;
  rlc_msg.lcid        = LCID_SRB0;
  rlc_msg.rx_upper_dn = srb0.rlc_rx_ul_sdu_notif.get();
  rlc_msg.tx_upper_dn = srb0.rlc_tx_ul_data_notif.get();
  rlc_msg.tx_upper_cn = srb0.rlc_tx_ul_ctrl_notif.get();
  rlc_msg.tx_lower_dn = srb0.rlc_tx_ll_data_notif.get();
  rlc_msg.config.mode = rlc_mode::tm;
  srb0.rlc_bearer     = create_rlc_entity(rlc_msg);
}

async_task<mac_ue_create_response_message> ue_creation_procedure::make_mac_ue_create_req()
{
  // Create MAC -> RLC adapters.
  du_logical_channel_context& srb0 = ue_ctx.bearers[LCID_SRB0];
  srb0.mac_rx_notifier = std::make_unique<mac_sdu_rx_adapter>(*srb0.rlc_bearer->get_rx_lower_layer_interface());
  srb0.mac_tx_notifier = std::make_unique<mac_sdu_tx_adapter>(*srb0.rlc_bearer->get_tx_lower_layer_interface());

  // Request MAC to create new UE.
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

void ue_creation_procedure::create_f1_ue()
{
  using namespace asn1::rrc_nr;

  f1ap_ue_create_request f1_msg{};
  f1_msg.ue_index   = ue_ctx.ue_index;
  f1_msg.c_rnti     = ue_ctx.rnti;
  f1_msg.cell_index = ue_ctx.pcell_index;

  // Pack SRB1 configuration that is going to be passed in the F1AP DU-to-CU-RRC-Container IE to the CU as per TS38.473,
  // Section 8.4.1.2.
  cell_group_cfg_s cell_group;
  cell_group.rlc_bearer_to_add_mod_list.resize(1);
  cell_group.rlc_bearer_to_add_mod_list[0].lc_ch_id        = 1;
  cell_group.rlc_bearer_to_add_mod_list[0].rlc_cfg_present = true;
  rlc_cfg_c::am_s_& am                                     = cell_group.rlc_bearer_to_add_mod_list[0].rlc_cfg.set_am();
  am.ul_am_rlc.sn_field_len_present                        = true;
  am.ul_am_rlc.sn_field_len.value                          = sn_field_len_am_opts::size12;
  am.ul_am_rlc.t_poll_retx.value                           = t_poll_retx_opts::ms45;
  am.ul_am_rlc.poll_pdu.value                              = poll_pdu_opts::infinity;
  am.ul_am_rlc.poll_byte.value                             = poll_byte_opts::infinity;
  am.ul_am_rlc.max_retx_thres.value                        = ul_am_rlc_s::max_retx_thres_opts::t8;
  am.dl_am_rlc.sn_field_len_present                        = true;
  am.dl_am_rlc.sn_field_len.value                          = sn_field_len_am_opts::size12;
  am.dl_am_rlc.t_reassembly.value                          = t_reassembly_opts::ms35;
  am.dl_am_rlc.t_status_prohibit.value                     = t_status_prohibit_opts::ms0;
  // TODO: Fill Remaining.

  asn1::bit_ref     bref{f1_msg.du_cu_rrc_container};
  asn1::SRSASN_CODE result = cell_group.pack(bref);
  srsgnb_assert(result == asn1::SRSASN_SUCCESS, "Failed to generate CellConfigGroup");

  f1_resp = cfg.f1ap_ue_ctx_mng->handle_ue_creation_request(f1_msg);
}
