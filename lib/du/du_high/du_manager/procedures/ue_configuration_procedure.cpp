/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "ue_configuration_procedure.h"
#include "../converters/asn1_rrc_config_helpers.h"
#include "../converters/scheduler_configuration_helpers.h"
#include "srsran/mac/mac_ue_configurator.h"
#include "srsran/rlc/rlc_factory.h"
#include <algorithm>

using namespace srsran;
using namespace srs_du;

ue_configuration_procedure::ue_configuration_procedure(const f1ap_ue_context_update_request& request_,
                                                       du_ue_manager_repository&             ue_mng_,
                                                       const du_manager_params&              du_params_) :
  request(request_),
  ue_mng(ue_mng_),
  du_params(du_params_),
  ue(ue_mng.find_ue(request.ue_index)),
  proc_logger(logger, name(), request.ue_index, ue != nullptr ? ue->rnti : rnti_t::INVALID_RNTI)
{
  srsran_assert(ue != nullptr, "ueId={} not found", fmt::underlying(request.ue_index));
}

void ue_configuration_procedure::operator()(coro_context<async_task<f1ap_ue_context_update_response>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  // > Flush any buffering F1-U bearers when HO is finalized.
  // > We do this before checking for changes in the config, as the RRC reconfiguration complete indicator
  // > may not change the configuration.
  if (request.rrc_recfg_complete_ind) {
    handle_rrc_reconfiguration_complete_ind();
  }

  if (not changed_detected()) {
    // Nothing to do (e.g. No SCells, DRBs or SRBs to setup or release)
    proc_logger.log_proc_completed();
    CORO_EARLY_RETURN(make_empty_ue_config_response());
  }

  prev_ue_res_cfg = ue->resources.value();
  ue_res_cfg_resp = ue->resources.update(
      ue->pcell_index, request, ue->reestablished_cfg_pending.get(), ue->reestablished_ue_caps_summary.get());
  if (ue_res_cfg_resp.failed()) {
    proc_logger.log_proc_failure("Failed to allocate DU UE resources");
    CORO_EARLY_RETURN(make_ue_config_failure());
  }

  // > Stop traffic in the DRBs that need to be removed.
  CORO_AWAIT(stop_drbs_to_rem());

  // > Update DU UE bearers.
  update_ue_context();

  // > Update MAC bearers.
  CORO_AWAIT_VALUE(mac_res, update_mac_and_sched());

  // > Destroy old DU UE bearers that are now detached from remaining layers.
  clear_old_ue_context();

  proc_logger.log_proc_completed();

  CORO_RETURN(mac_res.result ? make_ue_config_response() : make_ue_config_failure());
}

async_task<void> ue_configuration_procedure::stop_drbs_to_rem()
{
  // Request traffic to stop for DRBs that are going to be removed.
  return ue->handle_drb_stop_request(request.drbs_to_rem);
}

void ue_configuration_procedure::update_ue_context()
{
  // > Create DU UE SRB objects.
  for (const auto& srb_cfg : ue->resources->srbs) {
    srb_id_t srbid = srb_cfg.srb_id;
    if (ue->bearers.srbs().contains(srbid)) {
      // >> In case the SRB already exists, we ignore the request for its reconfiguration.
      continue;
    }
    srbs_added.push_back(srbid);

    // >> Create SRB bearer.
    du_ue_srb& srb = ue->bearers.add_srb(srbid);

    // >> Create RLC SRB entity.
    srb.rlc_bearer = create_rlc_entity(make_rlc_entity_creation_message(du_params.ran.gnb_du_id,
                                                                        ue->ue_index,
                                                                        ue->pcell_index,
                                                                        srb,
                                                                        srb_cfg.rlc_cfg,
                                                                        du_params.services,
                                                                        ue->get_rlc_rlf_notifier(),
                                                                        du_params.rlc.pcap_writer));
  }

  // > Create F1-C bearers.
  f1ap_ue_configuration_request req{};
  req.ue_index = ue->ue_index;
  for (const srb_id_t srb_id : srbs_added) {
    du_ue_srb& bearer = ue->bearers.srbs()[srb_id];
    req.f1c_bearers_to_add.emplace_back();
    req.f1c_bearers_to_add.back().srb_id          = srb_id;
    req.f1c_bearers_to_add.back().rx_sdu_notifier = &bearer.connector.f1c_rx_sdu_notif;
  }
  f1ap_ue_configuration_response resp = du_params.f1ap.ue_mng.handle_ue_configuration_request(req);

  // > Connect newly created F1-C bearers with RLC SRBs, and RLC SRBs with MAC logical channel notifiers.
  for (f1c_bearer_addmodded& bearer_added : resp.f1c_bearers_added) {
    du_ue_srb& srb = ue->bearers.srbs()[bearer_added.srb_id];
    srb.connector.connect(
        req.ue_index, bearer_added.srb_id, *bearer_added.bearer, *srb.rlc_bearer, du_params.rlc.mac_ue_info_handler);
  }

  // > Move DU UE DRBs to be removed out of the UE bearer manager.
  // Note: This DRB pointer will remain valid and accessible from other layers until we update the latter.
  for (const drb_id_t& drb_to_rem : request.drbs_to_rem) {
    if (ue->bearers.drbs().count(drb_to_rem) == 0) {
      proc_logger.log_proc_warning("Failed to release {}. Cause: DRB entity does not exist", drb_to_rem);
      continue;
    }
    drbs_to_rem.push_back(ue->bearers.remove_drb(drb_to_rem));
  }

  // > Create new DU UE DRB objects.
  auto& failed_drbs = ue_res_cfg_resp.failed_drbs;
  for (const f1ap_drb_to_setup& drbtoadd : request.drbs_to_setup) {
    if (std::find(failed_drbs.begin(), failed_drbs.end(), drbtoadd.drb_id) != failed_drbs.end()) {
      // >> In case it was not possible to setup DRB in the UE resources, we continue to the next DRB.
      continue;
    }
    if (ue->bearers.drbs().count(drbtoadd.drb_id) > 0) {
      proc_logger.log_proc_warning("Failed to setup {}. Cause: DRB setup for an already existing DRB.",
                                   drbtoadd.drb_id);
      failed_drbs.push_back(drbtoadd.drb_id);
      continue;
    }

    // Find the configurations for this DRB.
    const du_ue_drb_config& drb_cfg = ue->resources->drbs[drbtoadd.drb_id];

    // Create DU DRB instance.
    std::unique_ptr<du_ue_drb> drb = create_drb(drb_creation_info{ue->ue_index,
                                                                  ue->pcell_index,
                                                                  drbtoadd.drb_id,
                                                                  drb_cfg.lcid,
                                                                  drb_cfg.qos.qos_desc.get_5qi(),
                                                                  drb_cfg.rlc_cfg,
                                                                  drb_cfg.f1u,
                                                                  not request.ho_prep_info.empty(),
                                                                  drbtoadd.uluptnl_info_list,
                                                                  ue_mng.get_f1u_teid_pool(),
                                                                  du_params,
                                                                  ue->get_rlc_rlf_notifier()});
    if (drb == nullptr) {
      failed_drbs.push_back(drbtoadd.drb_id);
      proc_logger.log_proc_warning("Failed to create {}. Cause: Failed to allocate DU UE resources.", drbtoadd.drb_id);
      continue;
    }
    ue->bearers.add_drb(std::move(drb));
  }

  // > Modify existing UE DRBs.
  for (const f1ap_drb_to_modify& drbtomod : request.drbs_to_mod) {
    if (std::find(failed_drbs.begin(), failed_drbs.end(), drbtomod.drb_id) != failed_drbs.end()) {
      // >> Failed to modify DRB, continue to next DRB.
      continue;
    }

    // Find the RLC configuration for this DRB.
    const du_ue_drb_config& drb_cfg = ue->resources->drbs[drbtomod.drb_id];

    auto drb_it = ue->bearers.drbs().find(drbtomod.drb_id);
    if (drb_it == ue->bearers.drbs().end()) {
      // >> It's a DRB modification after RRC Reestablishment. We need to create a new DRB instance.

      // Create DU DRB instance.
      std::unique_ptr<du_ue_drb> drb = create_drb(drb_creation_info{ue->ue_index,
                                                                    ue->pcell_index,
                                                                    drbtomod.drb_id,
                                                                    drb_cfg.lcid,
                                                                    drb_cfg.qos.qos_desc.get_5qi(),
                                                                    drb_cfg.rlc_cfg,
                                                                    drb_cfg.f1u,
                                                                    false,
                                                                    drbtomod.uluptnl_info_list,
                                                                    ue_mng.get_f1u_teid_pool(),
                                                                    du_params,
                                                                    ue->get_rlc_rlf_notifier()});
      if (drb == nullptr) {
        proc_logger.log_proc_warning("Failed to create {}. Cause: Failed to allocate DU UE resources.",
                                     drbtomod.drb_id);
        failed_drbs.push_back(drbtomod.drb_id);
        continue;
      }
      ue->bearers.add_drb(std::move(drb));
    } else {
      // TODO: Support existing DRB entity modifications.
    }
  }
}

void ue_configuration_procedure::clear_old_ue_context()
{
  if (not drbs_to_rem.empty()) {
    // Dispatch DRB context destruction to the respective UE executor.
    task_executor& exec = du_params.services.ue_execs.ctrl_executor(ue->ue_index);
    if (not exec.defer([drbs = std::move(drbs_to_rem)]() mutable { drbs.clear(); })) {
      logger.warning("ue={}: Could not dispatch DRB removal task to UE executor. Destroying it the main DU manager "
                     "execution context",
                     fmt::underlying(ue->ue_index));
    }
  }
}

async_task<mac_ue_reconfiguration_response> ue_configuration_procedure::update_mac_and_sched()
{
  // Create Request to MAC to reconfigure existing UE.
  mac_ue_reconfiguration_request mac_ue_reconf_req;
  mac_ue_reconf_req.ue_index           = request.ue_index;
  mac_ue_reconf_req.crnti              = ue->rnti;
  mac_ue_reconf_req.pcell_index        = ue->pcell_index;
  mac_ue_reconf_req.mac_cell_group_cfg = ue->resources->cell_group.mcg_cfg;
  mac_ue_reconf_req.phy_cell_group_cfg = ue->resources->cell_group.pcg_cfg;

  for (const srb_id_t srbid : srbs_added) {
    du_ue_srb& bearer = ue->bearers.srbs()[srbid];
    mac_ue_reconf_req.bearers_to_addmod.emplace_back();
    auto& lc_ch     = mac_ue_reconf_req.bearers_to_addmod.back();
    lc_ch.lcid      = bearer.lcid();
    lc_ch.ul_bearer = &bearer.connector.mac_rx_sdu_notifier;
    lc_ch.dl_bearer = &bearer.connector.mac_tx_sdu_notifier;
  }

  for (const std::unique_ptr<du_ue_drb>& drb : drbs_to_rem) {
    mac_ue_reconf_req.bearers_to_rem.push_back(drb->lcid);
  }

  for (const auto& drb : request.drbs_to_setup) {
    if (ue->bearers.drbs().count(drb.drb_id) == 0) {
      // The DRB failed to be setup. Carry on with other DRBs.
      continue;
    }
    du_ue_drb& bearer = *ue->bearers.drbs().at(drb.drb_id);
    mac_ue_reconf_req.bearers_to_addmod.emplace_back();
    auto& lc_ch     = mac_ue_reconf_req.bearers_to_addmod.back();
    lc_ch.lcid      = bearer.lcid;
    lc_ch.ul_bearer = &bearer.connector.mac_rx_sdu_notifier;
    lc_ch.dl_bearer = &bearer.connector.mac_tx_sdu_notifier;
  }
  auto& failed_drbs = ue_res_cfg_resp.failed_drbs;
  for (const auto& drb : request.drbs_to_mod) {
    if (std::find(failed_drbs.begin(), failed_drbs.end(), drb.drb_id) != failed_drbs.end()) {
      // The DRB failed to be modified. Carry on with other DRBs.
      continue;
    }
    du_ue_drb& bearer = *ue->bearers.drbs().at(drb.drb_id);
    mac_ue_reconf_req.bearers_to_addmod.emplace_back();
    auto& lc_ch     = mac_ue_reconf_req.bearers_to_addmod.back();
    lc_ch.lcid      = bearer.lcid;
    lc_ch.ul_bearer = &bearer.connector.mac_rx_sdu_notifier;
    lc_ch.dl_bearer = &bearer.connector.mac_tx_sdu_notifier;
  }

  // Create Scheduler UE Reconfig Request that will be embedded in the MAC configuration request.
  mac_ue_reconf_req.sched_cfg               = create_scheduler_ue_config_request(*ue, *ue->resources);
  mac_ue_reconf_req.sched_cfg.reestablished = ue->reestablished_cfg_pending != nullptr;

  return du_params.mac.mgr.get_ue_configurator().handle_ue_reconfiguration_request(mac_ue_reconf_req);
}

f1ap_ue_context_update_response ue_configuration_procedure::make_ue_config_response()
{
  f1ap_ue_context_update_response resp;
  resp.result = true;

  // > Handle DRBs that were setup or failed to be setup.
  for (const f1ap_drb_to_setup& drb_req : request.drbs_to_setup) {
    if (ue->bearers.drbs().count(drb_req.drb_id) == 0) {
      resp.failed_drbs_setups.push_back({drb_req.drb_id, f1ap_cause_radio_network_t::no_radio_res_available});
      continue;
    }
    du_ue_drb& drb_added = *ue->bearers.drbs().at(drb_req.drb_id);

    resp.drbs_setup.emplace_back();
    f1ap_drb_setupmod& drb_setup = resp.drbs_setup.back();
    drb_setup.drb_id             = drb_added.drb_id;
    drb_setup.dluptnl_info_list  = drb_added.dluptnl_info_list;
  }

  // > Handle DRBs that were modified or failed to be modified.
  for (const f1ap_drb_to_modify& drb_req : request.drbs_to_mod) {
    if (ue->bearers.drbs().count(drb_req.drb_id) == 0) {
      resp.failed_drb_mods.push_back({drb_req.drb_id, f1ap_cause_radio_network_t::unknown_drb_id});
      continue;
    }
    du_ue_drb& drb_modified = *ue->bearers.drbs().at(drb_req.drb_id);

    resp.drbs_mod.emplace_back();
    f1ap_drb_setupmod& drb_mod = resp.drbs_mod.back();
    drb_mod.drb_id             = drb_modified.drb_id;
    drb_mod.dluptnl_info_list  = drb_modified.dluptnl_info_list;
    drb_mod.rlc_reestablished  = ue->reestablished_cfg_pending != nullptr;
  }

  // > Calculate ASN.1 CellGroupConfig to be sent in DU-to-CU container.
  ue->reestablished_ue_caps_summary = nullptr;
  asn1::rrc_nr::cell_group_cfg_s asn1_cell_group;
  if (ue->reestablished_cfg_pending != nullptr) {
    // In case of reestablishment, we send the full configuration to the UE but without an SRB1 and with SRB2 and DRBs
    // set to "RLCReestablish".
    ue->reestablished_cfg_pending = nullptr;

    calculate_cell_group_config_diff(asn1_cell_group, du_ue_resource_config{}, ue->resources.value());
    auto it = std::find_if(asn1_cell_group.rlc_bearer_to_add_mod_list.begin(),
                           asn1_cell_group.rlc_bearer_to_add_mod_list.end(),
                           [](const auto& b) { return b.lc_ch_id == LCID_SRB1; });
    if (it != asn1_cell_group.rlc_bearer_to_add_mod_list.end()) {
      asn1_cell_group.rlc_bearer_to_add_mod_list.erase(it);
    }
    for (auto& b : asn1_cell_group.rlc_bearer_to_add_mod_list) {
      b.rlc_cfg_present         = false;
      b.mac_lc_ch_cfg_present   = false;
      b.reestablish_rlc_present = true;
    }
  } else if (request.full_config_required) {
    // The CU requested a full configuration of the UE cellGroupConfig.
    calculate_cell_group_config_diff(asn1_cell_group, {}, ue->resources.value());
    resp.full_config_present = true;

  } else if (not request.source_cell_group_cfg.empty()) {
    // In case of source cell group configuration is passed, a delta configuration should be generated with it.
    // TODO: Apply diff using sourceCellGroup. For now, we use fullConfig.
    calculate_cell_group_config_diff(asn1_cell_group, {}, ue->resources.value());
    resp.full_config_present = true;

  } else {
    calculate_cell_group_config_diff(asn1_cell_group, prev_ue_res_cfg, ue->resources.value());
  }

  // Include RRC ReconfigWithSync if HandoverPreparationInformation is included.
  if (not request.ho_prep_info.empty()) {
    if (not request.spcell_id.has_value()) {
      proc_logger.log_proc_failure("Failed to handle HandoverPreparation IE. Cause: No Spcell ID provided");
      return make_ue_config_failure();
    }
    auto target_cell_it = std::find_if(du_params.ran.cells.begin(), du_params.ran.cells.end(), [this](const auto& e) {
      return e.nr_cgi == request.spcell_id;
    });
    if (target_cell_it == du_params.ran.cells.end()) {
      proc_logger.log_proc_failure("Failed to handle HandoverPreparation IE. Cause: No Spcell ID {}:{} found",
                                   request.spcell_id->plmn_id,
                                   request.spcell_id->nci);
      return make_ue_config_failure();
    }

    // Parse HandoverPreparationInformation.
    asn1::rrc_nr::ho_prep_info_s ho_prep_info;
    {
      asn1::cbit_ref    bref{request.ho_prep_info};
      asn1::SRSASN_CODE code = ho_prep_info.unpack(bref);
      if (code != asn1::SRSASN_SUCCESS) {
        proc_logger.log_proc_failure("Failed to unpack HandoverPreparation IE");
        return make_ue_config_failure();
      }
    }
    asn1_cell_group.sp_cell_cfg.recfg_with_sync_present = calculate_reconfig_with_sync_diff(
        asn1_cell_group.sp_cell_cfg.recfg_with_sync, *target_cell_it, ue->resources.value(), ho_prep_info, ue->rnti);
    if (not asn1_cell_group.sp_cell_cfg.recfg_with_sync_present) {
      proc_logger.log_proc_failure("Failed to calculate ReconfigWithSync");
      return make_ue_config_failure();
    }
    // Set all RLC bearer to reestablish for HO.
    for (auto& b : asn1_cell_group.rlc_bearer_to_add_mod_list) {
      b.rlc_cfg_present         = false;
      b.mac_lc_ch_cfg_present   = false;
      b.reestablish_rlc_present = true;
    }

    // Fill fields with -- Cond SyncAndCellAdd.
    asn1_cell_group.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id_present        = true;
    asn1_cell_group.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id                = 0;
    asn1_cell_group.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.first_active_ul_bwp_id_present = true;
    asn1_cell_group.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.first_active_ul_bwp_id         = 0;
  }

  // Set the servingCellMO if it is present in the request.
  if (request.serving_cell_mo.has_value()) {
    asn1_cell_group.sp_cell_cfg.sp_cell_cfg_ded.serving_cell_mo_present = true;
    asn1_cell_group.sp_cell_cfg.sp_cell_cfg_ded.serving_cell_mo         = *request.serving_cell_mo;
  }

  // > servingCellMO List IE.
  // [TS 38.473, 8.3.1.2] If the servingCellMO List IE is included in the UE CONTEXT SETUP REQUEST message, the gNB-DU
  // shall, if supported, select servingCellMO after determining the list of BWPs for the UE and include the list of
  // servingCellMOs that have been encoded in CellGroupConfig IE as ServingCellMO-encoded-in-CGC List IE in the UE
  // CONTEXT SETUP RESPONSE message.
  if (request.serving_cell_mo_list.has_value()) {
    const auto& serving_cell_mo_list = *request.serving_cell_mo_list;
    const auto& pcell_cfg            = du_params.ran.cells[ue->pcell_index];

    // Look for the SpCell ARFCN in the ServingCellMO List.
    const auto& serving_cell_mo_it =
        std::find_if(serving_cell_mo_list.begin(), serving_cell_mo_list.end(), [&pcell_cfg](const auto& item) {
          return item.ssb_freq == pcell_cfg.dl_carrier.arfcn_f_ref;
        });

    if (serving_cell_mo_it != serving_cell_mo_list.end()) {
      // If the SpCell ARFCN is found, set the servingCellMO.
      asn1_cell_group.sp_cell_cfg.sp_cell_cfg_ded.serving_cell_mo_present = true;
      asn1_cell_group.sp_cell_cfg.sp_cell_cfg_ded.serving_cell_mo         = serving_cell_mo_it->serving_cell_mo;
      resp.serving_cell_mo_encoded_in_cgc_list.push_back(serving_cell_mo_it->serving_cell_mo);
    }
  }

  // Pack cellGroupConfig.
  {
    asn1::bit_ref     bref{resp.cell_group_cfg};
    asn1::SRSASN_CODE code = asn1_cell_group.pack(bref);
    srsran_assert(code == asn1::SRSASN_SUCCESS, "Invalid cellGroupConfig");
  }

  // Calculate ASN.1 measGapConfig to be sent in DU-to-CU RRC container.
  if (prev_ue_res_cfg.meas_gap != ue->resources->meas_gap) {
    asn1::rrc_nr::meas_gap_cfg_s meas_gap;
    calculate_meas_gap_config_diff(meas_gap, prev_ue_res_cfg.meas_gap, ue->resources->meas_gap);

    // Pack measGapConfig.
    {
      asn1::bit_ref     bref{resp.meas_gap_cfg};
      asn1::SRSASN_CODE code = meas_gap.pack(bref);
      srsran_assert(code == asn1::SRSASN_SUCCESS, "Invalid measGapConfig");
    }
  }

  return resp;
}

f1ap_ue_context_update_response ue_configuration_procedure::make_ue_config_failure()
{
  f1ap_ue_context_update_response resp;
  resp.result = false;
  return resp;
}

f1ap_ue_context_update_response ue_configuration_procedure::make_empty_ue_config_response()
{
  f1ap_ue_context_update_response resp;
  resp.result = true;
  return resp;
}

bool ue_configuration_procedure::changed_detected() const
{
  return !request.drbs_to_setup.empty() || !request.drbs_to_mod.empty() || !request.srbs_to_setup.empty() ||
         !request.drbs_to_rem.empty() || !request.scells_to_setup.empty() || !request.scells_to_rem.empty() ||
         !request.ho_prep_info.empty() || request.full_config_required;
}

void ue_configuration_procedure::handle_rrc_reconfiguration_complete_ind()
{
  // Dispatch DRB context destruction to the respective UE executor.
  task_executor& exec = du_params.services.ue_execs.mac_ul_pdu_executor(ue->ue_index);
  if (not exec.defer([ue_ptr = ue]() mutable {
        for (const auto& [bearer_id, bearer] : ue_ptr->bearers.drbs()) {
          if (bearer != nullptr && bearer->drb_f1u != nullptr) {
            bearer->drb_f1u->get_tx_sdu_handler().flush_ul_buffer();
          }
        }
      })) {
    logger.warning("ue={}: Could not dispatch DRB removal task to UE executor. Destroying it the main DU manager "
                   "execution context",
                   fmt::underlying(ue->ue_index));
  }
}
