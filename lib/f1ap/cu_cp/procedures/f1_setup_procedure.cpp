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

#include "f1_setup_procedure.h"
#include "../../common/asn1_helpers.h"
#include "../f1ap_asn1_converters.h"
#include "srsran/adt/expected.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/f1ap/cu_cp/du_setup_notifier.h"
#include "srsran/f1ap/cu_cp/f1ap_du_context.h"
#include "srsran/ran/bcd_helper.h"
#include "srsran/ran/cause/f1ap_cause.h"

using namespace srsran;
using namespace srs_cu_cp;

static error_type<std::pair<asn1::f1ap::cause_c, std::string>>
validate_f1_setup_request(const asn1::f1ap::f1_setup_request_s& request)
{
  asn1::f1ap::cause_c cause;
  cause.set_protocol().value = asn1::f1ap::cause_protocol_opts::unspecified;
  if (not request->gnb_du_name_present) {
    return make_unexpected(std::make_pair(cause, "Missing gNB-DU name"));
  }
  if (not request->gnb_du_served_cells_list_present or request->gnb_du_served_cells_list.size() == 0) {
    return make_unexpected(std::make_pair(cause, "DU has no served cells"));
  }

  return {};
}

du_setup_request srsran::srs_cu_cp::create_du_setup_request(const asn1::f1ap::f1_setup_request_s& asn1_request)
{
  du_setup_request request;

  // GNB DU ID
  request.gnb_du_id = static_cast<gnb_du_id_t>(asn1_request->gnb_du_id);

  // GNB DU name
  if (asn1_request->gnb_du_name_present) {
    request.gnb_du_name = asn1_request->gnb_du_name.to_string();
  }

  // GNB DU served cells list
  if (asn1_request->gnb_du_served_cells_list_present) {
    for (const auto& asn1_served_cell_item : asn1_request->gnb_du_served_cells_list) {
      const auto& asn1_served_cell = asn1_served_cell_item.value().gnb_du_served_cells_item();

      cu_cp_du_served_cells_item served_cell;

      // served cell info
      served_cell.served_cell_info.nr_cgi = cgi_from_asn1(asn1_served_cell.served_cell_info.nr_cgi).value();
      served_cell.served_cell_info.nr_pci = asn1_served_cell.served_cell_info.nr_pci;
      if (asn1_served_cell.served_cell_info.five_gs_tac_present) {
        served_cell.served_cell_info.five_gs_tac = asn1_served_cell.served_cell_info.five_gs_tac.to_number();
      }
      if (asn1_served_cell.served_cell_info.cfg_eps_tac_present) {
        served_cell.served_cell_info.cfg_eps_tac = asn1_served_cell.served_cell_info.cfg_eps_tac.to_number();
      }
      for (const auto& asn1_plmn : asn1_served_cell.served_cell_info.served_plmns) {
        auto result = plmn_identity::from_bytes(asn1_plmn.plmn_id.to_bytes());
        // Note: If the ASN.1 PLMN ID is not valid, it is not considered in the response back to the DU.
        if (result.has_value()) {
          served_cell.served_cell_info.served_plmns.push_back(result.value());
        }
      }
      served_cell.served_cell_info.nr_mode_info =
          f1ap_asn1_to_nr_mode_info(asn1_served_cell.served_cell_info.nr_mode_info);
      served_cell.served_cell_info.meas_timing_cfg = asn1_served_cell.served_cell_info.meas_timing_cfg.copy();

      // GNB DU sys info
      if (asn1_served_cell.gnb_du_sys_info_present) {
        cu_cp_gnb_du_sys_info gnb_du_sys_info;

        // MIB msg
        gnb_du_sys_info.mib_msg = asn1_served_cell.gnb_du_sys_info.mib_msg.copy();

        // SIB1 msg
        gnb_du_sys_info.sib1_msg = asn1_served_cell.gnb_du_sys_info.sib1_msg.copy();

        served_cell.gnb_du_sys_info = gnb_du_sys_info;
      } else {
        srslog::fetch_basic_logger("CU-CP-F1").error("gNB DU system information must be present for NG-RAN.");
      }

      request.gnb_du_served_cells_list.push_back(served_cell);
    }
  }

  // GNB DU RRC version
  request.gnb_du_rrc_version = asn1_request->gnb_du_rrc_version.latest_rrc_version.to_number();

  return request;
}

/// \brief Create an ASN.1 F1 Setup Response.
/// \param[in] request ASN.1 F1 Setup Request
/// \param[in] cu_response CU response to the F1 Setup Request.
/// \return ASN.1 F1AP PDU with F1 Setup Response.
static f1ap_message create_f1_setup_response(const asn1::f1ap::f1_setup_request_s& request,
                                             const du_setup_result::accepted&      cu_response)
{
  f1ap_message f1ap_msg;

  f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  auto& resp = f1ap_msg.pdu.successful_outcome().value.f1_setup_resp();

  resp->transaction_id = request->transaction_id;

  // fill CU common info
  if (not cu_response.gnb_cu_name.empty()) {
    resp->gnb_cu_name_present = true;
    resp->gnb_cu_name.from_string(cu_response.gnb_cu_name);
  }
  resp->gnb_cu_rrc_version.latest_rrc_version.from_number(cu_response.gnb_cu_rrc_version);

  // activate all DU cells
  if (not cu_response.cells_to_be_activ_list.empty()) {
    resp->cells_to_be_activ_list_present = true;
    for (const auto& du_cell : cu_response.cells_to_be_activ_list) {
      asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_activ_list_item_ies_o> resp_cell;
      resp_cell->cells_to_be_activ_list_item().nr_cgi.plmn_id = du_cell.nr_cgi.plmn_id.to_bytes();
      resp_cell->cells_to_be_activ_list_item().nr_cgi.nr_cell_id.from_number(du_cell.nr_cgi.nci.value());

      if (du_cell.nr_pci.has_value()) {
        resp_cell->cells_to_be_activ_list_item().nr_pci_present = true;
        resp_cell->cells_to_be_activ_list_item().nr_pci         = du_cell.nr_pci.value();
      }

      resp->cells_to_be_activ_list.push_back(resp_cell);
    }
  }

  return f1ap_msg;
}

static f1ap_message create_f1_setup_reject(const asn1::f1ap::f1_setup_request_s& request,
                                           const asn1::f1ap::cause_c&            cause)
{
  f1ap_message f1ap_msg;

  f1ap_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  auto& fail = f1ap_msg.pdu.unsuccessful_outcome().value.f1_setup_fail();

  fail->transaction_id = request->transaction_id;
  fail->cause          = cause;

  return f1ap_msg;
}

void srsran::srs_cu_cp::handle_f1_setup_procedure(const asn1::f1ap::f1_setup_request_s& request,
                                                  f1ap_du_context&                      du_ctxt,
                                                  f1ap_message_notifier&                pdu_notifier,
                                                  du_setup_notifier&                    du_setup_notif,
                                                  srslog::basic_logger&                 logger)
{
  // Message content validation.
  auto msgerr = validate_f1_setup_request(request);
  if (not msgerr.has_value()) {
    logger.warning("Rejecting F1 Setup Request. Cause: {}", msgerr.error().second);
    pdu_notifier.on_new_message(create_f1_setup_reject(request, msgerr.error().first));
    return;
  }

  du_ctxt.gnb_du_id          = (gnb_du_id_t)request->gnb_du_id;
  du_ctxt.gnb_du_name        = request->gnb_du_name_present ? request->gnb_du_name.to_string() : "unnamed";
  du_ctxt.gnb_du_rrc_version = request->gnb_du_rrc_version.latest_rrc_version.to_number();

  // Request DU setup to CU-CP.
  du_setup_request du_req          = create_du_setup_request(request);
  du_setup_result  request_outcome = du_setup_notif.on_new_du_setup_request(du_req);

  // Handle Result.
  f1ap_message f1ap_msg;
  if (not request_outcome.is_accepted()) {
    // Failed to setup DU case.
    auto& fail_resp = std::get<du_setup_result::rejected>(request_outcome.result);
    logger.warning("Rejecting F1 Setup Request. Cause: {}", fail_resp.cause_str);
    f1ap_msg = create_f1_setup_reject(request, cause_to_asn1(fail_resp.cause));
  } else {
    // DU has been accepted.
    f1ap_msg = create_f1_setup_response(request, std::get<du_setup_result::accepted>(request_outcome.result));
  }

  // Send F1AP PDU to F1-C.
  pdu_notifier.on_new_message(f1ap_msg);
}
