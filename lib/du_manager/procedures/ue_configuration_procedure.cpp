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
                                                       const du_manager_params::f1ap_config_params& f1ap_mng_) :
  request(request_),
  ue_mng(ue_mng_),
  services(du_services_),
  mac_ue_mng(mac_ue_mng_),
  f1ap_mng(f1ap_mng_),
  ue(ue_mng.find_ue(request.ue_index))
{
  srsgnb_assert(ue != nullptr, "ueId={} not found", request.ue_index);
}

void ue_configuration_procedure::operator()(coro_context<async_task<f1ap_ue_context_update_response>>& ctx)
{
  CORO_BEGIN(ctx);

  log_proc_started(logger, request.ue_index, ue->rnti, "UE Modification");

  add_bearers_to_ue_context();

  // > Update F1c/F1u bearers.
  update_f1_bearers();

  // > Update RLC bearers.
  update_rlc_bearers();

  // > Update MAC bearers.
  CORO_AWAIT(update_mac_bearers());

  log_proc_completed(logger, request.ue_index, ue->rnti, "UE Modification");

  CORO_RETURN(make_ue_config_response());
}

void ue_configuration_procedure::calculate_next_ue_context()
{
  next_cell_group = ue->cells[0];

  // > Add SRBs to next UE context.
  for (srb_id_t srbid : request.srbs_to_setup) {
    rlc_bearer_config srb_to_addmod;
    srb_to_addmod.lcid    = srb_id_to_lcid(srbid);
    srb_to_addmod.rlc_cfg = ue->bearers[srb_to_addmod.lcid].rlc_cfg;
    auto it               = std::find_if(next_cell_group.rlc_bearers.begin(),
                           next_cell_group.rlc_bearers.end(),
                           [srbid](const rlc_bearer_config& cfg) { return cfg.lcid == srb_id_to_lcid(srbid); });
    if (it == next_cell_group.rlc_bearers.end()) {
      next_cell_group.rlc_bearers.push_back(srb_to_addmod);
    } else {
      *it = srb_to_addmod;
    }
  }

  // > Add DRBs to next UE context.
  for (const drb_to_setup& drb : request.drbs_to_setup) {
    rlc_bearer_config drb_to_addmod;
    drb_to_addmod.drb_id = drb.drb_id;
    auto it              = std::find_if(ue->bearers.begin(), ue->bearers.end(), [&drb](const auto& bearer) {
      return bearer.drbid.has_value() and *bearer.drbid == drb.drb_id;
    });
    srsgnb_assert(it != ue->bearers.end(), "The bearer should have been created at this point");
    du_bearer& bearer     = *it;
    drb_to_addmod.lcid    = bearer.lcid;
    drb_to_addmod.rlc_cfg = bearer.rlc_cfg;
    next_cell_group.rlc_bearers.push_back(drb_to_addmod);
    // TODO: Check if it is add or mod.
  }
}

void ue_configuration_procedure::update_f1_bearers()
{
  f1ap_ue_configuration_request req{};
  req.ue_index = ue->ue_index;

  for (srb_id_t srb_id : request.srbs_to_setup) {
    lcid_t     lcid   = srb_id_to_lcid(srb_id);
    du_bearer& bearer = ue->bearers[lcid];
    req.f1c_bearers_to_add.emplace_back();
    req.f1c_bearers_to_add.back().srb_id       = srb_id;
    req.f1c_bearers_to_add.back().tx_pdu_notif = &bearer.bearer_connector.f1c_tx_pdu_notif;
  }

  for (const drb_to_setup& drb : request.drbs_to_setup) {
    auto it = std::find_if(ue->bearers.begin(), ue->bearers.end(), [&drb](const auto& bearer) {
      return bearer.drbid.has_value() and *bearer.drbid == drb.drb_id;
    });
    srsgnb_assert(it != ue->bearers.end(), "The bearer should have been created at this point");
    du_bearer& bearer = *it;
    req.f1u_bearers_to_add.emplace_back();
    req.f1u_bearers_to_add.back().drb_id          = drb.drb_id;
    req.f1u_bearers_to_add.back().tx_pdu_notifier = &bearer.bearer_connector.f1u_tx_pdu_notif;
  }

  f1ap_ue_configuration_response resp = f1ap_mng.ue_mng.handle_ue_configuration_request(req);

  // > Connect newly created F1c bearers to RLC.
  for (f1c_bearer_addmodded& bearer_added : resp.f1c_bearers_added) {
    ue->bearers[srb_id_to_uint(bearer_added.srb_id)].bearer_connector.rlc_rx_f1c_sdu_notif.connect(
        *bearer_added.bearer);
  }

  // > Connect newly created F1u bearers to RLC.
  for (f1u_bearer_addmodded& bearer_added : resp.f1u_bearers_added) {
    auto       it     = std::find_if(ue->bearers.begin(), ue->bearers.end(), [&bearer_added](const auto& bearer) {
      return bearer.drbid.has_value() and *bearer.drbid == bearer_added.drb_id;
    });
    du_bearer& bearer = *it;
    bearer.bearer_connector.rlc_rx_f1u_sdu_notif.connect(bearer_added.bearer->get_rx_pdu_handler());
  }
}

void ue_configuration_procedure::add_bearers_to_ue_context()
{
  // > Create DU UE SRB objects.
  for (srb_id_t srbid : request.srbs_to_setup) {
    lcid_t lcid = srb_id_to_lcid(srbid);
    srsgnb_assert(not ue->bearers.contains(lcid), "Reconfigurations of bearers not supported");
    ue->bearers.emplace(lcid);
    du_bearer& srb = ue->bearers[lcid];
    srb.lcid       = lcid;
    srb.rlc_cfg    = make_default_srb_rlc_config();
  }

  // > Create DU UE DRB objects.
  for (const drb_to_setup& drbtoadd : request.drbs_to_setup) {
    lcid_t lcid;
    if (drbtoadd.lcid.has_value()) {
      lcid = *drbtoadd.lcid;
      srsgnb_assert(not ue->bearers.contains(lcid), "Reconfigurations of bearers not supported");
    } else {
      lcid = (lcid_t)ue->bearers.find_first_empty(srb_id_to_uint(srb_id_t::srb3) + 1);
      srsgnb_assert(lcid != lcid_t::INVALID_LCID, "Unable to allocate LCID");
    }
    ue->bearers.emplace(lcid);
    du_bearer& drb = ue->bearers[lcid];
    drb.lcid       = lcid;
    drb.drbid      = drbtoadd.drb_id;
    drb.rlc_cfg    = create_rlc_config(drbtoadd);
  }
}

async_task<mac_ue_reconfiguration_response_message> ue_configuration_procedure::update_mac_bearers()
{
  mac_ue_reconfiguration_request_message mac_ue_reconf_req;
  mac_ue_reconf_req.ue_index    = request.ue_index;
  mac_ue_reconf_req.pcell_index = ue->pcell_index;

  for (srb_id_t srbid : request.srbs_to_setup) {
    du_bearer& bearer = ue->bearers[srb_id_to_uint(srbid)];
    mac_ue_reconf_req.bearers_to_addmod.emplace_back();
    mac_logical_channel& lc_ch = mac_ue_reconf_req.bearers_to_addmod.back();

    lc_ch.lcid      = bearer.lcid;
    lc_ch.ul_bearer = &bearer.bearer_connector.mac_rx_notif;
    lc_ch.dl_bearer = &bearer.bearer_connector.mac_tx_notif;
    mac_ue_reconf_req.bearers_to_addmod.push_back(std::move(lc_ch));
  }

  for (const drb_to_setup& drb : request.drbs_to_setup) {
    auto it = std::find_if(ue->bearers.begin(), ue->bearers.end(), [&drb](const auto& bearer) {
      return bearer.drbid.has_value() and *bearer.drbid == drb.drb_id;
    });
    srsgnb_assert(it != ue->bearers.end(), "The bearer should have been created at this point");
    du_bearer& bearer = *it;
    mac_ue_reconf_req.bearers_to_addmod.emplace_back();
    mac_logical_channel& lc_ch = mac_ue_reconf_req.bearers_to_addmod.back();

    lc_ch.lcid      = bearer.lcid;
    lc_ch.ul_bearer = &bearer.bearer_connector.mac_rx_notif;
    lc_ch.dl_bearer = &bearer.bearer_connector.mac_tx_notif;
    mac_ue_reconf_req.bearers_to_addmod.push_back(std::move(lc_ch));
  }

  return mac_ue_mng.handle_ue_reconfiguration_request(mac_ue_reconf_req);
}

void ue_configuration_procedure::update_rlc_bearers()
{
  // Create RLC SRB bearers.
  for (srb_id_t srb_id : request.srbs_to_setup) {
    du_bearer& srb = ue->bearers[srb_id_to_uint(srb_id)];
    srb.rlc_bearer = create_rlc_entity(make_rlc_entity_creation_message(ue->ue_index, ue->pcell_index, srb, services));
  }

  // Create RLC DRB bearers.
  for (const drb_to_setup& drb_to_setup : request.drbs_to_setup) {
    auto       it  = std::find_if(ue->bearers.begin(), ue->bearers.end(), [&drb_to_setup](const auto& b) {
      return b.drbid == drb_to_setup.drb_id;
    });
    du_bearer& drb = *it;
    drb.rlc_bearer = create_rlc_entity(make_rlc_entity_creation_message(ue->ue_index, ue->pcell_index, drb, services));
  }
}

f1ap_ue_context_update_response ue_configuration_procedure::make_ue_config_response()
{
  f1ap_ue_context_update_response resp;
  resp.result = true;

  calculate_next_ue_context();

  // Calculate ASN.1 CellGroupConfig to be sent in DU-to-CU container.
  asn1::rrc_nr::cell_group_cfg_s asn1_cell_group;
  calculate_cell_group_config_diff(asn1_cell_group, ue->cells[0], next_cell_group);
  {
    asn1::bit_ref     bref{resp.du_to_cu_rrc_container};
    asn1::SRSASN_CODE code = asn1_cell_group.pack(bref);
    srsgnb_assert(code == asn1::SRSASN_SUCCESS, "Invalid cellGroupConfig");
  }

  // Update UE context.
  ue->cells[0] = next_cell_group;

  return resp;
}
