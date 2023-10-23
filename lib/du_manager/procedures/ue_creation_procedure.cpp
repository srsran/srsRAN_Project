/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "ue_creation_procedure.h"
#include "../converters/asn1_rrc_config_helpers.h"
#include "../converters/scheduler_configuration_helpers.h"
#include "srsran/rlc/rlc_factory.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"

using namespace srsran;
using namespace srsran::srs_du;

namespace {

/// \brief Adapter used by the MAC and RLC to notify the DU manager of a Radio Link Failure.
class du_ue_rlf_notification_adapter final : public du_ue_rlf_handler
{
public:
  du_ue_rlf_notification_adapter(du_ue_index_t             ue_index_,
                                 std::chrono::milliseconds release_timeout_,
                                 unique_timer              release_request_timer,
                                 du_ue_manager_repository& du_ue_mng_) :
    ue_index(ue_index_),
    release_timeout(release_timeout_),
    rel_timer(std::move(release_request_timer)),
    du_ue_mng(du_ue_mng_),
    logger(srslog::fetch_basic_logger("DU-MNG"))
  {
  }

  // Called by DU manager to disconnect the adapter during UE removal.
  void disconnect() override
  {
    std::lock_guard<std::mutex> lock(timer_mutex);
    rel_timer.reset();
  }

  void on_rlf_detected() override { start_rlf_timer(rlf_cause::max_mac_kos_reached); }
  void on_crnti_ce_received() override
  {
    bool timer_was_running = false;
    {
      std::lock_guard<std::mutex> lock(timer_mutex);
      timer_was_running = rel_timer.is_running();
      rel_timer.stop();
    }
    if (timer_was_running) {
      logger.info("ue={}: RLF timer reset. Cause: C-RNTI CE was received for the UE", ue_index);
    }
  }

  void on_protocol_failure() override { start_rlf_timer(rlf_cause::rlc_protocol_failure); }
  void on_max_retx() override { start_rlf_timer(rlf_cause::max_rlc_retxs_reached); }

private:
  void start_rlf_timer(rlf_cause cause)
  {
    bool rlf_timer_start = false;
    {
      std::lock_guard<std::mutex> lock(timer_mutex);
      if (is_connected_nolock() and not rel_timer.is_running()) {
        rel_timer.set(release_timeout, [this, cause](timer_id_t tid) { trigger_ue_release(cause); });
        rel_timer.run();
        rlf_timer_start = true;
      }
    }
    if (rlf_timer_start) {
      logger.info("ue={}: RLF detected. Timer of {} msec to release UE started...", ue_index, release_timeout.count());
    }
  }

  void trigger_ue_release(rlf_cause cause)
  {
    // Trigger UE release in the upper layers.
    du_ue_mng.handle_rlf_ue_release(ue_index, cause);

    // Release timer so no new RLF is triggered for the same UE, after is scheduled for release.
    disconnect();

    logger.info("ue={}: RLF timer expired. Requesting a UE release...", ue_index);
  }

  bool is_connected_nolock() const { return rel_timer.is_valid(); }

  const du_ue_index_t             ue_index;
  const std::chrono::milliseconds release_timeout;
  unique_timer                    rel_timer;
  du_ue_manager_repository&       du_ue_mng;
  srslog::basic_logger&           logger;

  // This class is accessed directly from the MAC, so potential race conditions apply when accessing the \c rel_timer.
  std::mutex timer_mutex;
};

} // namespace

ue_creation_procedure::ue_creation_procedure(const du_ue_creation_request& req_,
                                             du_ue_manager_repository&     ue_mng_,
                                             const du_manager_params&      du_params_,
                                             du_ran_resource_manager&      cell_res_alloc_) :
  req(req_),
  ue_mng(ue_mng_),
  du_params(du_params_),
  du_res_alloc(cell_res_alloc_),
  proc_logger(srslog::fetch_basic_logger("DU-MNG"), name(), req.ue_index, req.tc_rnti)
{
}

void ue_creation_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  // > Check if UE context was created in the DU manager.
  ue_ctx = create_du_ue_context();
  if (ue_ctx == nullptr) {
    proc_logger.log_proc_failure("UE context not created because the RNTI is duplicated");
    CORO_AWAIT(clear_ue());
    CORO_EARLY_RETURN();
  }

  // > Initialize bearers and PHY/MAC PCell resources of the DU UE.
  if (not setup_du_ue_resources()) {
    CORO_AWAIT(clear_ue());
    CORO_EARLY_RETURN();
  }

  // > Create F1AP UE context.
  f1ap_resp = create_f1ap_ue();
  if (not f1ap_resp.result) {
    proc_logger.log_proc_failure("Failure to create F1AP UE context");
    CORO_AWAIT(clear_ue());
    CORO_EARLY_RETURN();
  }

  // > Initiate creation of RLC SRB0.
  create_rlc_srbs();

  // > Connect bearers between layers F1AP<->RLC<->MAC.
  connect_layer_bearers();

  // > Initiate MAC UE creation and await result.
  CORO_AWAIT_VALUE(mac_resp, create_mac_ue());
  if (mac_resp.allocated_crnti == INVALID_RNTI) {
    proc_logger.log_proc_failure("Failure to create MAC UE context");
    CORO_AWAIT(clear_ue());
    CORO_EARLY_RETURN();
  }

  // > Assign C-RNTI allocated by MAC.
  ue_mng.update_crnti(req.ue_index, mac_resp.allocated_crnti);

  // > Start Initial UL RRC Message Transfer by signalling MAC to notify CCCH to upper layers.
  if (not req.ul_ccch_msg.empty()) {
    if (not du_params.mac.ue_cfg.handle_ul_ccch_msg(ue_ctx->ue_index, req.ul_ccch_msg.copy())) {
      proc_logger.log_proc_failure("Failure to notify CCCH message to upper layers");
      CORO_AWAIT(clear_ue());
      CORO_EARLY_RETURN();
    }
  }

  proc_logger.log_proc_completed();
  CORO_RETURN();
}

du_ue* ue_creation_procedure::create_du_ue_context()
{
  // Create a DU UE resource manager, which will be responsible for managing bearer and PUCCH resources.
  ue_ran_resource_configurator ue_res = du_res_alloc.create_ue_resource_configurator(req.ue_index, req.pcell_index);
  if (ue_res.empty()) {
    return nullptr;
  }

  // Create the adapter used by the MAC and RLC to notify the DU manager of a Radio Link Failure.
  const du_cell_config& pcell_cfg = du_params.ran.cells[req.pcell_index];
  // Note: Between an RLF being detected and the UE being released, the DU manager will wait for enough time to allow
  // the UE to perform C-RNTI CE (t310) and RRC re-establishment (t311).
  std::chrono::milliseconds release_timeout =
      pcell_cfg.ue_timers_and_constants.t310 + pcell_cfg.ue_timers_and_constants.t311;
  auto rlf_notifier = std::make_unique<du_ue_rlf_notification_adapter>(
      req.ue_index,
      release_timeout,
      du_params.services.timers.create_unique_timer(du_params.services.du_mng_exec),
      ue_mng);

  // Create the DU UE context.
  return ue_mng.add_ue(
      std::make_unique<du_ue>(req.ue_index, req.pcell_index, req.tc_rnti, std::move(rlf_notifier), std::move(ue_res)));
}

async_task<void> ue_creation_procedure::clear_ue()
{
  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    if (f1ap_resp.result) {
      du_params.f1ap.ue_mng.handle_ue_deletion_request(req.ue_index);
    }

    if (mac_resp.allocated_crnti != INVALID_RNTI) {
      CORO_AWAIT(du_params.mac.ue_cfg.handle_ue_delete_request(
          mac_ue_delete_request{req.pcell_index, req.ue_index, mac_resp.allocated_crnti}));
    }

    if (ue_ctx != nullptr) {
      // Clear UE from DU Manager UE repository.
      ue_mng.remove_ue(ue_ctx->ue_index);
    }

    CORO_RETURN();
  });
}

bool ue_creation_procedure::setup_du_ue_resources()
{
  // > Verify that the UE resource configurator is correctly setup.
  if (ue_ctx->resources.empty()) {
    proc_logger.log_proc_failure("Unable to allocate RAN resources for UE");
    return false;
  }

  // Allocate PCell PHY/MAC resources for DU UE.
  f1ap_ue_context_update_request f1_req;
  f1_req.ue_index = ue_ctx->ue_index;
  f1_req.srbs_to_setup.resize(1);
  f1_req.srbs_to_setup[0]             = srb_id_t::srb1;
  du_ue_resource_update_response resp = ue_ctx->resources.update(req.pcell_index, f1_req);
  if (resp.release_required) {
    proc_logger.log_proc_failure("Unable to setup DU UE PCell and SRB resources");
    return false;
  }

  // Create DU UE SRB0 and SRB1.
  rlc_config tm_rlc_cfg{};
  tm_rlc_cfg.mode = rlc_mode::tm;
  ue_ctx->bearers.add_srb(srb_id_t::srb0, tm_rlc_cfg);
  ue_ctx->bearers.add_srb(srb_id_t::srb1, ue_ctx->resources->rlc_bearers[0].rlc_cfg);

  return true;
}

void ue_creation_procedure::create_rlc_srbs()
{
  // Create SRB0 RLC entity.
  du_ue_srb& srb0 = ue_ctx->bearers.srbs()[srb_id_t::srb0];
  srb0.rlc_bearer = create_rlc_entity(make_rlc_entity_creation_message(
      ue_ctx->ue_index, ue_ctx->pcell_index, srb0, du_params.services, *ue_ctx->rlf_notifier));

  // Create SRB1 RLC entity.
  du_ue_srb& srb1 = ue_ctx->bearers.srbs()[srb_id_t::srb1];
  srb1.rlc_bearer = create_rlc_entity(make_rlc_entity_creation_message(
      ue_ctx->ue_index, ue_ctx->pcell_index, srb1, du_params.services, *ue_ctx->rlf_notifier));
}

async_task<mac_ue_create_response> ue_creation_procedure::create_mac_ue()
{
  // Create Request to MAC to create new UE.
  mac_ue_create_request mac_ue_create_msg{};
  mac_ue_create_msg.ue_index           = ue_ctx->ue_index;
  mac_ue_create_msg.crnti              = req.tc_rnti;
  mac_ue_create_msg.cell_index         = req.pcell_index;
  mac_ue_create_msg.mac_cell_group_cfg = ue_ctx->resources->mcg_cfg;
  mac_ue_create_msg.phy_cell_group_cfg = ue_ctx->resources->pcg_cfg;
  mac_ue_create_msg.rlf_notifier       = ue_ctx->rlf_notifier.get();
  for (du_ue_srb& bearer : ue_ctx->bearers.srbs()) {
    mac_ue_create_msg.bearers.emplace_back();
    mac_logical_channel_config& lc = mac_ue_create_msg.bearers.back();
    lc.lcid                        = bearer.lcid();
    lc.ul_bearer                   = &bearer.connector.mac_rx_sdu_notifier;
    lc.dl_bearer                   = &bearer.connector.mac_tx_sdu_notifier;
  }
  for (auto& bearer : ue_ctx->bearers.drbs()) {
    mac_ue_create_msg.bearers.emplace_back();
    mac_logical_channel_config& lc = mac_ue_create_msg.bearers.back();
    lc.lcid                        = bearer.second->lcid;
    lc.ul_bearer                   = &bearer.second->connector.mac_rx_sdu_notifier;
    lc.dl_bearer                   = &bearer.second->connector.mac_tx_sdu_notifier;
  }
  mac_ue_create_msg.ul_ccch_msg = not req.ul_ccch_msg.empty() ? &req.ul_ccch_msg : nullptr;

  // Create Scheduler UE Config Request that will be embedded in the mac UE creation request.
  mac_ue_create_msg.sched_cfg = create_scheduler_ue_config_request(*ue_ctx);

  // Request MAC to create new UE.
  return du_params.mac.ue_cfg.handle_ue_create_request(mac_ue_create_msg);
}

f1ap_ue_creation_response ue_creation_procedure::create_f1ap_ue()
{
  using namespace asn1::rrc_nr;

  f1ap_ue_creation_request f1ap_msg{};
  f1ap_msg.ue_index    = ue_ctx->ue_index;
  f1ap_msg.c_rnti      = ue_ctx->rnti;
  f1ap_msg.pcell_index = ue_ctx->pcell_index;
  f1ap_msg.f1c_bearers_to_add.resize(2);

  // Create SRB0 and SRB1.
  du_ue_srb& srb0                                = ue_ctx->bearers.srbs()[srb_id_t::srb0];
  f1ap_msg.f1c_bearers_to_add[0].srb_id          = srb_id_t::srb0;
  f1ap_msg.f1c_bearers_to_add[0].rx_sdu_notifier = &srb0.connector.f1c_rx_sdu_notif;
  du_ue_srb& srb1                                = ue_ctx->bearers.srbs()[srb_id_t::srb1];
  f1ap_msg.f1c_bearers_to_add[1].srb_id          = srb_id_t::srb1;
  f1ap_msg.f1c_bearers_to_add[1].rx_sdu_notifier = &srb1.connector.f1c_rx_sdu_notif;

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
    asn1::bit_ref     bref{f1ap_msg.du_cu_rrc_container};
    asn1::SRSASN_CODE result = cell_group.pack(bref);
    srsran_assert(result == asn1::SRSASN_SUCCESS, "Failed to generate CellConfigGroup");
  }

  return du_params.f1ap.ue_mng.handle_ue_creation_request(f1ap_msg);
}

void ue_creation_procedure::connect_layer_bearers()
{
  // Connect SRB0 bearer layers.
  du_ue_srb& srb0 = ue_ctx->bearers.srbs()[srb_id_t::srb0];
  srb0.connector.connect(ue_ctx->ue_index,
                         srb_id_t::srb0,
                         *f1ap_resp.f1c_bearers_added[0],
                         *srb0.rlc_bearer,
                         du_params.rlc.mac_ue_info_handler);

  // Connect SRB1 bearer layers.
  du_ue_srb& srb1 = ue_ctx->bearers.srbs()[srb_id_t::srb1];
  srb1.connector.connect(ue_ctx->ue_index,
                         srb_id_t::srb1,
                         *f1ap_resp.f1c_bearers_added[1],
                         *srb1.rlc_bearer,
                         du_params.rlc.mac_ue_info_handler);
}
