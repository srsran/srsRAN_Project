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
#include "../converters/asn1_cell_group_config_helpers.h"
#include "srsgnb/mac/config/mac_cell_group_config_factory.h"
#include "srsgnb/scheduler/config/logical_channel_config_factory.h"
#include "srsgnb/scheduler/config/serving_cell_config_factory.h"

using namespace srsgnb;
using namespace srsgnb::srs_du;

ue_creation_procedure::ue_creation_procedure(du_ue_index_t                                ue_index,
                                             const ul_ccch_indication_message&            ccch_ind_msg,
                                             ue_manager_ctrl_configurator&                ue_mng_,
                                             const du_manager_params::service_params&     du_services_,
                                             const du_manager_params::mac_config_params&  mac_mng_,
                                             const du_manager_params::rlc_config_params&  rlc_params_,
                                             const du_manager_params::f1ap_config_params& f1ap_mng_,
                                             du_ue_resource_configurator_factory&         cell_res_alloc_) :
  msg(ccch_ind_msg),
  ue_mng(ue_mng_),
  services(du_services_),
  mac_mng(mac_mng_),
  rlc_cfg(rlc_params_),
  f1ap_mng(f1ap_mng_),
  du_res_alloc(cell_res_alloc_),
  logger(srslog::fetch_basic_logger("DU-MNG")),
  ue_ctx(std::make_unique<du_ue>(ue_index,
                                 ccch_ind_msg.cell_index,
                                 ccch_ind_msg.crnti,
                                 du_res_alloc.create_ue_resource_configurator(ue_index, msg.cell_index)))
{
}

void ue_creation_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  log_proc_started(logger, ue_ctx->ue_index, msg.crnti, "UE Create");

  // > Initialize bearers and PHY/MAC PCell resources of the DU UE.
  if (not setup_du_ue_resources()) {
    log_proc_failure(logger, MAX_NOF_DU_UES, msg.crnti, name(), "Failure to allocate DU UE.");
    CORO_EARLY_RETURN();
  }

  // > Initiate creation of F1 UE context and await result.
  create_f1_ue();
  if (not f1_resp.result) {
    log_proc_failure(logger, ue_ctx->ue_index, msg.crnti, name(), "UE failed to be created in F1AP.");
    clear_ue();
    CORO_EARLY_RETURN();
  }

  // > Initiate creation of RLC SRB0.
  create_rlc_srbs();

  // > Connect bearers between layers F1AP<->RLC<->MAC.
  connect_layer_bearers();

  // > Initiate MAC UE creation and await result.
  CORO_AWAIT_VALUE(mac_resp, make_mac_ue_create_req());
  if (not mac_resp.result) {
    log_proc_failure(logger, ue_ctx->ue_index, msg.crnti, "UE failed to be created in MAC.");
    clear_ue();
    CORO_EARLY_RETURN();
  }

  // > Start UE activity timer.
  ue_ctx->activity_timer.run();

  // > Create UE context in DU manager.
  du_ue_index_t ue_index = ue_ctx->ue_index;
  if (ue_mng.add_ue(std::move(ue_ctx)) == nullptr) {
    log_proc_failure(logger, ue_index, msg.crnti, "UE failed to be created in DU manager.");
    clear_ue();
    CORO_EARLY_RETURN();
  }

  // > Start Initial UL RRC Message Transfer by signalling MAC to notify CCCH to upper layers.
  if (not msg.subpdu.empty()) {
    mac_mng.ue_cfg.handle_ul_ccch_msg(ue_index, std::move(msg.subpdu));
  }

  log_proc_completed(logger, ue_index, msg.crnti, "UE Create");
  CORO_RETURN();
}

bool ue_creation_procedure::setup_du_ue_resources()
{
  // > Verify if UE-Id was successfully allocated.
  if (ue_ctx->ue_index >= INVALID_DU_UE_INDEX) {
    log_proc_failure(logger, INVALID_DU_UE_INDEX, msg.crnti, name(), "Failure to allocate DU UE Index.");
    return false;
  }

  // > Verify if RNTI is not duplicate.
  if (ue_mng.find_rnti(ue_ctx->rnti) != nullptr) {
    log_proc_failure(logger, INVALID_DU_UE_INDEX, msg.crnti, name(), "Repeated RNTI.");
    return false;
  }

  // > Verify that the UE resource configurator is correctly setup.
  if (ue_ctx->resources.empty()) {
    log_proc_failure(logger, INVALID_DU_UE_INDEX, msg.crnti, name(), "Unable to allocate UE Resource Config instance.");
    return false;
  }

  // Allocate PCell PHY/MAC resources for DU UE.
  f1ap_ue_context_update_request req;
  req.ue_index = ue_ctx->ue_index;
  req.srbs_to_setup.resize(1);
  req.srbs_to_setup[0]                    = srb_id_t::srb1;
  du_ue_ran_resource_update_response resp = ue_ctx->resources.update(msg.cell_index, req);
  if (resp.release_required) {
    log_proc_failure(logger, MAX_NOF_DU_UES, msg.crnti, name(), "Failure to setup DU UE PCell and SRB resources.");
    return false;
  }

  // Create DU UE SRB0 and SRB1.
  rlc_config tm_rlc_cfg{};
  tm_rlc_cfg.mode = rlc_mode::tm;
  ue_ctx->bearers.add_srb(srb_id_t::srb0, tm_rlc_cfg);
  ue_ctx->bearers.add_srb(srb_id_t::srb1, ue_ctx->resources->rlc_bearers[0].rlc_cfg);

  const static unsigned UE_ACTIVITY_TIMEOUT = 500; // TODO: Parametrize.
  ue_ctx->activity_timer                    = services.timers.create_unique_timer();
  ue_ctx->activity_timer.set(UE_ACTIVITY_TIMEOUT, [ue_index = ue_ctx->ue_index, &logger = this->logger](unsigned tid) {
    logger.info("UE Manager: UE={} activity timeout.", ue_index);
    // TODO: Handle.
  });

  return true;
}

void ue_creation_procedure::create_rlc_srbs()
{
  // Create SRB0 RLC entity.
  du_ue_srb& srb0 = ue_ctx->bearers.srbs()[srb_id_t::srb0];
  srb0.rlc_bearer =
      create_rlc_entity(make_rlc_entity_creation_message(ue_ctx->ue_index, ue_ctx->pcell_index, srb0, services));

  // Create SRB1 RLC entity.
  du_ue_srb& srb1 = ue_ctx->bearers.srbs()[srb_id_t::srb1];
  srb1.rlc_bearer =
      create_rlc_entity(make_rlc_entity_creation_message(ue_ctx->ue_index, ue_ctx->pcell_index, srb1, services));
}

async_task<mac_ue_create_response_message> ue_creation_procedure::make_mac_ue_create_req()
{
  // Request MAC to create new UE.
  mac_ue_create_request_message mac_ue_create_msg{};
  mac_ue_create_msg.ue_index           = ue_ctx->ue_index;
  mac_ue_create_msg.crnti              = msg.crnti;
  mac_ue_create_msg.cell_index         = msg.cell_index;
  mac_ue_create_msg.serv_cell_cfg      = ue_ctx->resources->spcell_cfg.spcell_cfg_ded;
  mac_ue_create_msg.mac_cell_group_cfg = ue_ctx->resources->mcg_cfg;
  mac_ue_create_msg.phy_cell_group_cfg = ue_ctx->resources->pcg_cfg;
  for (du_ue_srb& bearer : ue_ctx->bearers.srbs()) {
    mac_ue_create_msg.bearers.emplace_back();
    auto& lc     = mac_ue_create_msg.bearers.back();
    lc.lcid      = bearer.lcid();
    lc.ul_bearer = &bearer.connector.mac_rx_sdu_notifier;
    lc.dl_bearer = &bearer.connector.mac_tx_sdu_notifier;
    // Add MAC-LogicalChannel.
    lc.lc_config = config_helpers::make_default_logical_channel_config(lc.lcid);
    lc.lc_config.sr_id.emplace(mac_ue_create_msg.mac_cell_group_cfg.scheduling_request_config.back().sr_id);
  }
  mac_ue_create_msg.ul_ccch_msg       = &msg.subpdu;
  mac_ue_create_msg.ue_activity_timer = &ue_ctx->activity_timer;

  return mac_mng.ue_cfg.handle_ue_create_request(mac_ue_create_msg);
}

void ue_creation_procedure::create_f1_ue()
{
  using namespace asn1::rrc_nr;

  f1ap_ue_creation_request f1_msg{};
  f1_msg.ue_index    = ue_ctx->ue_index;
  f1_msg.c_rnti      = ue_ctx->rnti;
  f1_msg.pcell_index = ue_ctx->pcell_index;
  f1_msg.f1c_bearers_to_add.resize(2);

  // Create SRB0 and SRB1.
  du_ue_srb& srb0                              = ue_ctx->bearers.srbs()[srb_id_t::srb0];
  f1_msg.f1c_bearers_to_add[0].srb_id          = srb_id_t::srb0;
  f1_msg.f1c_bearers_to_add[0].rx_sdu_notifier = &srb0.connector.f1c_rx_sdu_notif;
  du_ue_srb& srb1                              = ue_ctx->bearers.srbs()[srb_id_t::srb1];
  f1_msg.f1c_bearers_to_add[1].srb_id          = srb_id_t::srb1;
  f1_msg.f1c_bearers_to_add[1].rx_sdu_notifier = &srb1.connector.f1c_rx_sdu_notif;

  // Pack SRB1 configuration that is going to be passed in the F1AP DU-to-CU-RRC-Container IE to the CU as per TS38.473,
  // Section 8.4.1.2.
  cell_group_cfg_s cell_group;
  calculate_cell_group_config_diff(cell_group, {}, *ue_ctx->resources);
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

  {
    asn1::bit_ref     bref{f1_msg.du_cu_rrc_container};
    asn1::SRSASN_CODE result = cell_group.pack(bref);
    srsgnb_assert(result == asn1::SRSASN_SUCCESS, "Failed to generate CellConfigGroup");
  }

  f1_resp = f1ap_mng.ue_mng.handle_ue_creation_request(f1_msg);
}

void ue_creation_procedure::connect_layer_bearers()
{
  // Connect SRB0 bearer layers.
  du_ue_srb& srb0 = ue_ctx->bearers.srbs()[srb_id_t::srb0];
  srb0.connector.connect(
      ue_ctx->ue_index, srb_id_t::srb0, *f1_resp.f1c_bearers_added[0], *srb0.rlc_bearer, rlc_cfg.mac_ue_info_handler);

  // Connect SRB1 bearer layers.
  du_ue_srb& srb1 = ue_ctx->bearers.srbs()[srb_id_t::srb1];
  srb1.connector.connect(
      ue_ctx->ue_index, srb_id_t::srb1, *f1_resp.f1c_bearers_added[1], *srb1.rlc_bearer, rlc_cfg.mac_ue_info_handler);
}
