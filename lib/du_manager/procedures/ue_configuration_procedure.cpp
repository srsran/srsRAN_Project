/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_configuration_procedure.h"
#include "../../ran/gnb_format.h"
#include "../converters/asn1_cell_group_config_helpers.h"
#include "srsgnb/mac/mac_ue_configurator.h"
#include "srsgnb/rlc/rlc_factory.h"

using namespace srsgnb;
using namespace srsgnb::srs_du;

ue_configuration_procedure::ue_configuration_procedure(const f1ap_ue_context_update_request&        request_,
                                                       ue_manager_ctrl_configurator&                ue_mng_,
                                                       const du_manager_params::service_params&     du_services_,
                                                       mac_ue_configurator&                         mac_ue_mng_,
                                                       const du_manager_params::rlc_config_params&  rlc_services_,
                                                       const du_manager_params::f1ap_config_params& f1ap_mng_) :
  request(request_),
  ue_mng(ue_mng_),
  services(du_services_),
  mac_ue_mng(mac_ue_mng_),
  rlc_services(rlc_services_),
  f1ap_mng(f1ap_mng_),
  ue(ue_mng.find_ue(request.ue_index))
{
  srsgnb_assert(ue != nullptr, "ueId={} not found", request.ue_index);
}

void ue_configuration_procedure::operator()(coro_context<async_task<f1ap_ue_context_update_response>>& ctx)
{
  CORO_BEGIN(ctx);

  log_proc_started(logger, request.ue_index, ue->rnti, "UE Configuration");

  prev_cell_group = ue->resources.value();
  if (ue->resources.update(ue->pcell_index, request).release_required) {
    logger.warning("Failed to allocate UE={} resources", ue->ue_index);
    CORO_EARLY_RETURN(make_ue_config_failure());
  }

  add_bearers_to_ue_context();

  // > Update RLC bearers.
  update_rlc_bearers();

  // > Update F1-C/F1-U bearers.
  update_f1_bearers();

  // > Update MAC bearers.
  CORO_AWAIT(update_mac_bearers());

  log_proc_completed(logger, request.ue_index, ue->rnti, "UE Configuration");

  CORO_RETURN(make_ue_config_response());
}

void ue_configuration_procedure::update_f1_bearers()
{
  f1ap_ue_configuration_request req{};
  req.ue_index = ue->ue_index;

  for (srb_id_t srb_id : request.srbs_to_setup) {
    du_ue_srb& bearer = ue->bearers.srbs()[srb_id];
    req.f1c_bearers_to_add.emplace_back();
    req.f1c_bearers_to_add.back().srb_id          = srb_id;
    req.f1c_bearers_to_add.back().rx_sdu_notifier = &bearer.connector.f1c_rx_sdu_notif;
  }

  for (const f1ap_drb_to_setup& drb : request.drbs_to_setup) {
    du_ue_drb& bearer = ue->bearers.drbs()[drb.drb_id];
    req.f1u_bearers_to_add.emplace_back();
    req.f1u_bearers_to_add.back().drb_id          = drb.drb_id;
    req.f1u_bearers_to_add.back().rx_sdu_notifier = &bearer.connector.f1u_rx_sdu_notif;
  }

  f1ap_ue_configuration_response resp = f1ap_mng.ue_mng.handle_ue_configuration_request(req);

  // Connect newly created F1-C bearers with RLC SRBs, and RLC SRBs with MAC logical channel notifiers.
  for (f1c_bearer_addmodded& bearer_added : resp.f1c_bearers_added) {
    du_ue_srb& srb = ue->bearers.srbs()[bearer_added.srb_id];
    srb.connector.connect(
        req.ue_index, bearer_added.srb_id, *bearer_added.bearer, *srb.rlc_bearer, rlc_services.mac_ue_info_handler);
  }

  // > Connect newly created F1-U bearers to RLC DRBs.
  for (f1u_bearer_addmodded& bearer_added : resp.f1u_bearers_added) {
    du_ue_drb& drb = ue->bearers.drbs()[bearer_added.drb_id];
    drb.connector.connect(req.ue_index,
                          bearer_added.drb_id,
                          drb.lcid,
                          *bearer_added.bearer,
                          *drb.rlc_bearer,
                          rlc_services.mac_ue_info_handler);
  }
}

void ue_configuration_procedure::add_bearers_to_ue_context()
{
  // > Create DU UE SRB objects.
  for (srb_id_t srbid : request.srbs_to_setup) {
    lcid_t lcid = srb_id_to_lcid(srbid);
    auto   it   = std::find_if(ue->resources->rlc_bearers.begin(),
                           ue->resources->rlc_bearers.end(),
                           [lcid](const rlc_bearer_config& e) { return e.lcid == lcid; });
    srsgnb_assert(it != ue->resources->rlc_bearers.end(), "SRB should have been allocated at this point");
    ue->bearers.add_srb(srbid, it->rlc_cfg);
  }

  // > Create DU UE DRB objects.
  for (const f1ap_drb_to_setup& drbtoadd : request.drbs_to_setup) {
    auto it = std::find_if(ue->resources->rlc_bearers.begin(),
                           ue->resources->rlc_bearers.end(),
                           [&drbtoadd](const rlc_bearer_config& e) { return e.drb_id == drbtoadd.drb_id; });
    srsgnb_assert(it != ue->resources->rlc_bearers.end(), "The bearer config should be created at this point");
    ue->bearers.add_drb(drbtoadd.drb_id, it->lcid, it->rlc_cfg);
  }
}

async_task<mac_ue_reconfiguration_response_message> ue_configuration_procedure::update_mac_bearers()
{
  mac_ue_reconfiguration_request_message mac_ue_reconf_req;
  mac_ue_reconf_req.ue_index    = request.ue_index;
  mac_ue_reconf_req.pcell_index = ue->pcell_index;

  for (srb_id_t srbid : request.srbs_to_setup) {
    du_ue_srb& bearer = ue->bearers.srbs()[srbid];
    mac_ue_reconf_req.bearers_to_addmod.emplace_back();
    mac_logical_channel& lc_ch = mac_ue_reconf_req.bearers_to_addmod.back();

    lc_ch.lcid      = bearer.lcid();
    lc_ch.ul_bearer = &bearer.connector.mac_rx_sdu_notifier;
    lc_ch.dl_bearer = &bearer.connector.mac_tx_sdu_notifier;
  }

  for (const f1ap_drb_to_setup& drb : request.drbs_to_setup) {
    du_ue_drb& bearer = ue->bearers.drbs()[drb.drb_id];
    mac_ue_reconf_req.bearers_to_addmod.emplace_back();
    mac_logical_channel& lc_ch = mac_ue_reconf_req.bearers_to_addmod.back();

    lc_ch.lcid      = bearer.lcid;
    lc_ch.ul_bearer = &bearer.connector.mac_rx_sdu_notifier;
    lc_ch.dl_bearer = &bearer.connector.mac_tx_sdu_notifier;
  }

  return mac_ue_mng.handle_ue_reconfiguration_request(mac_ue_reconf_req);
}

void ue_configuration_procedure::update_rlc_bearers()
{
  // Create RLC SRB bearers.
  for (srb_id_t srb_id : request.srbs_to_setup) {
    du_ue_srb& srb = ue->bearers.srbs()[srb_id];
    srb.rlc_bearer = create_rlc_entity(make_rlc_entity_creation_message(ue->ue_index, ue->pcell_index, srb, services));
  }

  // Create RLC DRB bearers.
  for (const f1ap_drb_to_setup& drb_to_setup : request.drbs_to_setup) {
    du_ue_drb& drb = ue->bearers.drbs()[drb_to_setup.drb_id];
    drb.rlc_bearer = create_rlc_entity(make_rlc_entity_creation_message(ue->ue_index, ue->pcell_index, drb, services));
  }
}

f1ap_ue_context_update_response ue_configuration_procedure::make_ue_config_response()
{
  f1ap_ue_context_update_response resp;
  resp.result = true;

  // Calculate ASN.1 CellGroupConfig to be sent in DU-to-CU container.
  asn1::rrc_nr::cell_group_cfg_s asn1_cell_group;
  calculate_cell_group_config_diff(asn1_cell_group, prev_cell_group, *ue->resources);
  {
    asn1::bit_ref     bref{resp.du_to_cu_rrc_container};
    asn1::SRSASN_CODE code = asn1_cell_group.pack(bref);
    srsgnb_assert(code == asn1::SRSASN_SUCCESS, "Invalid cellGroupConfig");
  }

  return resp;
}

f1ap_ue_context_update_response ue_configuration_procedure::make_ue_config_failure()
{
  f1ap_ue_context_update_response resp;
  resp.result = false;
  return resp;
}
