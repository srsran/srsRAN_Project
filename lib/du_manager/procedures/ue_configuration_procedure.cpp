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

using namespace srsgnb;
using namespace srsgnb::srs_du;

ue_configuration_procedure::ue_configuration_procedure(const f1ap_ue_config_update_request& request_,
                                                       ue_manager_ctrl_configurator&        ue_mng_,
                                                       mac_ue_configurator&                 mac_ue_mng_) :
  request(request_), ue_mng(ue_mng_), mac_ue_mng(mac_ue_mng_), ue(ue_mng.find_ue(request.ue_index))
{
  srsgnb_assert(ue != nullptr, "ueId={} not found", request.ue_index);

  calculate_next_ue_context();
}

void ue_configuration_procedure::operator()(coro_context<async_task<f1ap_ue_config_update_response>>& ctx)
{
  CORO_BEGIN(ctx);

  log_proc_started(logger, request.ue_index, ue->rnti, "UE Modification");

  add_bearers_to_ue_context();

  CORO_AWAIT(update_mac_bearers());

  log_proc_completed(logger, request.ue_index, ue->rnti, "UE Modification");

  CORO_RETURN(make_ue_config_response());
}

void ue_configuration_procedure::calculate_next_ue_context()
{
  next_cell_group = ue->cells[0];

  // Add SRBs to next UE context.
  for (srb_id_t srbid : request.srbs_to_addmod) {
    rlc_bearer_config srb_to_addmod = make_default_srb(srb_id_to_lcid(srbid));

    auto it = std::find_if(next_cell_group.rlc_bearers.begin(),
                           next_cell_group.rlc_bearers.end(),
                           [srbid](const rlc_bearer_config& cfg) { return cfg.lcid == srb_id_to_lcid(srbid); });
    if (it == next_cell_group.rlc_bearers.end()) {
      next_cell_group.rlc_bearers.push_back(srb_to_addmod);
    } else {
      *it = srb_to_addmod;
    }
  }

  // Add DRBs to next UE context.
  for (const drb_to_addmod& drb : request.drbs_to_addmod) {
    rlc_bearer_config drb_to_addmod = make_default_srb(drb.lcid);
    drb_to_addmod.drb_id            = drb.drbid;
    next_cell_group.rlc_bearers.push_back(drb_to_addmod);
    // TODO: Check if it is add or mod.
  }
}

void ue_configuration_procedure::add_bearers_to_ue_context()
{
  for (srb_id_t srbid : request.srbs_to_addmod) {
    lcid_t lcid = (lcid_t)srbid;
    srsgnb_assert(not ue->bearers.contains(lcid), "Reconfigurations of bearers not supported");
    ue->bearers.emplace(lcid);
    du_bearer& srb = ue->bearers[lcid];
    srb.lcid       = lcid;
    srb.drbid      = drb_id_t::invalid;
  }

  for (const drb_to_addmod& drbtoadd : request.drbs_to_addmod) {
    lcid_t lcid = drbtoadd.lcid;
    if (lcid != lcid_t::INVALID_LCID) {
      srsgnb_assert(not ue->bearers.contains(drbtoadd.lcid), "Reconfigurations of bearers not supported");
    } else {
      lcid = (lcid_t)ue->bearers.find_first_empty(srb_id_to_uint(srb_id_t::srb3) + 1);
      srsgnb_assert(lcid != lcid_t::INVALID_LCID, "Unable to allocate LCID");
    }
    ue->bearers.emplace(lcid);
    du_bearer& drb = ue->bearers[lcid];
    drb.lcid       = lcid;
    drb.drbid      = drbtoadd.drbid;
  }
}

async_task<mac_ue_reconfiguration_response_message> ue_configuration_procedure::update_mac_bearers()
{
  mac_ue_reconfiguration_request_message mac_ue_reconf_req;
  mac_ue_reconf_req.ue_index    = request.ue_index;
  mac_ue_reconf_req.pcell_index = ue->pcell_index;

  for (srb_id_t srbid : request.srbs_to_addmod) {
    du_bearer& bearer = ue->bearers[srb_id_to_uint(srbid)];
    mac_ue_reconf_req.bearers_to_addmod.emplace_back();
    mac_logical_channel_addmod& lc_ch = mac_ue_reconf_req.bearers_to_addmod.back();

    lc_ch.lcid      = bearer.lcid;
    lc_ch.ul_bearer = &bearer.bearer_connector.mac_rx_notif;
    lc_ch.dl_bearer = &bearer.bearer_connector.mac_tx_notif;
    mac_ue_reconf_req.bearers_to_addmod.push_back(std::move(lc_ch));
  }

  for (const drb_to_addmod& drb : request.drbs_to_addmod) {
    du_bearer& bearer = ue->bearers[drb.lcid];
    mac_ue_reconf_req.bearers_to_addmod.emplace_back();
    mac_logical_channel_addmod& lc_ch = mac_ue_reconf_req.bearers_to_addmod.back();

    lc_ch.lcid      = bearer.lcid;
    lc_ch.ul_bearer = &bearer.bearer_connector.mac_rx_notif;
    lc_ch.dl_bearer = &bearer.bearer_connector.mac_tx_notif;
    mac_ue_reconf_req.bearers_to_addmod.push_back(std::move(lc_ch));
  }

  return mac_ue_mng.handle_ue_reconfiguration_request(mac_ue_reconf_req);
}

f1ap_ue_config_update_response ue_configuration_procedure::make_ue_config_response()
{
  f1ap_ue_config_update_response resp;
  resp.result = true;

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
