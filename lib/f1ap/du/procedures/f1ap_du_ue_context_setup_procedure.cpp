/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "f1ap_du_ue_context_setup_procedure.h"
#include "../ue_context/f1ap_du_ue_manager.h"
#include "f1ap_du_ue_context_common.h"
#include "proc_logger.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/f1ap/common/f1ap_message.h"

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

f1ap_du_ue_context_setup_procedure::f1ap_du_ue_context_setup_procedure(
    const asn1::f1ap::ue_context_setup_request_s& msg_,
    f1ap_du_ue_manager&                           ue_mng_,
    f1ap_du_configurator&                         du_mng_,
    du_ue_index_t                                 ue_index_) :
  msg(msg_), ue_mng(ue_mng_), du_mng(du_mng_), ue_index(ue_index_), logger(srslog::fetch_basic_logger("F1AP-DU"))
{
}

void f1ap_du_ue_context_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  if (msg->gnb_du_ue_f1ap_id_present) {
    const gnb_cu_ue_f1ap_id_t gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id);
    const gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id);

    logger.debug("{}: Starting procedure...", f1ap_log_prefix{gnb_du_ue_f1ap_id, gnb_cu_ue_f1ap_id, name()});

    ue = ue_mng.find(gnb_du_ue_f1ap_id);
    if (ue == nullptr) {
      logger.warning("{}: Discarding UeContextSetupRequest. Cause: Unrecognized GNB-DU-UE-F1AP-ID.",
                     f1ap_log_prefix{gnb_du_ue_f1ap_id, gnb_cu_ue_f1ap_id, name()});
      send_ue_context_setup_failure();
      CORO_EARLY_RETURN();
    }
  } else {
    // [TS38.473, 8.3.1.2] If no UE-associated logical F1-connection exists, the UE-associated logical F1-connection
    // shall be established as part of the procedure.

    // Request the creation of a new UE context in the DU.
    CORO_AWAIT_VALUE(
        du_ue_create_response,
        du_mng.request_ue_creation(f1ap_ue_context_creation_request{ue_index, to_du_cell_index(msg->serv_cell_idx)}));
    if (not du_ue_create_response->result) {
      // Failed to create UE context in the DU.
      logger.warning("{}: Failed to allocate new UE context in DU.",
                     f1ap_log_prefix{int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id), name()});
      send_ue_context_setup_failure();
      CORO_EARLY_RETURN();
    }

    // Fetch created UE context.
    ue = ue_mng.find(ue_index);
    srsran_assert(ue != nullptr, "UE context was not correctly created");

    // Save allocated C-RNTI for created UE.
    ue->context.rnti = du_ue_create_response->crnti;
  }

  if (ue->context.gnb_cu_ue_f1ap_id == gnb_cu_ue_f1ap_id_t::invalid) {
    // If this is the first message with a GNB-CU-UE-F1AP-ID, update the UE context.
    ue->context.gnb_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id);
  }

  // Setup new UE configuration in DU.
  CORO_AWAIT_VALUE(du_ue_cfg_response, request_du_ue_config());
  if (not du_ue_cfg_response.result) {
    // Clear up resources allocated during the procedure.
    if (du_ue_create_response.has_value() and du_ue_cfg_response.result) {
      CORO_AWAIT(du_mng.request_ue_removal(f1ap_ue_delete_request{ue_index}));
    }
    send_ue_context_setup_failure();
    CORO_EARLY_RETURN();
  }

  // Send RRC container to lower layers.
  send_rrc_container();

  // Respond back to CU-CP with success.
  send_ue_context_setup_response();

  CORO_RETURN();
}

async_task<f1ap_ue_context_update_response> f1ap_du_ue_context_setup_procedure::request_du_ue_config()
{
  // Construct DU request.
  f1ap_ue_context_update_request du_request;
  du_request.ue_index = ue->context.ue_index;

  // > Set whether full configuration is required.
  // [TS 38.473, section 8.3.1.1] If the received CU to DU RRC Information IE does not include source cell group
  // configuration, the gNB-DU shall generate the cell group configuration using full configuration. Otherwise,
  // delta configuration is allowed.
  du_request.full_config_required =
      not msg->cu_to_du_rrc_info.ie_exts_present or not msg->cu_to_du_rrc_info.ie_exts.cell_group_cfg_present;

  // > Pass SRBs to setup.
  for (const auto& srb : msg->srbs_to_be_setup_list) {
    du_request.srbs_to_setup.push_back(make_srb_id(srb.value().srbs_to_be_setup_item()));
  }

  // > Pass DRBs to setup.
  for (const auto& drb : msg->drbs_to_be_setup_list) {
    du_request.drbs_to_setup.push_back(make_drb_to_setup(drb.value().drbs_to_be_setup_item()));
  }

  if (msg->cu_to_du_rrc_info.ie_exts_present) {
    // > Add HO preparation information in case of Handover.
    if (msg->cu_to_du_rrc_info.ie_exts.ho_prep_info_present) {
      du_request.ho_prep_info = msg->cu_to_du_rrc_info.ie_exts.ho_prep_info.copy();
    }

    // > Add source cell group config for delta configuration.
    if (msg->cu_to_du_rrc_info.ie_exts.cell_group_cfg_present) {
      du_request.source_cell_group_cfg = msg->cu_to_du_rrc_info.ie_exts.cell_group_cfg.copy();
    }
  }

  return ue->du_handler.request_ue_context_update(du_request);
}

void f1ap_du_ue_context_setup_procedure::send_rrc_container()
{
  // > If the UE CONTEXT SETUP REQUEST message contains the RRC-Container IE, the gNB-DU shall send the corresponding
  // RRC message to the UE via SRB1.
  if (msg->rrc_container_present and not msg->rrc_container.empty()) {
    ue->bearers.find_srb(srb_id_t::srb1)->handle_pdu(msg->rrc_container.copy());
  }
}

void f1ap_du_ue_context_setup_procedure::send_ue_context_setup_response()
{
  // Prepare UE CONTEXT SETUP RESPONSE message.
  f1ap_message f1ap_msg;
  f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_resp_s& resp = f1ap_msg.pdu.successful_outcome().value.ue_context_setup_resp();

  resp->gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(ue->context.gnb_du_ue_f1ap_id);
  resp->gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(ue->context.gnb_cu_ue_f1ap_id);

  // > DU-to-CU RRC Container.
  if (not resp->du_to_cu_rrc_info.cell_group_cfg.append(du_ue_cfg_response.du_to_cu_rrc_container)) {
    logger.error("{}: Failed to append DU-to-CU RRC container.", f1ap_log_prefix{ue->context, name()});
    return;
  }

  // > Check if DU-to-CU RRC Container is a full cellGroupConfig or a delta.
  if (du_ue_cfg_response.full_config_present) {
    resp->full_cfg_present = true;
    resp->full_cfg.value   = asn1::f1ap::full_cfg_opts::full;
  }

  // > If the C-RNTI IE is included in the UE CONTEXT SETUP RESPONSE, the gNB-CU shall consider that the C-RNTI has
  // been allocated by the gNB-DU for this UE context.
  if (du_ue_create_response.has_value()) {
    resp->c_rnti_present = true;
    resp->c_rnti         = to_value(du_ue_create_response->crnti);
  }

  // > SRBs setup list.
  resp->srbs_setup_list_present = msg->srbs_to_be_setup_list_present;
  if (resp->srbs_setup_list_present) {
    resp->srbs_setup_list.resize(msg->srbs_to_be_setup_list.size());
    for (unsigned i = 0; i != resp->srbs_setup_list.size(); ++i) {
      resp->srbs_setup_list[i].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_ITEM);
      auto&              srb_req_item = msg->srbs_to_be_setup_list[i]->srbs_to_be_setup_item();
      srbs_setup_item_s& srb_item     = resp->srbs_setup_list[i].value().srbs_setup_item();
      srb_item.srb_id                 = srb_req_item.srb_id;
      srb_item.lcid                   = srb_id_to_lcid(int_to_srb_id(srb_req_item.srb_id));
    }
  }

  // > DRBs setup List.
  resp->drbs_setup_list_present = not du_ue_cfg_response.drbs_setup.empty();
  if (resp->drbs_setup_list_present) {
    resp->drbs_setup_list.resize(du_ue_cfg_response.drbs_setup.size());
    for (unsigned i = 0; i != resp->drbs_setup_list.size(); ++i) {
      resp->drbs_setup_list[i].load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_ITEM);
      drbs_setup_item_s& asn1_drb = resp->drbs_setup_list[i].value().drbs_setup_item();
      auto&              drb_resp = du_ue_cfg_response.drbs_setup[i];
      asn1_drb.drb_id             = drb_id_to_uint(drb_resp.drb_id);
      asn1_drb.lcid_present       = drb_resp.lcid.has_value();
      if (asn1_drb.lcid_present) {
        asn1_drb.lcid = drb_resp.lcid.value();
      }
      asn1_drb.dl_up_tnl_info_to_be_setup_list.resize(drb_resp.dluptnl_info_list.size());
      for (unsigned j = 0; j != drb_resp.dluptnl_info_list.size(); ++j) {
        up_transport_layer_info_to_asn1(asn1_drb.dl_up_tnl_info_to_be_setup_list[j].dl_up_tnl_info,
                                        drb_resp.dluptnl_info_list[j]);
      }
    }
  }

  // > DRBs-FailedToBeSetupMod-List.
  resp->drbs_failed_to_be_setup_list_present = not du_ue_cfg_response.drbs_failed_to_setup.empty();
  if (resp->drbs_failed_to_be_setup_list_present) {
    resp->drbs_failed_to_be_setup_list.resize(du_ue_cfg_response.drbs_failed_to_setup.size());
    for (unsigned i = 0; i != du_ue_cfg_response.drbs_failed_to_setup.size(); ++i) {
      resp->drbs_failed_to_be_setup_list[i].load_info_obj(ASN1_F1AP_ID_DRBS_FAILED_TO_BE_SETUP_MOD_ITEM);
      drbs_failed_to_be_setup_item_s& asn1_drb = resp->drbs_failed_to_be_setup_list[i]->drbs_failed_to_be_setup_item();
      asn1_drb.drb_id                          = drb_id_to_uint(du_ue_cfg_response.drbs_failed_to_setup[i]);
      asn1_drb.cause.set_transport().value     = cause_transport_opts::transport_res_unavailable;
    }
  }

  // Send Response to CU-CP.
  ue->f1ap_msg_notifier.on_new_message(f1ap_msg);

  logger.debug("{}: Procedure finished successfully.", f1ap_log_prefix{ue->context, name()});
}

void f1ap_du_ue_context_setup_procedure::send_ue_context_setup_failure()
{
  // Prepare UE CONTEXT SETUP FAILURE message.
  f1ap_message f1ap_msg;
  f1ap_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_fail_s& resp = f1ap_msg.pdu.unsuccessful_outcome().value.ue_context_setup_fail();

  resp->gnb_cu_ue_f1ap_id = msg->gnb_cu_ue_f1ap_id;
  if (ue != nullptr) {
    resp->gnb_du_ue_f1ap_id_present = true;
    resp->gnb_du_ue_f1ap_id         = gnb_du_ue_f1ap_id_to_uint(ue->context.gnb_du_ue_f1ap_id);
  }

  resp->cause.set_radio_network().value = asn1::f1ap::cause_radio_network_opts::no_radio_res_available;

  // Send UE CONTEXT SETUP FAILURE to CU-CP.
  ue->f1ap_msg_notifier.on_new_message(f1ap_msg);

  logger.debug("{}: Procedure finished with failure.",
               ue == nullptr ? f1ap_log_prefix{int_to_gnb_cu_ue_f1ap_id(resp->gnb_cu_ue_f1ap_id), name()}
                             : f1ap_log_prefix{ue->context, name()});
}
