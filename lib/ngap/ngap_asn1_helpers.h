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

#pragma once

#include "ngap_asn1_converters.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/ngap/ngap_ies.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_context.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/ngap/ngap_init_context_setup.h"
#include "srsran/ngap/ngap_nas.h"
#include "srsran/ngap/ngap_nrppa.h"
#include "srsran/ngap/ngap_reset.h"
#include "srsran/ngap/ngap_setup.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/cu_types.h"
#include "srsran/security/security.h"
#include <string>
#include <vector>

namespace srsran {

namespace srs_cu_cp {

/// \brief Fills ASN.1 NGSetupRequest struct.
/// \param[out] asn1_request The NGSetupRequest ASN.1 struct to fill.
/// \param[in] ngap_ctxt The NGAP context.
inline void fill_asn1_ng_setup_request(asn1::ngap::ng_setup_request_s& asn1_request, const ngap_context_t& ngap_ctxt)
{
  // fill global ran node id
  auto& global_gnb = asn1_request->global_ran_node_id.set_global_gnb_id();
  global_gnb.gnb_id.set_gnb_id();
  global_gnb.gnb_id.gnb_id().from_number(ngap_ctxt.gnb_id.id, ngap_ctxt.gnb_id.bit_length);
  // TODO: Which PLMN do we need to use here?
  global_gnb.plmn_id = ngap_ctxt.supported_tas.front().plmn_list.front().plmn_id.to_bytes();

  // fill ran node name
  asn1_request->ran_node_name_present = true;
  asn1_request->ran_node_name.from_string(ngap_ctxt.ran_node_name);

  // fill supported ta list
  for (const auto& supported_ta_item : ngap_ctxt.supported_tas) {
    asn1::ngap::supported_ta_item_s asn1_supported_ta_item = {};

    // fill tac
    asn1_supported_ta_item.tac.from_number(supported_ta_item.tac);

    // fill broadcast plmn list
    for (const auto& plmn_item : supported_ta_item.plmn_list) {
      asn1::ngap::broadcast_plmn_item_s asn1_broadcast_plmn_item = {};

      // fill plmn id
      asn1_broadcast_plmn_item.plmn_id = plmn_item.plmn_id.to_bytes();

      // fill tai slice support list
      for (const auto& slice_support_item : plmn_item.slice_support_list) {
        // fill s_nssai
        asn1::ngap::slice_support_item_s asn1_slice_support_item = {};
        asn1_slice_support_item.s_nssai                          = s_nssai_to_asn1(slice_support_item);

        asn1_broadcast_plmn_item.tai_slice_support_list.push_back(asn1_slice_support_item);
      }
      asn1_supported_ta_item.broadcast_plmn_list.push_back(asn1_broadcast_plmn_item);
    }

    asn1_request->supported_ta_list.push_back(asn1_supported_ta_item);
  }

  // fill paging drx
  asn1::number_to_enum(asn1_request->default_paging_drx, ngap_ctxt.default_paging_drx);
}

/// \brief Fills the common type \c ngap_ng_setup_result struct.
/// \param[out] result The common type \c ngap_ng_setup_result struct to fill.
/// \param[in] asn1_response The ASN.1 type NGSetupResponse.
inline void fill_ngap_ng_setup_result(ngap_ng_setup_result& result, const asn1::ngap::ng_setup_resp_s& asn1_response)
{
  ngap_ng_setup_response response;

  // fill amf name
  response.amf_name = asn1_response->amf_name.to_string();

  // fill served guami list
  for (const auto& asn1_served_guami_item : asn1_response->served_guami_list) {
    ngap_served_guami_item served_guami_item = {};
    served_guami_item.guami                  = asn1_to_guami(asn1_served_guami_item.guami);
    if (asn1_served_guami_item.backup_amf_name_present) {
      served_guami_item.backup_amf_name = asn1_served_guami_item.backup_amf_name.to_string();
    }
    response.served_guami_list.push_back(served_guami_item);
  }

  // fill relative amf capacity
  response.relative_amf_capacity = asn1_response->relative_amf_capacity;

  // fill plmn support list
  for (const auto& asn1_plmn_support_item : asn1_response->plmn_support_list) {
    ngap_plmn_support_item plmn_support_item = {};
    plmn_support_item.plmn_id                = asn1_plmn_support_item.plmn_id.to_string();

    for (const auto& asn1_slice_support_item : asn1_plmn_support_item.slice_support_list) {
      slice_support_item_t slice_support_item = {};
      slice_support_item.s_nssai.sst = slice_service_type{(uint8_t)asn1_slice_support_item.s_nssai.sst.to_number()};
      if (asn1_slice_support_item.s_nssai.sd_present) {
        slice_support_item.s_nssai.sd =
            slice_differentiator::create(asn1_slice_support_item.s_nssai.sd.to_number()).value();
      }
      plmn_support_item.slice_support_list.push_back(slice_support_item);
    }
    response.plmn_support_list.push_back(plmn_support_item);
  }

  // TODO: Fill crit diagnostics

  // TODO: Add missing optional values

  result = response;
}

/// \brief Fills the common type \c ngap_ng_setup_result struct.
/// \param[out] result The common type \c ngap_ng_setup_result struct to fill.
/// \param[in] asn1_fail The ASN.1 type NGSetupFailure.
inline void fill_ngap_ng_setup_result(ngap_ng_setup_result& result, const asn1::ngap::ng_setup_fail_s& asn1_fail)
{
  ngap_ng_setup_failure fail;
  fail.cause = asn1_to_cause(asn1_fail->cause);

  if (asn1_fail->crit_diagnostics_present) {
    // TODO: Add crit diagnostics
  }

  result = fail;
}

inline void fill_asn1_ng_reset(asn1::ngap::ng_reset_s& asn1_reset, const ngap_ng_reset& reset)
{
  asn1_reset->cause = cause_to_asn1(reset.cause);

  if (reset.reset_type.index() == 0) {
    asn1_reset->reset_type.set_ng_interface();
  } else {
    asn1_reset->reset_type.set_part_of_ng_interface();
    for (const auto& item : std::get<ngap_reset_type_part_of_interface>(reset.reset_type)) {
      asn1::ngap::ue_associated_lc_ng_conn_item_s asn1_item;
      if (item.amf_ue_id.has_value()) {
        asn1_item.amf_ue_ngap_id_present = true;
        asn1_item.amf_ue_ngap_id         = amf_ue_id_to_uint(item.amf_ue_id.value());
      }
      if (item.ran_ue_id.has_value()) {
        asn1_item.ran_ue_ngap_id_present = true;
        asn1_item.ran_ue_ngap_id         = ran_ue_id_to_uint(item.ran_ue_id.value());
      }
      asn1_reset->reset_type.part_of_ng_interface().push_back(asn1_item);
    }
  }
}

/// \brief Fills the common type \c ngap_dl_nas_transport_message struct.
/// \param[out] msg The common type \c ngap_dl_nas_transport_message struct to fill.
/// \param[in] ue_index The index of the UE.
/// \param[in] asn1_msg The ASN.1 type DLNASTransport.
inline void fill_ngap_dl_nas_transport_message(ngap_dl_nas_transport_message&        msg,
                                               ue_index_t                            ue_index,
                                               const asn1::ngap::dl_nas_transport_s& asn1_msg)
{
  msg.ue_index = ue_index;
  msg.nas_pdu  = asn1_msg->nas_pdu.copy();
  if (asn1_msg->ue_cap_info_request_present &&
      asn1_msg->ue_cap_info_request == asn1::ngap::ue_cap_info_request_opts::options::requested) {
    msg.ue_cap_info_request = true;
  }
}

/// \brief Convert common type Initial UE Message to NGAP Initial UE Message.
/// \param[out] asn1_msg The ASN1 NGAP Initial UE Message.
/// \param[in] msg The CU-CP Initial UE Message.
inline void fill_asn1_initial_ue_message(asn1::ngap::init_ue_msg_s&      asn1_msg,
                                         const cu_cp_initial_ue_message& msg,
                                         const ngap_context_t&           context)
{
  asn1_msg->nas_pdu = msg.nas_pdu.copy();

  asn1_msg->rrc_establishment_cause.value =
      static_cast<asn1::ngap::rrc_establishment_cause_opts::options>(msg.establishment_cause);

  auto& user_loc_info_nr = asn1_msg->user_location_info.set_user_location_info_nr();
  user_loc_info_nr       = cu_cp_user_location_info_to_asn1(msg.user_location_info);

  asn1_msg->ue_context_request_present = true;
  asn1_msg->ue_context_request.value   = asn1::ngap::ue_context_request_opts::options::requested;

  if (msg.five_g_s_tmsi.has_value()) {
    // TS 23.003 - 5G-S-TMSI contains AMF Set ID, AMF Pointer and 5G TMSI.
    asn1_msg->five_g_s_tmsi_present = true;
    asn1_msg->five_g_s_tmsi.amf_set_id.from_number(msg.five_g_s_tmsi.value().get_amf_set_id());
    asn1_msg->five_g_s_tmsi.amf_pointer.from_number(msg.five_g_s_tmsi.value().get_amf_pointer());
    asn1_msg->five_g_s_tmsi.five_g_tmsi.from_number(msg.five_g_s_tmsi.value().get_five_g_tmsi());
  }

  if (msg.amf_set_id.has_value()) {
    asn1_msg->amf_set_id_present = true;
    asn1_msg->amf_set_id.from_number(msg.amf_set_id.value());
  }

  // TODO: Add missing optional values
}

/// \brief Convert common type UL NAS Transport message to NGAP UL NAS Transport message.
/// \param[out] asn1_msg The ASN1 NGAP UL NAS Transport message.
/// \param[in] msg The CU-CP UL NAS Transport message.
inline void fill_asn1_ul_nas_transport(asn1::ngap::ul_nas_transport_s& asn1_msg, const cu_cp_ul_nas_transport& msg)
{
  asn1_msg->nas_pdu = msg.nas_pdu.copy();

  auto& user_loc_info_nr = asn1_msg->user_location_info.set_user_location_info_nr();
  user_loc_info_nr       = cu_cp_user_location_info_to_asn1(msg.user_location_info);
}

/// Helper function to fill the CU-CP PDU Session Resource Setup Item for both, PDUSessionResourceSetupItemSUReq and
/// PDUSessionResourceSetupItemCxtReq. Note that the NAS-PDU is added in separate functions
/// \param[out] setup_item The cu_cp_pdu_session_res_setup_item struct to fill.
/// \param[in] asn1_session_item The pdu_session_res_setup_item_su_req_s or pdu_session_res_setup_item_cxt_req_s ASN1
/// struct.
/// \returns True if the conversion was successful, false otherwise.
template <typename template_asn1_item>
inline bool fill_cu_cp_pdu_session_resource_setup_item_base(cu_cp_pdu_session_res_setup_item& setup_item,
                                                            const template_asn1_item&         asn1_session_item,
                                                            byte_buffer                       asn1_request_transfer)
{
  // pDUSessionID
  setup_item.pdu_session_id = uint_to_pdu_session_id(asn1_session_item.pdu_session_id);

  // pDUSessionNAS-PDU / NAS-PDU will be added in a separate function

  // s-NSSAI
  if (asn1_session_item.s_nssai.sd_present) {
    setup_item.s_nssai.sd = slice_differentiator::create(asn1_session_item.s_nssai.sd.to_number()).value();
  }
  setup_item.s_nssai.sst = slice_service_type{(uint8_t)asn1_session_item.s_nssai.sst.to_number()};

  // pDUSessionResourceSetupRequestTransfer
  asn1::ngap::pdu_session_res_setup_request_transfer_s asn1_setup_req_transfer;
  asn1::cbit_ref bref({asn1_request_transfer.begin(), asn1_request_transfer.end()});

  if (asn1_setup_req_transfer.unpack(bref) != asn1::SRSASN_SUCCESS) {
    srslog::fetch_basic_logger("NGAP").error("Couldn't unpack PDU Session Resource Setup Request Transfer PDU");
    return false;
  }

  if (asn1_setup_req_transfer->pdu_session_aggr_max_bit_rate_present) {
    // id-PDUSessionAggregateMaximumBitRate
    setup_item.pdu_session_aggregate_maximum_bit_rate_dl =
        asn1_setup_req_transfer->pdu_session_aggr_max_bit_rate.pdu_session_aggr_max_bit_rate_dl;
    setup_item.pdu_session_aggregate_maximum_bit_rate_ul =
        asn1_setup_req_transfer->pdu_session_aggr_max_bit_rate.pdu_session_aggr_max_bit_rate_ul;
  }

  // id-UL-NGU-UP-TNLInformation
  setup_item.ul_ngu_up_tnl_info = asn1_to_up_transport_layer_info(asn1_setup_req_transfer->ul_ngu_up_tnl_info);

  // id-PDUSessionType
  setup_item.pdu_session_type = asn1_setup_req_transfer->pdu_session_type.to_string();

  // id-SecurityIndication
  if (asn1_setup_req_transfer->security_ind_present) {
    security_indication_t ind = {};
    asn1_to_security_indication(ind, asn1_setup_req_transfer->security_ind);
    setup_item.security_ind = ind;
  }

  // id-QosFlowSetupRequestList
  for (const auto& asn1_flow_item : asn1_setup_req_transfer->qos_flow_setup_request_list) {
    qos_flow_setup_request_item qos_flow_setup_req_item;

    // qosFlowIdentifier
    qos_flow_setup_req_item.qos_flow_id = uint_to_qos_flow_id(asn1_flow_item.qos_flow_id);

    // qosFlowLevelQosParameters
    if (asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.type() ==
        asn1::ngap::qos_characteristics_c::types::dyn5qi) {
      dyn_5qi_descriptor dyn_5qi = {};
      if (asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().five_qi_present) {
        dyn_5qi.five_qi =
            uint_to_five_qi(asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().five_qi);
      }
      // TODO: Add optional values

      qos_flow_setup_req_item.qos_flow_level_qos_params.qos_desc = dyn_5qi;

      // TODO: Add optional values

    } else if (asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.type() ==
               asn1::ngap::qos_characteristics_c::types::non_dyn5qi) {
      non_dyn_5qi_descriptor non_dyn_5qi = {};
      non_dyn_5qi.five_qi =
          uint_to_five_qi(asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.non_dyn5qi().five_qi);
      qos_flow_setup_req_item.qos_flow_level_qos_params.qos_desc = non_dyn_5qi;

      // TODO: Add optional values
    }

    // allocationAndRetentionPriority
    qos_flow_setup_req_item.qos_flow_level_qos_params.alloc_retention_prio.prio_level_arp =
        asn1_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp;
    qos_flow_setup_req_item.qos_flow_level_qos_params.alloc_retention_prio.may_trigger_preemption =
        asn1_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap ==
        asn1::ngap::pre_emption_cap_opts::may_trigger_pre_emption;
    qos_flow_setup_req_item.qos_flow_level_qos_params.alloc_retention_prio.is_preemptable =
        asn1_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability ==
        asn1::ngap::pre_emption_vulnerability_opts::pre_emptable;

    // Optional Parameters
    if (asn1_flow_item.qos_flow_level_qos_params.add_qos_flow_info_present) {
      qos_flow_setup_req_item.qos_flow_level_qos_params.add_qos_flow_info =
          asn1_flow_item.qos_flow_level_qos_params.add_qos_flow_info.to_string();
    }

    if (asn1_flow_item.qos_flow_level_qos_params.gbr_qos_info_present) {
      // TODO: Add to common type
    }

    if (asn1_flow_item.qos_flow_level_qos_params.reflective_qos_attribute_present) {
      qos_flow_setup_req_item.qos_flow_level_qos_params.reflective_qos_attribute_subject_to = true;
    }

    if (asn1_flow_item.erab_id_present) {
      qos_flow_setup_req_item.erab_id = asn1_flow_item.erab_id;
    }

    setup_item.qos_flow_setup_request_items.emplace(qos_flow_setup_req_item.qos_flow_id, qos_flow_setup_req_item);
  }
  return true;
}

/// \brief Convert NGAP ASN1 PDU Session Resource Setup List SU REQ  ASN1 struct to common type.
/// \param[out] cu_cp_pdu_session_res_setup_msg The cu_cp_pdu_session_res_setup_msg struct to fill.
/// \param[in] asn1_pdu_session_res_setup_list The pdu_session_res_setup_list_su_req ASN1 struct.
/// \returns True if the conversion was successful, false otherwise.
inline bool fill_cu_cp_pdu_session_resource_setup_request(
    cu_cp_pdu_session_resource_setup_request& cu_cp_pdu_session_resource_setup_msg,
    const asn1::dyn_seq_of<asn1::ngap::pdu_session_res_setup_item_su_req_s, 1U, 256U, true>&
        asn1_pdu_session_res_setup_list)
{
  if (asn1_pdu_session_res_setup_list.size() == 0) {
    return false;
  }

  for (const auto& asn1_session_item : asn1_pdu_session_res_setup_list) {
    cu_cp_pdu_session_res_setup_item setup_item;

    if (!fill_cu_cp_pdu_session_resource_setup_item_base(
            setup_item, asn1_session_item, asn1_session_item.pdu_session_res_setup_request_transfer.copy())) {
      return false;
    }

    // pDUSessionNAS-PDU
    if (!asn1_session_item.pdu_session_nas_pdu.empty()) {
      if (!setup_item.pdu_session_nas_pdu.resize(asn1_session_item.pdu_session_nas_pdu.size())) {
        return false;
      }
      std::copy(asn1_session_item.pdu_session_nas_pdu.begin(),
                asn1_session_item.pdu_session_nas_pdu.end(),
                setup_item.pdu_session_nas_pdu.begin());
    }

    cu_cp_pdu_session_resource_setup_msg.pdu_session_res_setup_items.emplace(setup_item.pdu_session_id,
                                                                             std::move(setup_item));
  }
  return true;
}

/// \brief Convert NGAP ASN1 PDU Session Resource Setup List CTX REQ
/// ASN1 struct to common type.
/// \param[out] cu_cp_pdu_session_res_setup_msg The cu_cp_pdu_session_res_setup_msg struct to fill.
/// \param[in] asn1_pdu_session_res_setup_list The pdu_session_res_setup_item_cxt_req_s ASN1 struct.
/// \returns True if the conversion was successful, false otherwise.
inline bool fill_cu_cp_pdu_session_resource_setup_request(
    cu_cp_pdu_session_resource_setup_request& cu_cp_pdu_session_resource_setup_msg,
    const asn1::dyn_seq_of<asn1::ngap::pdu_session_res_setup_item_cxt_req_s, 1U, 256U, true>&
        asn1_pdu_session_res_setup_list)
{
  for (const auto& asn1_session_item : asn1_pdu_session_res_setup_list) {
    cu_cp_pdu_session_res_setup_item setup_item;

    if (!fill_cu_cp_pdu_session_resource_setup_item_base(
            setup_item, asn1_session_item, asn1_session_item.pdu_session_res_setup_request_transfer.copy())) {
      return false;
    }

    // NAS-PDU
    if (!asn1_session_item.nas_pdu.empty()) {
      if (!setup_item.pdu_session_nas_pdu.resize(asn1_session_item.nas_pdu.size())) {
        return false;
      }
      std::copy(
          asn1_session_item.nas_pdu.begin(), asn1_session_item.nas_pdu.end(), setup_item.pdu_session_nas_pdu.begin());
    }

    cu_cp_pdu_session_resource_setup_msg.pdu_session_res_setup_items.emplace(setup_item.pdu_session_id,
                                                                             std::move(setup_item));
  }
  return true;
}

/// \brief Convert NGAP ASN1 Init Context Setup Request ASN1 struct to common type.
/// \param[out] request The ngap_init_context_setup_request struct to fill.
/// \param[in] asn1_request The Init Context Setup Request ASN1 struct.
inline bool fill_ngap_initial_context_setup_request(ngap_init_context_setup_request&                request,
                                                    const asn1::ngap::init_context_setup_request_s& asn1_request)
{
  // old_amf
  if (asn1_request->old_amf_present) {
    request.old_amf = asn1_request->old_amf.to_string();
  }

  // ue_aggr_max_bit_rate
  if (asn1_request->ue_aggr_max_bit_rate_present) {
    request.ue_aggr_max_bit_rate.emplace();
    request.ue_aggr_max_bit_rate.value().ue_aggr_max_bit_rate_dl =
        asn1_request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_dl;
    request.ue_aggr_max_bit_rate.value().ue_aggr_max_bit_rate_ul =
        asn1_request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_ul;
  }

  // guami
  request.guami = asn1_to_guami(asn1_request->guami);

  // pdu_session_res_setup_list_cxt_req
  if (asn1_request->pdu_session_res_setup_list_cxt_req_present) {
    request.pdu_session_res_setup_list_cxt_req.emplace();
    if (!fill_cu_cp_pdu_session_resource_setup_request(request.pdu_session_res_setup_list_cxt_req.value(),
                                                       asn1_request->pdu_session_res_setup_list_cxt_req)) {
      return false;
    }
  }

  // allowed_nssai
  for (const auto& asn1_s_nssai : asn1_request->allowed_nssai) {
    request.allowed_nssai.push_back(ngap_asn1_to_s_nssai(asn1_s_nssai.s_nssai));
  }

  // security_context
  copy_asn1_key(request.security_context.k, asn1_request->security_key);
  fill_supported_algorithms(request.security_context.supported_int_algos,
                            asn1_request->ue_security_cap.nr_integrity_protection_algorithms);
  fill_supported_algorithms(request.security_context.supported_enc_algos,
                            asn1_request->ue_security_cap.nr_encryption_algorithms);

  // ue_radio_cap
  if (asn1_request->ue_radio_cap_present) {
    request.ue_radio_cap = asn1_request->ue_radio_cap.copy();
  }

  // idx_to_rfsp
  if (asn1_request->idx_to_rfsp_present) {
    request.idx_to_rfsp = asn1_request->idx_to_rfsp;
  }

  // masked_imeisv
  if (asn1_request->masked_imeisv_present) {
    request.masked_imeisv = asn1_request->masked_imeisv.to_number();
  }

  // nas_pdu
  if (asn1_request->nas_pdu_present) {
    request.nas_pdu = asn1_request->nas_pdu.copy();
  }

  // ue_radio_cap_for_paging
  if (asn1_request->ue_radio_cap_for_paging_present) {
    cu_cp_ue_radio_cap_for_paging ue_radio_cap_for_paging;
    ue_radio_cap_for_paging.ue_radio_cap_for_paging_of_nr =
        asn1_request->ue_radio_cap_for_paging.ue_radio_cap_for_paging_of_nr.copy();

    request.ue_radio_cap_for_paging = ue_radio_cap_for_paging;
  }

  // TODO: Add missing optional values

  return true;
}

/// \brief Convert common type Initial Context Setup Response message to NGAP Initial Context Setup Response message.
/// \param[out] asn1_resp The ASN1 NGAP Initial Context Setup Response message.
/// \param[in] resp The CU-CP Initial Context Setup Response message.
/// \return True on success, otherwise false.
inline bool fill_asn1_initial_context_setup_response(asn1::ngap::init_context_setup_resp_s&  asn1_resp,
                                                     const ngap_init_context_setup_response& resp)
{
  // Fill PDU Session Resource Setup Response List
  if (!resp.pdu_session_res_setup_response_items.empty()) {
    asn1_resp->pdu_session_res_setup_list_cxt_res_present = true;

    for (const auto& resp_item : resp.pdu_session_res_setup_response_items) {
      asn1::ngap::pdu_session_res_setup_item_cxt_res_s asn1_resp_item;

      if (!pdu_session_res_setup_response_item_to_asn1(asn1_resp_item, resp_item)) {
        return false;
      }

      asn1_resp->pdu_session_res_setup_list_cxt_res.push_back(asn1_resp_item);
    }
  }

  // Fill PDU Session Resource Failed to Setup List
  if (!resp.pdu_session_res_failed_to_setup_items.empty()) {
    asn1_resp->pdu_session_res_failed_to_setup_list_cxt_res_present = true;
    for (const auto& setup_failed_item : resp.pdu_session_res_failed_to_setup_items) {
      asn1::ngap::pdu_session_res_failed_to_setup_item_cxt_res_s asn1_setup_failed_item;

      if (!pdu_session_res_setup_failed_item_to_asn1(asn1_setup_failed_item, setup_failed_item)) {
        return false;
      }

      asn1_resp->pdu_session_res_failed_to_setup_list_cxt_res.push_back(asn1_setup_failed_item);
    }
  }

  // Fill Criticality Diagnostics
  if (resp.crit_diagnostics.has_value()) {
    // TODO: Add crit diagnostics
  }

  return true;
}

/// \brief Convert common type Initial Context Setup Failure message to NGAP Initial Context Setup Failure
/// message.
/// \param[out] asn1_fail The ASN1 NGAP Initial Context Setup Failure message.
/// \param[in] fail The CU-CP Initial Context Setup Failure message.
inline void fill_asn1_initial_context_setup_failure(asn1::ngap::init_context_setup_fail_s& asn1_fail,
                                                    const ngap_init_context_setup_failure& fail)
{
  // Fill cause
  asn1_fail->cause = cause_to_asn1(fail.cause);

  // Fill PDU Session Resource Failed to Setup List
  if (!fail.pdu_session_res_failed_to_setup_items.empty()) {
    asn1_fail->pdu_session_res_failed_to_setup_list_cxt_fail_present = true;
    for (const auto& setup_failed_item : fail.pdu_session_res_failed_to_setup_items) {
      asn1::ngap::pdu_session_res_failed_to_setup_item_cxt_fail_s asn1_setup_failed_item;

      pdu_session_res_setup_failed_item_to_asn1(asn1_setup_failed_item, setup_failed_item);

      asn1_fail->pdu_session_res_failed_to_setup_list_cxt_fail.push_back(asn1_setup_failed_item);
    }
  }

  // Fill Criticality Diagnostics
  if (fail.crit_diagnostics.has_value()) {
    // TODO: Add crit diagnostics
  }
}

/// \brief Convert a NGAP ASN1 modify item to common type.
/// \param[out] modify_item The flat/common version
/// \param[in] asn1_session_item The ASN1 struct to be converted.
/// \return True on success, otherwise false.
inline bool fill_cu_cp_pdu_session_resource_modify_item_base(
    cu_cp_pdu_session_res_modify_item_mod_req&               modify_item,
    const asn1::ngap::pdu_session_res_modify_item_mod_req_s& asn1_session_item)
{
  modify_item.pdu_session_id = uint_to_pdu_session_id(asn1_session_item.pdu_session_id);

  asn1::ngap::pdu_session_res_modify_request_transfer_s asn1_modify_req_transfer;
  asn1::cbit_ref                                        bref(asn1_session_item.pdu_session_res_modify_request_transfer);
  if (asn1_modify_req_transfer.unpack(bref) != asn1::SRSASN_SUCCESS) {
    srslog::fetch_basic_logger("NGAP").error("Couldn't unpack PDU Session Resource Modify Request Transfer PDU.");
    return false;
  }

  if (asn1_modify_req_transfer->qos_flow_add_or_modify_request_list_present) {
    for (const auto& asn1_flow_item : asn1_modify_req_transfer->qos_flow_add_or_modify_request_list) {
      cu_cp_qos_flow_add_or_mod_item qos_flow_add_item;

      // qosFlowIdentifier
      qos_flow_add_item.qos_flow_id = uint_to_qos_flow_id(asn1_flow_item.qos_flow_id);

      // qosFlowLevelQosParameters
      if (asn1_flow_item.qos_flow_level_qos_params_present) {
        if (asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.type() ==
            asn1::ngap::qos_characteristics_c::types::dyn5qi) {
          dyn_5qi_descriptor dyn_5qi = {};
          if (asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().five_qi_present) {
            dyn_5qi.five_qi =
                uint_to_five_qi(asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.dyn5qi().five_qi);
          }
          // TODO: Add optional values

          qos_flow_add_item.qos_flow_level_qos_params.qos_desc = dyn_5qi;

          // TODO: Add optional values

        } else if (asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.type() ==
                   asn1::ngap::qos_characteristics_c::types::non_dyn5qi) {
          non_dyn_5qi_descriptor non_dyn_5qi = {};
          non_dyn_5qi.five_qi =
              uint_to_five_qi(asn1_flow_item.qos_flow_level_qos_params.qos_characteristics.non_dyn5qi().five_qi);
          qos_flow_add_item.qos_flow_level_qos_params.qos_desc = non_dyn_5qi;

          // TODO: Add optional values
        }

        // allocationAndRetentionPriority
        qos_flow_add_item.qos_flow_level_qos_params.alloc_retention_prio.prio_level_arp =
            asn1_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.prio_level_arp;
        qos_flow_add_item.qos_flow_level_qos_params.alloc_retention_prio.may_trigger_preemption =
            asn1_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_cap.value ==
            asn1::ngap::pre_emption_cap_opts::may_trigger_pre_emption;
        qos_flow_add_item.qos_flow_level_qos_params.alloc_retention_prio.is_preemptable =
            asn1_flow_item.qos_flow_level_qos_params.alloc_and_retention_prio.pre_emption_vulnerability.value ==
            asn1::ngap::pre_emption_vulnerability_opts::pre_emptable;
      }

      modify_item.transfer.qos_flow_add_or_modify_request_list.emplace(qos_flow_add_item.qos_flow_id,
                                                                       qos_flow_add_item);
    }
  }

  if (asn1_modify_req_transfer->qos_flow_to_release_list_present) {
    for (const auto& asn1_flow_item : asn1_modify_req_transfer->qos_flow_to_release_list) {
      cu_cp_qos_flow_with_cause_item qos_flow_release_item;
      qos_flow_release_item.qos_flow_id = uint_to_qos_flow_id(asn1_flow_item.qos_flow_id);
      qos_flow_release_item.cause       = asn1_to_cause(asn1_flow_item.cause);
      modify_item.transfer.qos_flow_to_release_list.emplace(qos_flow_release_item.qos_flow_id, qos_flow_release_item);
    }
  }

  if (!asn1_session_item.nas_pdu.empty()) {
    if (!modify_item.nas_pdu.resize(asn1_session_item.nas_pdu.size())) {
      return false;
    }
    std::copy(asn1_session_item.nas_pdu.begin(), asn1_session_item.nas_pdu.end(), modify_item.nas_pdu.begin());
  }

  return true;
}

/// \brief Convert NGAP ASN1 PDU Session Resource Modify List ASN1 struct to common type.
/// \param[out] cu_cp_pdu_session_resource_modify_msg The cu_cp_pdu_session_res_modify_msg struct to fill.
/// \param[in] asn1_pdu_session_res_modify_list The pdu_session_res_modify_list ASN1 struct.
/// \return True on success, otherwise false.
inline bool fill_cu_cp_pdu_session_resource_modify_request(
    cu_cp_pdu_session_resource_modify_request& cu_cp_pdu_session_resource_modify_msg,
    const asn1::dyn_seq_of<asn1::ngap::pdu_session_res_modify_item_mod_req_s, 1, 256, true>&
        asn1_pdu_session_res_modify_list)
{
  for (const auto& asn1_session_item : asn1_pdu_session_res_modify_list) {
    cu_cp_pdu_session_res_modify_item_mod_req modify_item;
    if (!fill_cu_cp_pdu_session_resource_modify_item_base(modify_item, asn1_session_item)) {
      return false;
    }
    cu_cp_pdu_session_resource_modify_msg.pdu_session_res_modify_items.emplace(modify_item.pdu_session_id,
                                                                               std::move(modify_item));
  }

  return true;
}

/// \brief Convert common type PDU Session Resource Setup Response message to NGAP PDU Session Resource Setup Response
/// message.
/// \param[out] resp The ASN1 NGAP PDU Session Resource Setup Response message.
/// \param[in] cu_cp_resp The CU-CP PDU Session Resource Setup Response message.
inline void fill_asn1_pdu_session_res_setup_response(asn1::ngap::pdu_session_res_setup_resp_s&        resp,
                                                     const cu_cp_pdu_session_resource_setup_response& cu_cp_resp)
{
  // Fill PDU Session Resource Setup Response List
  if (!cu_cp_resp.pdu_session_res_setup_response_items.empty()) {
    resp->pdu_session_res_setup_list_su_res_present = true;

    for (const auto& cu_cp_resp_item : cu_cp_resp.pdu_session_res_setup_response_items) {
      asn1::ngap::pdu_session_res_setup_item_su_res_s resp_item;

      pdu_session_res_setup_response_item_to_asn1(resp_item, cu_cp_resp_item);

      resp->pdu_session_res_setup_list_su_res.push_back(resp_item);
    }
  }

  // Fill PDU Session Resource Failed to Setup List
  if (!cu_cp_resp.pdu_session_res_failed_to_setup_items.empty()) {
    resp->pdu_session_res_failed_to_setup_list_su_res_present = true;
    for (const auto& cu_cp_setup_failed_item : cu_cp_resp.pdu_session_res_failed_to_setup_items) {
      srsran_assert(!cu_cp_setup_failed_item.unsuccessful_transfer.cause.valueless_by_exception(),
                    "Failed cause must not be null.");
      asn1::ngap::pdu_session_res_failed_to_setup_item_su_res_s setup_failed_item;

      pdu_session_res_setup_failed_item_to_asn1(setup_failed_item, cu_cp_setup_failed_item);

      resp->pdu_session_res_failed_to_setup_list_su_res.push_back(setup_failed_item);
    }
  }
}

/// \brief Convert common type UE Context Release Request to NGAP UE Context Release Request.
/// \param[out] asn1_msg The ASN1 NGAP UE Context Release Request.
/// \param[in] msg The CU-CP UE Context Release Request.
inline void fill_asn1_ue_context_release_request(asn1::ngap::ue_context_release_request_s& asn1_msg,
                                                 const cu_cp_ue_context_release_request&   msg)
{
  // Add PDU Session IDs
  if (!msg.pdu_session_res_list_cxt_rel_req.empty()) {
    asn1_msg->pdu_session_res_list_cxt_rel_req_present = true;
    for (const auto& session_id : msg.pdu_session_res_list_cxt_rel_req) {
      asn1::ngap::pdu_session_res_item_cxt_rel_req_s pdu_session_item;
      pdu_session_item.pdu_session_id = pdu_session_id_to_uint(session_id);
      asn1_msg->pdu_session_res_list_cxt_rel_req.push_back(pdu_session_item);
    }
  }

  asn1_msg->cause = cause_to_asn1(msg.cause);
}

/// \brief Convert NGAP ASN1 PDU Session Resource Release Command ASN1 struct to common type.
/// \param[out] pdu_session_resource_release_cmd The cu_cp_pdu_session_resource_release_command struct to fill.
/// \param[in] asn1_pdu_session_resource_release_cmd The pdu_session_res_release_cmd ASN1 struct.
inline void fill_cu_cp_pdu_session_resource_release_command(
    cu_cp_pdu_session_resource_release_command&      pdu_session_resource_release_cmd,
    const asn1::ngap::pdu_session_res_release_cmd_s& asn1_pdu_session_resource_release_cmd)
{
  if (asn1_pdu_session_resource_release_cmd->ran_paging_prio_present) {
    pdu_session_resource_release_cmd.ran_paging_prio = asn1_pdu_session_resource_release_cmd->ran_paging_prio;
  }

  if (asn1_pdu_session_resource_release_cmd->nas_pdu_present) {
    pdu_session_resource_release_cmd.nas_pdu = asn1_pdu_session_resource_release_cmd->nas_pdu.copy();
  }

  for (const auto& pdu_session_res_to_release_item :
       asn1_pdu_session_resource_release_cmd->pdu_session_res_to_release_list_rel_cmd) {
    cu_cp_pdu_session_res_to_release_item_rel_cmd pdu_session_res_to_release_item_rel_cmd;
    pdu_session_res_to_release_item_rel_cmd.pdu_session_id =
        uint_to_pdu_session_id(pdu_session_res_to_release_item.pdu_session_id);

    asn1::ngap::pdu_session_res_release_cmd_transfer_s asn1_pdu_session_res_release_cmd_transfer;
    asn1::cbit_ref bref(pdu_session_res_to_release_item.pdu_session_res_release_cmd_transfer);

    if (asn1_pdu_session_res_release_cmd_transfer.unpack(bref) != asn1::SRSASN_SUCCESS) {
      srslog::fetch_basic_logger("NGAP").error("Couldn't unpack PDU Session Resource Release Command Transfer PDU.");
      return;
    }

    pdu_session_res_to_release_item_rel_cmd.pdu_session_res_release_cmd_transfer.cause =
        asn1_to_cause(asn1_pdu_session_res_release_cmd_transfer.cause);

    pdu_session_resource_release_cmd.pdu_session_res_to_release_list_rel_cmd.emplace(
        pdu_session_res_to_release_item_rel_cmd.pdu_session_id, pdu_session_res_to_release_item_rel_cmd);
  }
}

/// \brief Convert NGAP ASN1 UE Context Release Command ASN1 struct to common type.
/// \param[out] cu_cp_ngap_ue_context_release_cmd The cu_cp_ngap_ue_context_release_cmd struct to fill.
/// \param[in] asn1_ue_context_release_cmd The UE Context Release Command ASN1 struct.
inline void
fill_cu_cp_ue_context_release_command(cu_cp_ue_context_release_command&           cu_cp_ue_context_release_cmd,
                                      const asn1::ngap::ue_context_release_cmd_s& asn1_ue_context_release_cmd)
{
  cu_cp_ue_context_release_cmd.cause = asn1_to_cause(asn1_ue_context_release_cmd->cause);
}

/// \brief Convert common type UE Context Release Complete message to NGAP ASN1 UE Context Release Complete
/// message.
/// \param[out] asn1_resp The ASN1 NGAP UE Context Release Complete message.
/// \param[in] cu_cp_resp The CU-CP UE Context Release Complete message.
inline void fill_asn1_ue_context_release_complete(asn1::ngap::ue_context_release_complete_s& asn1_resp,
                                                  const cu_cp_ue_context_release_complete&   cu_cp_resp)
{
  // add user location info
  if (cu_cp_resp.user_location_info.has_value()) {
    asn1_resp->user_location_info_present = true;
    asn1_resp->user_location_info.set_user_location_info_nr() =
        cu_cp_user_location_info_to_asn1(cu_cp_resp.user_location_info.value());
  }

  // add info on recommended cells and ran nodes for paging
  if (cu_cp_resp.info_on_recommended_cells_and_ran_nodes_for_paging.has_value()) {
    asn1_resp->info_on_recommended_cells_and_ran_nodes_for_paging_present = true;

    for (auto cu_cp_recommended_cell_item : cu_cp_resp.info_on_recommended_cells_and_ran_nodes_for_paging.value()
                                                .recommended_cells_for_paging.recommended_cell_list) {
      asn1::ngap::recommended_cell_item_s asn1_recommended_cell_item;

      // add ngran cgi
      asn1_recommended_cell_item.ngran_cgi.set_nr_cgi().nr_cell_id.from_number(
          cu_cp_recommended_cell_item.ngran_cgi.nci.value());
      asn1_recommended_cell_item.ngran_cgi.set_nr_cgi().plmn_id =
          cu_cp_recommended_cell_item.ngran_cgi.plmn_id.to_bytes();

      // add time stayed in cell
      if (cu_cp_recommended_cell_item.time_stayed_in_cell.has_value()) {
        asn1_recommended_cell_item.time_stayed_in_cell_present = true;
        asn1_recommended_cell_item.time_stayed_in_cell = cu_cp_recommended_cell_item.time_stayed_in_cell.value();
      }

      asn1_resp->info_on_recommended_cells_and_ran_nodes_for_paging.recommended_cells_for_paging.recommended_cell_list
          .push_back(asn1_recommended_cell_item);
    }

    for (auto cu_cp_recommended_ran_node_item : cu_cp_resp.info_on_recommended_cells_and_ran_nodes_for_paging.value()
                                                    .recommended_ran_nodes_for_paging.recommended_ran_node_list) {
      asn1::ngap::recommended_ran_node_item_s asn1_recommended_ran_node_item;

      // add amf paging target
      if (cu_cp_recommended_ran_node_item.amf_paging_target.is_global_ran_node_id) {
        // add global gnb id
        auto& asn1_global_ran_node_id = asn1_recommended_ran_node_item.amf_paging_target.set_global_ran_node_id();
        auto& global_gnb              = asn1_global_ran_node_id.set_global_gnb_id();
        global_gnb.plmn_id =
            cu_cp_recommended_ran_node_item.amf_paging_target.global_ran_node_id.value().plmn_id.to_bytes();
        global_gnb.gnb_id.set_gnb_id().from_number(
            cu_cp_recommended_ran_node_item.amf_paging_target.global_ran_node_id.value().gnb_id.id,
            cu_cp_recommended_ran_node_item.amf_paging_target.global_ran_node_id.value().gnb_id.bit_length);
      } else if (cu_cp_recommended_ran_node_item.amf_paging_target.is_tai) {
        // add tai
        auto& asn1_tai   = asn1_recommended_ran_node_item.amf_paging_target.set_tai();
        asn1_tai.plmn_id = cu_cp_recommended_ran_node_item.amf_paging_target.tai.value().plmn_id.to_bytes();
        asn1_tai.tac.from_number(cu_cp_recommended_ran_node_item.amf_paging_target.tai.value().tac);
      } else {
        asn1_recommended_ran_node_item.amf_paging_target.set(asn1::ngap::amf_paging_target_c::types_opts::nulltype);
      }

      asn1_resp->info_on_recommended_cells_and_ran_nodes_for_paging.recommend_ran_nodes_for_paging
          .recommended_ran_node_list.push_back(asn1_recommended_ran_node_item);
    }
  }

  // add pdu session res list context release complete
  if (!cu_cp_resp.pdu_session_res_list_cxt_rel_cpl.empty()) {
    asn1_resp->pdu_session_res_list_cxt_rel_cpl_present = true;

    for (auto pdu_session_id : cu_cp_resp.pdu_session_res_list_cxt_rel_cpl) {
      asn1::ngap::pdu_session_res_item_cxt_rel_cpl_s asn1_rel_item;
      asn1_rel_item.pdu_session_id = pdu_session_id_to_uint(pdu_session_id);
      asn1_resp->pdu_session_res_list_cxt_rel_cpl.push_back(asn1_rel_item);
    }
  }

  // add crit diagnostics
  if (cu_cp_resp.crit_diagnostics.has_value()) {
    // TODO: Add crit diagnostics
  }
}

/// \brief Convert NGAP ASN1 Paging ASN1 struct to common type.
/// \param[out] paging The cu_cp_paging_message struct to fill.
/// \param[in] asn1_paging The Paging ASN1 struct.
inline void fill_cu_cp_paging_message(cu_cp_paging_message& paging, const asn1::ngap::paging_s& asn1_paging)
{
  // add ue paging id
  paging.ue_paging_id = ngap_asn1_to_ue_paging_id(asn1_paging->ue_paging_id);

  // add paging drx
  if (asn1_paging->paging_drx_present) {
    paging.paging_drx = asn1_paging->paging_drx.to_number();
  }

  // add tai list for paging
  for (const auto& asn1_tai_item : asn1_paging->tai_list_for_paging) {
    cu_cp_tai_list_for_paging_item tai_item;
    tai_item.tai.plmn_id = plmn_identity::from_bytes(asn1_tai_item.tai.plmn_id.to_bytes()).value();
    tai_item.tai.tac     = asn1_tai_item.tai.tac.to_number();

    paging.tai_list_for_paging.push_back(tai_item);
  }

  // add paging prio
  if (asn1_paging->paging_prio_present) {
    paging.paging_prio = asn1_paging->paging_prio.to_number();
  }

  // add ue radio cap for paging
  if (asn1_paging->ue_radio_cap_for_paging_present) {
    cu_cp_ue_radio_cap_for_paging ue_radio_cap_for_paging;
    ue_radio_cap_for_paging.ue_radio_cap_for_paging_of_nr =
        asn1_paging->ue_radio_cap_for_paging.ue_radio_cap_for_paging_of_nr.copy();

    paging.ue_radio_cap_for_paging = ue_radio_cap_for_paging;
  }

  // add paging origin
  if (asn1_paging->paging_origin_present) {
    paging.paging_origin = asn1_paging->paging_origin.to_string();
  }

  // add assist data for paging
  if (asn1_paging->assist_data_for_paging_present) {
    cu_cp_assist_data_for_paging assist_data_for_paging;

    // add assist data for recommended cells
    if (asn1_paging->assist_data_for_paging.assist_data_for_recommended_cells_present) {
      cu_cp_assist_data_for_recommended_cells assist_data_for_recommended_cells;

      for (const auto& asn1_recommended_cell : asn1_paging->assist_data_for_paging.assist_data_for_recommended_cells
                                                   .recommended_cells_for_paging.recommended_cell_list) {
        cu_cp_recommended_cell_item recommended_cell_item;

        // add ngran cgi
        recommended_cell_item.ngran_cgi.nci =
            nr_cell_identity::create(asn1_recommended_cell.ngran_cgi.nr_cgi().nr_cell_id.to_number()).value();
        recommended_cell_item.ngran_cgi.plmn_id =
            plmn_identity::from_bytes(asn1_recommended_cell.ngran_cgi.nr_cgi().plmn_id.to_bytes()).value();

        // add time stayed in cell
        if (asn1_recommended_cell.time_stayed_in_cell_present) {
          recommended_cell_item.time_stayed_in_cell = asn1_recommended_cell.time_stayed_in_cell;
        }

        assist_data_for_recommended_cells.recommended_cells_for_paging.recommended_cell_list.push_back(
            recommended_cell_item);
      }

      assist_data_for_paging.assist_data_for_recommended_cells = assist_data_for_recommended_cells;
    }

    // add paging attempt info
    if (asn1_paging->assist_data_for_paging.paging_attempt_info_present) {
      cu_cp_paging_attempt_info paging_attempt_info;

      paging_attempt_info.paging_attempt_count =
          asn1_paging->assist_data_for_paging.paging_attempt_info.paging_attempt_count;
      paging_attempt_info.intended_nof_paging_attempts =
          asn1_paging->assist_data_for_paging.paging_attempt_info.intended_nof_paging_attempts;

      if (asn1_paging->assist_data_for_paging.paging_attempt_info.next_paging_area_scope_present) {
        paging_attempt_info.next_paging_area_scope =
            asn1_paging->assist_data_for_paging.paging_attempt_info.next_paging_area_scope.to_string();
      }

      assist_data_for_paging.paging_attempt_info = paging_attempt_info;
    }

    paging.assist_data_for_paging = assist_data_for_paging;
  }
}

/// \brief Convert Handover Request ASN1 struct to common type.
/// \param[out] request The ngap_handover_request struct to fill.
/// \param[in] asn1_request The Handover Request ASN1 struct.
/// \returns True if the conversion was successful, false otherwise.
inline bool fill_ngap_handover_request(ngap_handover_request& request, const asn1::ngap::ho_request_s& asn1_request)
{
  // handov type
  asn1_to_handov_type(request.handov_type, asn1_request->handov_type);

  // cause
  request.cause = asn1_to_cause(asn1_request->cause);

  // ue aggr max bit rate
  request.ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_dl = asn1_request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_dl;
  request.ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_ul = asn1_request->ue_aggr_max_bit_rate.ue_aggr_max_bit_rate_ul;

  // core network assist info for inactive
  // TODO

  // ue security cap & security context
  asn1_to_security_context(request.security_context, asn1_request->ue_security_cap, asn1_request->security_context);

  // new security context ind
  if (asn1_request->new_security_context_ind_present) {
    request.new_security_context_ind = asn1::enum_to_bool(asn1_request->new_security_context_ind);
  }

  // nasc
  if (asn1_request->nasc_present) {
    request.nasc = asn1_request->nasc.copy();
  }

  // pdu session res setup list ho req
  for (const auto& asn1_pdu_session_res_setup_item : asn1_request->pdu_session_res_setup_list_ho_req) {
    cu_cp_pdu_session_res_setup_item pdu_session_res_setup_item;
    if (!fill_cu_cp_pdu_session_resource_setup_item_base(pdu_session_res_setup_item,
                                                         asn1_pdu_session_res_setup_item,
                                                         asn1_pdu_session_res_setup_item.ho_request_transfer.copy())) {
      srslog::fetch_basic_logger("NGAP").error("Couldn't convert PDU Session Resource Setup List HO Request.");
      return false;
    }

    request.pdu_session_res_setup_list_ho_req.emplace(pdu_session_res_setup_item.pdu_session_id,
                                                      pdu_session_res_setup_item);
  }

  // allowed nssai
  for (const auto& asn1_s_nssai : asn1_request->allowed_nssai) {
    request.allowed_nssai.push_back(ngap_asn1_to_s_nssai(asn1_s_nssai.s_nssai));
  }

  // trace activation
  // TODO

  // masked imeisv
  if (asn1_request->masked_imeisv_present) {
    request.masked_imeisv = asn1_request->masked_imeisv.to_number();
  }

  // source to target transparent container
  asn1::cbit_ref bref(asn1_request->source_to_target_transparent_container);
  asn1::ngap::source_ngran_node_to_target_ngran_node_transparent_container_s asn1_transparent_container;
  if (asn1_transparent_container.unpack(bref) != asn1::SRSASN_SUCCESS) {
    srslog::fetch_basic_logger("NGAP").error("Couldn't unpack Source to Target Transparent Container.");
    return false;
  }
  asn1_to_source_to_target_transport_container(request.source_to_target_transparent_container,
                                               asn1_transparent_container);

  // mob restrict list
  // TODO

  // location report request type
  // TODO

  // rrc inactive transition report request
  // TODO

  // guami
  request.guami = asn1_to_guami(asn1_request->guami);

  // redirection voice fallback
  // TODO

  // cn assisted ran tuning
  // TODO

  return true;
}

/// \brief Convert \c ngap_handover_resource_allocation_response common type struct to ASN.1.
/// \param[out] asn1_ho_request_ack The Handover Request Ack ASN1 struct to fill.
/// \param[in] ho_response The ngap_handover_resource_allocation_response common type struct.
/// \returns True if the conversion was successful, false otherwise.
inline bool
fill_asn1_handover_resource_allocation_response(asn1::ngap::ho_request_ack_s&                     asn1_ho_request_ack,
                                                const ngap_handover_resource_allocation_response& ho_response)
{
  if (ho_response.success) {
    // pdu session res admitted list
    for (const auto& admitted_item : ho_response.pdu_session_res_admitted_list) {
      asn1::ngap::pdu_session_res_admitted_item_s asn1_admitted_item;
      if (!pdu_session_res_admitted_item_to_asn1(asn1_admitted_item, admitted_item)) {
        return false;
      }
      asn1_ho_request_ack->pdu_session_res_admitted_list.push_back(asn1_admitted_item);
    }

    // pdu session res failed to setup list ho ack
    if (!ho_response.pdu_session_res_failed_to_setup_list_ho_ack.empty()) {
      asn1_ho_request_ack->pdu_session_res_failed_to_setup_list_ho_ack_present = true;
      for (const auto& failed_item : ho_response.pdu_session_res_failed_to_setup_list_ho_ack) {
        asn1::ngap::pdu_session_res_failed_to_setup_item_ho_ack_s asn1_failed_item;
        if (!pdu_session_res_failed_to_setup_item_ho_ack_to_asn1(asn1_failed_item, failed_item)) {
          return false;
        }
        asn1_ho_request_ack->pdu_session_res_failed_to_setup_list_ho_ack.push_back(asn1_failed_item);
      }
    }

    // target to source transparent container
    if (!target_to_source_transport_container_to_asn1(asn1_ho_request_ack->target_to_source_transparent_container,
                                                      ho_response.target_to_source_transparent_container)) {
      return false;
    }

    // crit diagnostics
    if (ho_response.crit_diagnostics.has_value()) {
      // TODO: Add crit diagnostics
    }
  } else {
    return false;
  }

  return true;
}

/// \brief Convert \c ngap_handover_resource_allocation_response common type struct to ASN.1.
/// \param[out] asn1_ho_failure The Handover Request Failure ASN1 struct to fill.
/// \param[in] ho_response The ngap_handover_resource_allocation_response common type struct.
/// \returns True if the conversion was successful, false otherwise.
inline bool
fill_asn1_handover_resource_allocation_response(asn1::ngap::ho_fail_s&                            asn1_ho_failure,
                                                const ngap_handover_resource_allocation_response& ho_response)
{
  if (!ho_response.success) {
    // cause
    asn1_ho_failure->cause = cause_to_asn1(ho_response.cause);

    // crit diagnostics
    if (ho_response.crit_diagnostics.has_value()) {
      // TODO: Add crit diagnostics
    }
  } else {
    return false;
  }

  return true;
}

/// \brief Fill the Handover Notify to ASN.1 struct.
/// \param[out] asn1_msg The Handover Notify ASN1 struct to fill.
/// \param[in] cgi The nr_cell_global_id common type struct of the UE.
/// \param[in] tac The tac of the UE.
inline void
fill_asn1_handover_notify(asn1::ngap::ho_notify_s& asn1_msg, const nr_cell_global_id_t& cgi, const unsigned tac)
{
  auto& user_loc_info_nr       = asn1_msg->user_location_info.set_user_location_info_nr();
  user_loc_info_nr.nr_cgi      = nr_cgi_to_ngap_asn1(cgi);
  user_loc_info_nr.tai.plmn_id = cgi.plmn_id.to_bytes();
  user_loc_info_nr.tai.tac.from_number(tac);
}

/// \brief Convert DL UE Associated NRPPa Transport ASN1 struct to common type.
/// \param[out] msg The ngap_ue_associated_nrppa_transport struct to fill.
/// \param[in] asn1_msg The DL UE Associated NRPPa Transport ASN1 struct.
inline void fill_dl_ue_associated_nrppa_transport(ngap_ue_associated_nrppa_transport&                   msg,
                                                  const asn1::ngap::dl_ue_associated_nrppa_transport_s& asn1_msg)
{
  msg.routing_id = asn1_msg->routing_id.copy();
  msg.nrppa_pdu  = asn1_msg->nrppa_pdu.copy();
}

/// \brief Convert common type UL UE Associated NRPPa Transport struct to ASN.1.
/// \param[out] asn1_msg The DL UE Associated NRPPa Transport ASN1 struct to fill.
/// \param[in] msg The common type ngap_ue_associated_nrppa_transport struct.
inline void fill_ul_ue_associated_nrppa_transport(asn1::ngap::ul_ue_associated_nrppa_transport_s& asn1_msg,
                                                  const ngap_ue_associated_nrppa_transport&       msg)
{
  asn1_msg->routing_id = msg.routing_id.copy();
  asn1_msg->nrppa_pdu  = msg.nrppa_pdu.copy();
}

/// \brief Convert DL Non UE Associated NRPPa Transport ASN1 struct to common type.
/// \param[out] msg The ngap_ue_associated_nrppa_transport struct to fill.
/// \param[in] asn1_msg The DL Non UE Associated NRPPa Transport ASN1 struct.
inline void
fill_dl_non_ue_associated_nrppa_transport(ngap_non_ue_associated_nrppa_transport&                   msg,
                                          const asn1::ngap::dl_non_ue_associated_nrppa_transport_s& asn1_msg)
{
  msg.routing_id = asn1_msg->routing_id.copy();
  msg.nrppa_pdu  = asn1_msg->nrppa_pdu.copy();
}

/// \brief Convert common type UL Non UE Associated NRPPa Transport struct to ASN.1.
/// \param[out] asn1_msg The DL Non UE Associated NRPPa Transport ASN1 struct to fill.
/// \param[in] msg The common type ngap_ue_associated_nrppa_transport struct.
inline void fill_ul_non_ue_associated_nrppa_transport(asn1::ngap::ul_non_ue_associated_nrppa_transport_s& asn1_msg,
                                                      const ngap_non_ue_associated_nrppa_transport&       msg)
{
  asn1_msg->routing_id = msg.routing_id.copy();
  asn1_msg->nrppa_pdu  = msg.nrppa_pdu.copy();
}

} // namespace srs_cu_cp
} // namespace srsran
