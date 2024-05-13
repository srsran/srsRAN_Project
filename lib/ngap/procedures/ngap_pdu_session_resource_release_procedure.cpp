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

#include "ngap_pdu_session_resource_release_procedure.h"
#include "../ngap/ngap_asn1_helpers.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/ngap/ngap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

ngap_pdu_session_resource_release_procedure::ngap_pdu_session_resource_release_procedure(
    const cu_cp_pdu_session_resource_release_command& command_,
    const ngap_ue_ids&                                ue_ids_,
    ngap_cu_cp_notifier&                              cu_cp_notifier_,
    ngap_message_notifier&                            amf_notif_,
    ngap_ue_logger&                                   logger_) :
  command(command_), ue_ids(ue_ids_), cu_cp_notifier(cu_cp_notifier_), amf_notifier(amf_notif_), logger(logger_)
{
}

void ngap_pdu_session_resource_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" initialized", name());

  // Handle mandatory IEs
  CORO_AWAIT_VALUE(response, cu_cp_notifier.on_new_pdu_session_resource_release_command(command));

  // TODO: Handle optional IEs

  send_pdu_session_resource_release_response();

  logger.log_debug("\"{}\" finalized", name());

  CORO_RETURN();
}

/// \brief Convert common type PDU Session Resource Release Response message to NGAP PDU Session Resource Release
/// Response.
/// \param[out] resp The ASN1 NGAP PDU Session Resource Release Response message.
/// \param[in] cu_cp_resp The CU-CP PDU Session Resource Release Response message.
/// \return True on success, otherwise false.
static bool
fill_asn1_pdu_session_resource_release_response(asn1::ngap::pdu_session_res_release_resp_s&        resp,
                                                const cu_cp_pdu_session_resource_release_response& cu_cp_resp)
{
  for (const auto& cu_cp_pdu_session_res_released_item : cu_cp_resp.released_pdu_sessions) {
    asn1::ngap::pdu_session_res_released_item_rel_res_s asn1_pdu_session_res_released_item;
    asn1_pdu_session_res_released_item.pdu_session_id =
        pdu_session_id_to_uint(cu_cp_pdu_session_res_released_item.pdu_session_id);

    asn1::ngap::pdu_session_res_release_resp_transfer_s res_release_resp_transfer;

    if (cu_cp_pdu_session_res_released_item.resp_transfer.secondary_rat_usage_info.has_value()) {
      res_release_resp_transfer.ext = true;

      asn1::protocol_ext_field_s<asn1::ngap::pdu_session_res_release_resp_transfer_ext_ies_o>
            res_release_resp_transfer_ext;
      auto& asn1_secondary_rat_usage_info = res_release_resp_transfer_ext.value().secondary_rat_usage_info();

      if (cu_cp_pdu_session_res_released_item.resp_transfer.secondary_rat_usage_info.value()
              .pdu_session_usage_report.has_value()) {
        asn1_secondary_rat_usage_info.pdu_session_usage_report_present = true;

        const auto& pdu_session_usage_report =
            cu_cp_pdu_session_res_released_item.resp_transfer.secondary_rat_usage_info.value()
                .pdu_session_usage_report.value();

        asn1::string_to_enum(asn1_secondary_rat_usage_info.pdu_session_usage_report.rat_type,
                             pdu_session_usage_report.rat_type);

        for (const auto& pdu_session_usage_timed_item : pdu_session_usage_report.pdu_session_timed_report_list) {
          asn1::ngap::volume_timed_report_item_s asn1_pdu_session_usage_timed_item;

          asn1_pdu_session_usage_timed_item.start_time_stamp.from_number(pdu_session_usage_timed_item.start_time_stamp);
          asn1_pdu_session_usage_timed_item.end_time_stamp.from_number(pdu_session_usage_timed_item.end_time_stamp);
          asn1_pdu_session_usage_timed_item.usage_count_ul = pdu_session_usage_timed_item.usage_count_ul;
          asn1_pdu_session_usage_timed_item.usage_count_dl = pdu_session_usage_timed_item.usage_count_dl;

          asn1_secondary_rat_usage_info.pdu_session_usage_report.pdu_session_timed_report_list.push_back(
              asn1_pdu_session_usage_timed_item);
        }
      }

      for (const auto& qos_flows_usage_report_item :
           cu_cp_pdu_session_res_released_item.resp_transfer.secondary_rat_usage_info.value()
               .qos_flows_usage_report_list) {
        asn1::ngap::qos_flows_usage_report_item_s asn1_qos_flows_usage_report_item;
        asn1_qos_flows_usage_report_item.qos_flow_id = qos_flow_id_to_uint(qos_flows_usage_report_item.qos_flow_id);

        asn1::string_to_enum(asn1_qos_flows_usage_report_item.rat_type, qos_flows_usage_report_item.rat_type);

        for (const auto& qos_flow_timed_report_item : qos_flows_usage_report_item.qos_flows_timed_report_list) {
          asn1::ngap::volume_timed_report_item_s asn1_qos_flow_timed_report_item;

          asn1_qos_flow_timed_report_item.start_time_stamp.from_number(qos_flow_timed_report_item.start_time_stamp);
          asn1_qos_flow_timed_report_item.end_time_stamp.from_number(qos_flow_timed_report_item.end_time_stamp);
          asn1_qos_flow_timed_report_item.usage_count_ul = qos_flow_timed_report_item.usage_count_ul;
          asn1_qos_flow_timed_report_item.usage_count_dl = qos_flow_timed_report_item.usage_count_dl;

          asn1_qos_flows_usage_report_item.qos_flows_timed_report_list.push_back(asn1_qos_flow_timed_report_item);
        }

        asn1_secondary_rat_usage_info.qos_flows_usage_report_list.push_back(asn1_qos_flows_usage_report_item);
      }

      res_release_resp_transfer.ie_exts.push_back(res_release_resp_transfer_ext);
    } else {
      res_release_resp_transfer.ext = false;
    }

    // Pack pdu_session_res_release_resp_transfer_s
    byte_buffer pdu = pack_into_pdu(res_release_resp_transfer);
    if (pdu.empty()) {
      return false;
    }
    asn1_pdu_session_res_released_item.pdu_session_res_release_resp_transfer = std::move(pdu);

    resp->pdu_session_res_released_list_rel_res.push_back(asn1_pdu_session_res_released_item);
  }

  if (cu_cp_resp.user_location_info.has_value()) {
    resp->user_location_info_present = true;
    resp->user_location_info.set_user_location_info_nr() =
        cu_cp_user_location_info_to_asn1(cu_cp_resp.user_location_info.value());
  }

  return true;
}

void ngap_pdu_session_resource_release_procedure::send_pdu_session_resource_release_response()
{
  ngap_message ngap_msg = {};
  ngap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_NGAP_ID_PDU_SESSION_RES_RELEASE);
  auto& pdu_session_res_release_resp = ngap_msg.pdu.successful_outcome().value.pdu_session_res_release_resp();

  pdu_session_res_release_resp->amf_ue_ngap_id = amf_ue_id_to_uint(ue_ids.amf_ue_id);
  pdu_session_res_release_resp->ran_ue_ngap_id = ran_ue_id_to_uint(ue_ids.ran_ue_id);

  // TODO: needs more handling in the coro above?
  if (not fill_asn1_pdu_session_resource_release_response(pdu_session_res_release_resp, response)) {
    logger.log_warning("Cannot fill ASN1 PDU Session Resource Release Response");
    return;
  }

  amf_notifier.on_new_message(ngap_msg);
}
