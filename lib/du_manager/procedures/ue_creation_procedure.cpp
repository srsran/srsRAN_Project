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
#include "../converters/asn1_cell_group_config_helpers.h"
#include "../du_ue/cell_group_config.h"
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
                                             const du_manager_params::f1ap_config_params& f1ap_mng_) :
  msg(ccch_ind_msg),
  ue_mng(ue_mng_),
  services(du_services_),
  mac_mng(mac_mng_),
  rlc_cfg(rlc_params_),
  f1ap_mng(f1ap_mng_),
  logger(srslog::fetch_basic_logger("DU-MNG")),
  ue_ctx(std::make_unique<du_ue>(ue_index, ccch_ind_msg.cell_index, ccch_ind_msg.crnti))
{
  ue_ctx->bearers.emplace(LCID_SRB0);
  ue_ctx->bearers[LCID_SRB0].lcid = LCID_SRB0;
  ue_ctx->bearers.emplace(LCID_SRB1);
  ue_ctx->bearers[LCID_SRB1].lcid = LCID_SRB1;

  ue_ctx->cells.resize(1);
  cell_group_config& pcell = ue_ctx->cells[0];
  pcell.rlc_bearers.push_back(make_default_srb(LCID_SRB1));
  pcell.spcell_cfg.serv_cell_idx  = msg.cell_index;
  pcell.spcell_cfg.spcell_cfg_ded = config_helpers::make_default_initial_ue_serving_cell_config();
  pcell.mcg_cfg                   = config_helpers::make_initial_mac_cell_group_config();

  const static unsigned UE_ACTIVITY_TIMEOUT = 500; // TODO: Parametrize.
  ue_ctx->activity_timer                    = services.timers.create_unique_timer();
  ue_ctx->activity_timer.set(UE_ACTIVITY_TIMEOUT, [ue_index, &logger = this->logger](unsigned tid) {
    logger.info("UE Manager: UE={} activity timeout.", ue_index);
    // TODO: Handle.
  });

  // TODO: Move to helper.
  physical_cell_group_config& pcg_cfg = pcell.pcg_cfg;
  pcg_cfg.p_nr_fr1                    = 10;
  pcg_cfg.pdsch_harq_codebook         = pdsch_harq_ack_codebook::dynamic;
}

void ue_creation_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  log_proc_started(logger, ue_ctx->ue_index, msg.crnti, "UE Create");

  // > Verify if UE index was successfully allocated and params are valid.
  if (ue_ctx->ue_index == MAX_NOF_DU_UES) {
    log_proc_failure(logger, MAX_NOF_DU_UES, msg.crnti, name(), "Failure to allocate DU UE index.");
    CORO_EARLY_RETURN();
  }
  if (ue_mng.find_rnti(ue_ctx->rnti) != nullptr) {
    log_proc_failure(logger, MAX_NOF_DU_UES, msg.crnti, name(), "Repeated RNTI.");
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

void ue_creation_procedure::create_rlc_srbs()
{
  // Create SRB0 RLC entity.
  du_bearer&                  srb0 = ue_ctx->bearers[LCID_SRB0];
  rlc_entity_creation_message rlc_msg{};
  rlc_msg.ue_index       = ue_ctx->ue_index;
  rlc_msg.lcid           = LCID_SRB0;
  rlc_msg.rx_upper_dn    = &srb0.bearer_connector.rlc_rx_sdu_notif;
  rlc_msg.tx_upper_dn    = &srb0.bearer_connector.rlc_tx_data_notif;
  rlc_msg.tx_upper_cn    = &srb0.bearer_connector.rlc_tx_ctrl_notif;
  rlc_msg.tx_lower_dn    = &srb0.bearer_connector.rlc_tx_buffer_state_notif;
  rlc_msg.config.mode    = rlc_mode::tm;
  rlc_msg.timers         = &services.timers;
  rlc_msg.pcell_executor = &services.cell_execs.executor(ue_ctx->pcell_index);
  rlc_msg.ue_executor    = &services.ue_execs.executor(ue_ctx->ue_index);
  srb0.rlc_bearer        = create_rlc_entity(rlc_msg);

  // Create SRB1 RLC entity.
  du_bearer& srb1        = ue_ctx->bearers[LCID_SRB1];
  rlc_msg.lcid           = LCID_SRB1;
  rlc_msg.rx_upper_dn    = &srb1.bearer_connector.rlc_rx_sdu_notif;
  rlc_msg.tx_upper_dn    = &srb1.bearer_connector.rlc_tx_data_notif;
  rlc_msg.tx_upper_cn    = &srb1.bearer_connector.rlc_tx_ctrl_notif;
  rlc_msg.tx_lower_dn    = &srb1.bearer_connector.rlc_tx_buffer_state_notif;
  rlc_msg.config         = ue_ctx->cells[0].rlc_bearers[0].rlc_cfg;
  rlc_msg.timers         = &services.timers;
  rlc_msg.pcell_executor = &services.cell_execs.executor(ue_ctx->pcell_index);
  rlc_msg.ue_executor    = &services.ue_execs.executor(ue_ctx->ue_index);
  srb1.rlc_bearer        = create_rlc_entity(rlc_msg);
}

async_task<mac_ue_create_response_message> ue_creation_procedure::make_mac_ue_create_req()
{
  // Request MAC to create new UE.
  mac_ue_create_request_message mac_ue_create_msg{};
  mac_ue_create_msg.ue_index           = ue_ctx->ue_index;
  mac_ue_create_msg.crnti              = msg.crnti;
  mac_ue_create_msg.cell_index         = msg.cell_index;
  mac_ue_create_msg.serv_cell_cfg      = ue_ctx->cells[ue_ctx->pcell_index].spcell_cfg.spcell_cfg_ded;
  mac_ue_create_msg.mac_cell_group_cfg = ue_ctx->cells[ue_ctx->pcell_index].mcg_cfg;
  mac_ue_create_msg.phy_cell_group_cfg = ue_ctx->cells[ue_ctx->pcell_index].pcg_cfg;
  for (du_bearer& bearer : ue_ctx->bearers) {
    mac_ue_create_msg.bearers.emplace_back();
    auto& lc     = mac_ue_create_msg.bearers.back();
    lc.lcid      = bearer.lcid;
    lc.ul_bearer = &bearer.bearer_connector.mac_rx_notif;
    lc.dl_bearer = &bearer.bearer_connector.mac_tx_notif;
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

  f1ap_ue_create_request f1_msg{};
  f1_msg.ue_index   = ue_ctx->ue_index;
  f1_msg.c_rnti     = ue_ctx->rnti;
  f1_msg.cell_index = ue_ctx->pcell_index;
  f1_msg.srbs_to_add.resize(2);

  // Create SRB0 and SRB1.
  du_bearer& srb0                       = ue_ctx->bearers[LCID_SRB0];
  f1_msg.srbs_to_add[0].srb_id          = srb_id_t::srb0;
  f1_msg.srbs_to_add[0].f1_tx_pdu_notif = &srb0.bearer_connector.f1_tx_pdu_notif;
  du_bearer& srb1                       = ue_ctx->bearers[LCID_SRB1];
  f1_msg.srbs_to_add[1].srb_id          = srb_id_t::srb1;
  f1_msg.srbs_to_add[1].f1_tx_pdu_notif = &srb1.bearer_connector.f1_tx_pdu_notif;

  // Pack SRB1 configuration that is going to be passed in the F1AP DU-to-CU-RRC-Container IE to the CU as per TS38.473,
  // Section 8.4.1.2.
  cell_group_cfg_s cell_group;
  calculate_cell_group_config_diff(cell_group, {}, ue_ctx->cells[0]);
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
  du_bearer& srb0 = ue_ctx->bearers[LCID_SRB0];
  du_bearer& srb1 = ue_ctx->bearers[LCID_SRB1];

  // Connect F1 Tx PDU -> RLC Tx SDU.
  srb0.bearer_connector.f1_tx_pdu_notif.connect(*srb0.rlc_bearer->get_tx_upper_layer_data_interface());
  srb1.bearer_connector.f1_tx_pdu_notif.connect(*srb1.rlc_bearer->get_tx_upper_layer_data_interface());

  // Connect RLC RX SDU -> F1AP RX PDU adapter.
  srb0.bearer_connector.rlc_rx_sdu_notif.connect(*f1_resp.bearers_added[0]);
  srb1.bearer_connector.rlc_rx_sdu_notif.connect(*f1_resp.bearers_added[1]);

  // Connect RLC BSR update notifier -> MAC Control Info Handler.
  srb0.bearer_connector.rlc_tx_buffer_state_notif.connect(ue_ctx->ue_index, LCID_SRB0, rlc_cfg.mac_ue_info_handler);
  srb1.bearer_connector.rlc_tx_buffer_state_notif.connect(ue_ctx->ue_index, LCID_SRB1, rlc_cfg.mac_ue_info_handler);

  // Connect MAC Rx SDU notifier -> RLC Rx PDU.
  srb0.bearer_connector.mac_rx_notif.connect(*srb0.rlc_bearer->get_rx_lower_layer_interface());
  srb1.bearer_connector.mac_rx_notif.connect(*srb1.rlc_bearer->get_rx_lower_layer_interface());

  // Connect MAC Tx SDU builder -> RLC Tx PDU builder.
  srb0.bearer_connector.mac_tx_notif.connect(*srb0.rlc_bearer->get_tx_lower_layer_interface());
  srb1.bearer_connector.mac_tx_notif.connect(*srb1.rlc_bearer->get_tx_lower_layer_interface());
}
