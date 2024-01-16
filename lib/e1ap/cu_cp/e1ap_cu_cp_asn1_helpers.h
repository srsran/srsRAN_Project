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

#include "../common/e1ap_asn1_converters.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/ran/bcd_helpers.h"
#include "srsran/ran/qos_prio_level.h"

namespace srsran {
namespace srs_cu_cp {

inline void fill_asn1_qos_flow_info_item(asn1::e1ap::qos_flow_qos_param_item_s& asn1_qos_flow_info_item,
                                         const e1ap_qos_flow_qos_param_item&    qos_flow_info_item)
{
  asn1_qos_flow_info_item.qos_flow_id = qos_flow_id_to_uint(qos_flow_info_item.qos_flow_id);

  // qos flow level qos params
  auto& qos_flow_level_params = qos_flow_info_item.qos_flow_level_qos_params;

  // dynamic 5qi
  if (qos_flow_level_params.qos_characteristics.dyn_5qi.has_value()) {
    auto& dynamic_5qi = qos_flow_level_params.qos_characteristics.dyn_5qi.value();
    asn1_qos_flow_info_item.qos_flow_level_qos_params.qos_characteristics.set_dyn_5qi();
    auto& asn1_dynamic_5qi = asn1_qos_flow_info_item.qos_flow_level_qos_params.qos_characteristics.dyn_5qi();

    asn1_dynamic_5qi.qos_prio_level                 = qos_prio_level_to_uint(dynamic_5qi.qos_prio_level);
    asn1_dynamic_5qi.packet_delay_budget            = dynamic_5qi.packet_delay_budget;
    asn1_dynamic_5qi.packet_error_rate.per_scalar   = dynamic_5qi.packet_error_rate.per_scalar;
    asn1_dynamic_5qi.packet_error_rate.per_exponent = dynamic_5qi.packet_error_rate.per_exponent;
    if (dynamic_5qi.five_qi.has_value()) {
      asn1_dynamic_5qi.five_qi_present = true;
      asn1_dynamic_5qi.five_qi         = five_qi_to_uint(dynamic_5qi.five_qi.value());
    }
    if (dynamic_5qi.delay_crit.has_value()) {
      asn1_dynamic_5qi.delay_crit_present = true;
      asn1::string_to_enum(asn1_dynamic_5qi.delay_crit, dynamic_5qi.delay_crit.value());
    }
    if (dynamic_5qi.averaging_win.has_value()) {
      asn1_dynamic_5qi.averaging_win_present = true;
      asn1_dynamic_5qi.averaging_win         = dynamic_5qi.averaging_win.value();
    }
    if (dynamic_5qi.max_data_burst_volume.has_value()) {
      asn1_dynamic_5qi.max_data_burst_volume_present = true;
      asn1_dynamic_5qi.max_data_burst_volume         = dynamic_5qi.max_data_burst_volume.value();
    }
  } else if (qos_flow_level_params.qos_characteristics.non_dyn_5qi.has_value()) /* non dynamic 5qi */ {
    auto& non_dynamic_5qi = qos_flow_level_params.qos_characteristics.non_dyn_5qi.value();
    asn1_qos_flow_info_item.qos_flow_level_qos_params.qos_characteristics.set_non_dyn_5qi();
    auto& asn1_non_dynamic_5qi = asn1_qos_flow_info_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi();

    asn1_non_dynamic_5qi.five_qi = five_qi_to_uint(non_dynamic_5qi.five_qi);

    if (non_dynamic_5qi.qos_prio_level.has_value()) {
      asn1_non_dynamic_5qi.qos_prio_level_present = true;
      asn1_non_dynamic_5qi.qos_prio_level         = qos_prio_level_to_uint(non_dynamic_5qi.qos_prio_level.value());
    }
    if (non_dynamic_5qi.averaging_win.has_value()) {
      asn1_non_dynamic_5qi.averaging_win_present = true;
      asn1_non_dynamic_5qi.averaging_win         = non_dynamic_5qi.averaging_win.value();
    }
    if (non_dynamic_5qi.max_data_burst_volume.has_value()) {
      asn1_non_dynamic_5qi.max_data_burst_volume_present = true;
      asn1_non_dynamic_5qi.max_data_burst_volume         = non_dynamic_5qi.max_data_burst_volume.value();
    }
  } else {
    srsran_assertion_failure("Neither non-dynamic nor dynamic 5QI value is set.");
  }

  // ng ran alloc retention prio
  asn1_qos_flow_info_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.prio_level =
      qos_flow_level_params.ng_ran_alloc_retention_prio.prio_level;
  asn1::string_to_enum(asn1_qos_flow_info_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.pre_emption_cap,
                       qos_flow_level_params.ng_ran_alloc_retention_prio.pre_emption_cap);
  asn1::string_to_enum(
      asn1_qos_flow_info_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.pre_emption_vulnerability,
      qos_flow_level_params.ng_ran_alloc_retention_prio.pre_emption_vulnerability);

  // gbr qos flow info
  if (qos_flow_level_params.gbr_qos_flow_info.has_value()) {
    asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info_present = true;
    asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_flow_bit_rate_dl =
        qos_flow_level_params.gbr_qos_flow_info.value().max_flow_bit_rate_dl;
    asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_flow_bit_rate_ul =
        qos_flow_level_params.gbr_qos_flow_info.value().max_flow_bit_rate_ul;
    asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.guaranteed_flow_bit_rate_dl =
        qos_flow_level_params.gbr_qos_flow_info.value().guaranteed_flow_bit_rate_dl;
    asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.guaranteed_flow_bit_rate_ul =
        qos_flow_level_params.gbr_qos_flow_info.value().guaranteed_flow_bit_rate_ul;
    if (qos_flow_level_params.gbr_qos_flow_info.value().max_packet_loss_rate_dl.has_value()) {
      asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_dl_present = true;
      asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_dl =
          qos_flow_level_params.gbr_qos_flow_info.value().max_packet_loss_rate_dl.value();
    }
    if (qos_flow_level_params.gbr_qos_flow_info.value().max_packet_loss_rate_ul.has_value()) {
      asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_ul_present = true;
      asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_ul =
          qos_flow_level_params.gbr_qos_flow_info.value().max_packet_loss_rate_ul.value();
    }
  }

  // reflective qos attribute
  if (qos_flow_level_params.reflective_qos_attribute.has_value()) {
    asn1_qos_flow_info_item.qos_flow_level_qos_params.reflective_qos_attribute_present = true;
    asn1::bool_to_enum(asn1_qos_flow_info_item.qos_flow_level_qos_params.reflective_qos_attribute,
                       qos_flow_level_params.reflective_qos_attribute.value());
  }

  // add qos info
  if (qos_flow_level_params.add_qos_info.has_value()) {
    asn1_qos_flow_info_item.qos_flow_level_qos_params.add_qos_info_present = true;
    asn1::bool_to_enum(asn1_qos_flow_info_item.qos_flow_level_qos_params.add_qos_info,
                       qos_flow_level_params.add_qos_info.value());
  }

  // paging policy ind
  if (qos_flow_level_params.paging_policy_ind.has_value()) {
    asn1_qos_flow_info_item.qos_flow_level_qos_params.paging_policy_ind_present = true;
    asn1_qos_flow_info_item.qos_flow_level_qos_params.paging_policy_ind =
        qos_flow_level_params.paging_policy_ind.value();
  }

  // reflective qos ind
  if (qos_flow_level_params.reflective_qos_ind.has_value()) {
    asn1_qos_flow_info_item.qos_flow_level_qos_params.reflective_qos_ind_present = true;
    asn1::bool_to_enum(asn1_qos_flow_info_item.qos_flow_level_qos_params.reflective_qos_ind,
                       qos_flow_level_params.reflective_qos_ind.value());
  }

  // qos flow map ind
  if (qos_flow_info_item.qos_flow_map_ind.has_value()) {
    asn1_qos_flow_info_item.qos_flow_map_ind_present = true;
    asn1::string_to_enum(asn1_qos_flow_info_item.qos_flow_map_ind, qos_flow_info_item.qos_flow_map_ind.value());
  }
}

inline void fill_asn1_drb_to_setup_item(asn1::e1ap::drb_to_setup_item_ng_ran_s& asn1_drb_to_setup_item,
                                        const e1ap_drb_to_setup_item_ng_ran&    drb_to_setup_item)
{
  asn1_drb_to_setup_item.drb_id = drb_id_to_uint(drb_to_setup_item.drb_id);

  // sdap config
  asn1_drb_to_setup_item.sdap_cfg = sdap_config_to_e1ap_asn1(drb_to_setup_item.sdap_cfg);

  // pdcp config
  asn1_drb_to_setup_item.pdcp_cfg = pdcp_config_to_e1ap_asn1(drb_to_setup_item.pdcp_cfg);

  // cell group info
  for (const auto& cell_group_info_item : drb_to_setup_item.cell_group_info) {
    asn1::e1ap::cell_group_info_item_s asn1_cell_group_info_item;

    asn1_cell_group_info_item.cell_group_id = cell_group_info_item.cell_group_id;

    if (cell_group_info_item.ul_cfg.has_value()) {
      asn1_cell_group_info_item.ul_cfg_present = true;
      asn1::string_to_enum(asn1_cell_group_info_item.ul_cfg, cell_group_info_item.ul_cfg.value());
    }

    if (cell_group_info_item.dl_tx_stop.has_value()) {
      asn1_cell_group_info_item.dl_tx_stop_present = true;
      asn1::string_to_enum(asn1_cell_group_info_item.dl_tx_stop, cell_group_info_item.dl_tx_stop.value());
    }

    if (cell_group_info_item.rat_type.has_value()) {
      asn1_cell_group_info_item.rat_type_present = true;
      asn1::string_to_enum(asn1_cell_group_info_item.rat_type, cell_group_info_item.rat_type.value());
    }

    asn1_drb_to_setup_item.cell_group_info.push_back(asn1_cell_group_info_item);
  }

  // qos flow info to be setup
  for (const auto& qos_flow_info_item : drb_to_setup_item.qos_flow_info_to_be_setup) {
    asn1::e1ap::qos_flow_qos_param_item_s asn1_qos_flow_info_item;
    fill_asn1_qos_flow_info_item(asn1_qos_flow_info_item, qos_flow_info_item);
    asn1_drb_to_setup_item.qos_flow_info_to_be_setup.push_back(asn1_qos_flow_info_item);
  }

  // drb data forwarding info request
  if (drb_to_setup_item.drb_data_forwarding_info_request.has_value()) {
    asn1_drb_to_setup_item.drb_data_forwarding_info_request_present = true;
    asn1::string_to_enum(asn1_drb_to_setup_item.drb_data_forwarding_info_request.data_forwarding_request,
                         drb_to_setup_item.drb_data_forwarding_info_request.value().data_forwarding_request);
    for (const auto& qos_flow_map_item :
         drb_to_setup_item.drb_data_forwarding_info_request.value().qos_flows_forwarded_on_fwd_tunnels) {
      asn1::e1ap::qos_flow_map_item_s asn1_qos_flow_map_item;
      asn1_qos_flow_map_item.qos_flow_id = qos_flow_id_to_uint(qos_flow_map_item.qos_flow_id);
      if (qos_flow_map_item.qos_flow_map_ind.has_value()) {
        asn1_qos_flow_map_item.qos_flow_map_ind_present = true;
        asn1::string_to_enum(asn1_qos_flow_map_item.qos_flow_map_ind, qos_flow_map_item.qos_flow_map_ind.value());
      }

      asn1_drb_to_setup_item.drb_data_forwarding_info_request.qos_flows_forwarded_on_fwd_tunnels.push_back(
          asn1_qos_flow_map_item);
    }
  }

  // drb inactivity timer
  if (drb_to_setup_item.drb_inactivity_timer.has_value()) {
    asn1_drb_to_setup_item.drb_inactivity_timer_present = true;
    asn1_drb_to_setup_item.drb_inactivity_timer         = drb_to_setup_item.drb_inactivity_timer.value().count();
  }

  // pdcp sn status info
  if (drb_to_setup_item.pdcp_sn_status_info.has_value()) {
    asn1_drb_to_setup_item.pdcp_sn_status_info_present = true;

    asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_ul.count_value.pdcp_sn =
        drb_to_setup_item.pdcp_sn_status_info.value().pdcp_status_transfer_ul.count_value.pdcp_sn;
    asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_ul.count_value.hfn =
        drb_to_setup_item.pdcp_sn_status_info.value().pdcp_status_transfer_ul.count_value.hfn;
    if (drb_to_setup_item.pdcp_sn_status_info.value().pdcp_status_transfer_ul.receive_status_of_pdcp_sdu.has_value()) {
      asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_ul.receive_statusof_pdcp_sdu_present = true;
      asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_ul.receive_statusof_pdcp_sdu.from_number(
          drb_to_setup_item.pdcp_sn_status_info.value().pdcp_status_transfer_ul.receive_status_of_pdcp_sdu.value());
    }

    asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_dl.pdcp_sn =
        drb_to_setup_item.pdcp_sn_status_info.value().pdcp_status_transfer_dl.pdcp_sn;
    asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_dl.hfn =
        drb_to_setup_item.pdcp_sn_status_info.value().pdcp_status_transfer_dl.hfn;
  }
}

inline void fill_asn1_bearer_context_setup_request(asn1::e1ap::bearer_context_setup_request_s& asn1_request,
                                                   const e1ap_bearer_context_setup_request&    request)
{
  // security info
  asn1_request->security_info.security_algorithm.ciphering_algorithm =
      ciphering_algorithm_to_e1ap_asn1(request.security_info.security_algorithm.ciphering_algo);
  if (request.security_info.security_algorithm.integrity_protection_algorithm.has_value()) {
    asn1_request->security_info.security_algorithm.integrity_protection_algorithm_present = true;
    asn1_request->security_info.security_algorithm.integrity_protection_algorithm = integrity_algorithm_to_e1ap_asn1(
        request.security_info.security_algorithm.integrity_protection_algorithm.value());
  }
  asn1_request->security_info.up_securitykey.encryption_key =
      request.security_info.up_security_key.encryption_key.copy();
  asn1_request->security_info.up_securitykey.integrity_protection_key =
      request.security_info.up_security_key.integrity_protection_key.copy();

  // ue dl aggregate maximum bit rate
  asn1_request->ue_dl_aggr_max_bit_rate = request.ue_dl_aggregate_maximum_bit_rate;

  // serving plmn
  asn1_request->serving_plmn.from_number(plmn_string_to_bcd(request.serving_plmn));

  // activity notification level
  asn1::string_to_enum(asn1_request->activity_notif_level, request.activity_notif_level);

  // pdu session resource to setup list
  asn1_request->sys_bearer_context_setup_request.set_ng_ran_bearer_context_setup_request();
  auto& ng_ran_bearer_context_setup_request =
      asn1_request->sys_bearer_context_setup_request.ng_ran_bearer_context_setup_request();

  for (const auto& pdu_session_res_item : request.pdu_session_res_to_setup_list) {
    asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_setup_request_o> bearer_request_item;
    auto& asn1_pdu_session_res_items = bearer_request_item.value().pdu_session_res_to_setup_list();

    asn1::e1ap::pdu_session_res_to_setup_item_s asn1_pdu_session_res_item;

    // pdu session id
    asn1_pdu_session_res_item.pdu_session_id = pdu_session_id_to_uint(pdu_session_res_item.pdu_session_id);

    // pdu session type
    asn1::string_to_enum(asn1_pdu_session_res_item.pdu_session_type, pdu_session_res_item.pdu_session_type);

    // s-nssai
    asn1_pdu_session_res_item.snssai = snssai_to_e1ap_asn1(pdu_session_res_item.snssai);

    // ng ul up transport layer information
    up_transport_layer_info_to_asn1(asn1_pdu_session_res_item.ng_ul_up_tnl_info,
                                    pdu_session_res_item.ng_ul_up_tnl_info);

    // security indication
    security_indication_to_asn1(asn1_pdu_session_res_item.security_ind, pdu_session_res_item.security_ind);

    // drb to setup list ng ran
    for (const auto& drb_to_setup_item : pdu_session_res_item.drb_to_setup_list_ng_ran) {
      asn1::e1ap::drb_to_setup_item_ng_ran_s asn1_drb_to_setup_item;
      fill_asn1_drb_to_setup_item(asn1_drb_to_setup_item, drb_to_setup_item);
      asn1_pdu_session_res_item.drb_to_setup_list_ng_ran.push_back(asn1_drb_to_setup_item);
    }

    // pdu session resource dl aggregate maximum bit rate
    if (pdu_session_res_item.pdu_session_res_dl_ambr.has_value()) {
      asn1_pdu_session_res_item.pdu_session_res_dl_ambr_present = true;
      asn1_pdu_session_res_item.pdu_session_res_dl_ambr         = pdu_session_res_item.pdu_session_res_dl_ambr.value();
    }

    // pdu session data forwarding info request
    if (pdu_session_res_item.pdu_session_data_forwarding_info_request.has_value()) {
      asn1_pdu_session_res_item.pdu_session_data_forwarding_info_request_present = true;

      asn1::string_to_enum(
          asn1_pdu_session_res_item.pdu_session_data_forwarding_info_request.data_forwarding_request,
          pdu_session_res_item.pdu_session_data_forwarding_info_request.value().data_forwarding_request);
      for (const auto& qos_flow_map_item :
           pdu_session_res_item.pdu_session_data_forwarding_info_request.value().qos_flows_forwarded_on_fwd_tunnels) {
        asn1::e1ap::qos_flow_map_item_s asn1_qos_flow_map_item;
        asn1_qos_flow_map_item.qos_flow_id = qos_flow_id_to_uint(qos_flow_map_item.qos_flow_id);
        if (qos_flow_map_item.qos_flow_map_ind.has_value()) {
          asn1_qos_flow_map_item.qos_flow_map_ind_present = true;
          asn1::string_to_enum(asn1_qos_flow_map_item.qos_flow_map_ind, qos_flow_map_item.qos_flow_map_ind.value());
        }

        asn1_pdu_session_res_item.pdu_session_data_forwarding_info_request.qos_flows_forwarded_on_fwd_tunnels.push_back(
            asn1_qos_flow_map_item);
      }
    }

    // pdu session inactivity timer
    if (pdu_session_res_item.pdu_session_inactivity_timer.has_value()) {
      asn1_pdu_session_res_item.pdu_session_inactivity_timer_present = true;
      asn1_pdu_session_res_item.pdu_session_inactivity_timer =
          pdu_session_res_item.pdu_session_inactivity_timer.value().count();
    }

    // existing allocated ng dl up tnl info
    if (pdu_session_res_item.existing_allocated_ng_dl_up_tnl_info.has_value()) {
      asn1_pdu_session_res_item.existing_allocated_ng_dl_up_tnl_info_present = true;
      up_transport_layer_info_to_asn1(asn1_pdu_session_res_item.existing_allocated_ng_dl_up_tnl_info,
                                      pdu_session_res_item.existing_allocated_ng_dl_up_tnl_info.value());
    }

    // network instance
    if (pdu_session_res_item.network_instance.has_value()) {
      asn1_pdu_session_res_item.network_instance_present = true;
      asn1_pdu_session_res_item.network_instance         = pdu_session_res_item.network_instance.value();
    }

    asn1_pdu_session_res_items.push_back(asn1_pdu_session_res_item);

    ng_ran_bearer_context_setup_request.push_back(bearer_request_item);
  }

  // ue dl maximum integrity protection data rate
  if (request.ue_dl_maximum_integrity_protected_data_rate.has_value()) {
    asn1_request->ue_dl_max_integrity_protected_data_rate_present = true;
    asn1_request->ue_dl_max_integrity_protected_data_rate = request.ue_dl_maximum_integrity_protected_data_rate.value();
  }

  // ue inactivity timer
  if (request.ue_inactivity_timer.has_value()) {
    asn1_request->ue_inactivity_timer_present = true;
    asn1_request->ue_inactivity_timer         = request.ue_inactivity_timer.value().count();
  }

  // bearer context status change
  if (request.bearer_context_status_change.has_value()) {
    asn1_request->bearer_context_status_change_present = true;
    asn1::string_to_enum(asn1_request->bearer_context_status_change, request.bearer_context_status_change.value());
  }

  // ran ue id
  if (request.ran_ue_id.has_value()) {
    asn1_request->ran_ue_id_present = true;
    asn1_request->ran_ue_id.from_number(ran_ue_id_to_uint(request.ran_ue_id.value()));
  }

  // gnb du id
  if (request.gnb_du_id.has_value()) {
    asn1_request->gnb_du_id_present = true;
    asn1_request->gnb_du_id         = request.gnb_du_id.value();
  }
}

inline void
fill_e1ap_bearer_context_setup_response(e1ap_bearer_context_setup_response&            res,
                                        const asn1::e1ap::bearer_context_setup_resp_s& asn1_bearer_context_setup_resp)
{
  // Fail if E-UTRAN bearer context setup is returned
  if (asn1_bearer_context_setup_resp->sys_bearer_context_setup_resp.type() ==
      asn1::e1ap::sys_bearer_context_setup_resp_c::types::e_utran_bearer_context_setup_resp) {
    res.success = false;
    res.cause   = cause_protocol_t::unspecified;
    return;
  } else {
    res.success = true;

    auto& asn1_bearer_context_setup_response =
        asn1_bearer_context_setup_resp->sys_bearer_context_setup_resp.ng_ran_bearer_context_setup_resp();

    for (const auto& asn1_res_setup_item : asn1_bearer_context_setup_response.pdu_session_res_setup_list) {
      e1ap_pdu_session_resource_setup_modification_item res_setup_item;
      res_setup_item.pdu_session_id = uint_to_pdu_session_id(asn1_res_setup_item.pdu_session_id);

      // Add NG DL UP TNL Info
      res_setup_item.ng_dl_up_tnl_info = asn1_to_up_transport_layer_info(asn1_res_setup_item.ng_dl_up_tnl_info);

      // Add DRB Setup List NG RAN
      for (const auto& asn1_drb_setup_item : asn1_res_setup_item.drb_setup_list_ng_ran) {
        e1ap_drb_setup_item_ng_ran drb_setup_item;
        drb_setup_item.drb_id = uint_to_drb_id(asn1_drb_setup_item.drb_id);

        // Add UL UP Transport Params
        for (const auto& asn1_ul_up_transport_param : asn1_drb_setup_item.ul_up_transport_params) {
          e1ap_up_params_item ue_params_item;
          ue_params_item.up_tnl_info   = asn1_to_up_transport_layer_info(asn1_ul_up_transport_param.up_tnl_info);
          ue_params_item.cell_group_id = asn1_ul_up_transport_param.cell_group_id;

          drb_setup_item.ul_up_transport_params.push_back(ue_params_item);
        }

        // Add Flow setup List
        for (auto& asn1_qos_flow_item : asn1_drb_setup_item.flow_setup_list) {
          e1ap_qos_flow_item qos_flow_item;
          qos_flow_item.qos_flow_id = uint_to_qos_flow_id(asn1_qos_flow_item.qos_flow_id);

          drb_setup_item.flow_setup_list.emplace(qos_flow_item.qos_flow_id, qos_flow_item);
        }

        // Add Flow Failed List
        for (const auto& asn1_failed_qos_flow_item : asn1_drb_setup_item.flow_failed_list) {
          e1ap_qos_flow_failed_item failed_qos_flow_item;

          failed_qos_flow_item.qos_flow_id = uint_to_qos_flow_id(asn1_failed_qos_flow_item.qos_flow_id);
          failed_qos_flow_item.cause       = e1ap_cause_to_cause(asn1_failed_qos_flow_item.cause);

          drb_setup_item.flow_failed_list.emplace(failed_qos_flow_item.qos_flow_id, failed_qos_flow_item);
        }

        // Add DRB Data Forwarding Info Response
        if (asn1_drb_setup_item.drb_data_forwarding_info_resp_present) {
          if (asn1_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding_present) {
            drb_setup_item.drb_data_forwarding_info_resp.value().ul_data_forwarding =
                asn1_to_up_transport_layer_info(asn1_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding);
          }
          if (asn1_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding_present) {
            drb_setup_item.drb_data_forwarding_info_resp.value().dl_data_forwarding =
                asn1_to_up_transport_layer_info(asn1_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding);
          }
        }

        res_setup_item.drb_setup_list_ng_ran.emplace(drb_setup_item.drb_id, drb_setup_item);
      }

      // Add DRB Failed List NG RAN
      for (const auto& asn1_drb_failed_item : asn1_res_setup_item.drb_failed_list_ng_ran) {
        e1ap_drb_failed_item_ng_ran drb_failed_item;
        drb_failed_item.drb_id = uint_to_drb_id(asn1_drb_failed_item.drb_id);
        drb_failed_item.cause  = e1ap_cause_to_cause(asn1_drb_failed_item.cause);

        res_setup_item.drb_failed_list_ng_ran.emplace(drb_failed_item.drb_id, drb_failed_item);
      }

      // Add Security Result
      if (asn1_res_setup_item.security_result_present) {
        res_setup_item.security_result = security_result_t{};
        asn1_to_security_result(res_setup_item.security_result.value(), asn1_res_setup_item.security_result);
      }

      // Add PDU Session Data Forwarding Info Response
      if (asn1_res_setup_item.pdu_session_data_forwarding_info_resp_present) {
        if (asn1_res_setup_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding_present) {
          res_setup_item.pdu_session_data_forwarding_info_resp.value().ul_data_forwarding =
              asn1_to_up_transport_layer_info(
                  asn1_res_setup_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding);
        }
        if (asn1_res_setup_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding_present) {
          res_setup_item.pdu_session_data_forwarding_info_resp.value().dl_data_forwarding =
              asn1_to_up_transport_layer_info(
                  asn1_res_setup_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding);
        }
      }

      // Add NG DL UP Unchanged
      if (asn1_res_setup_item.ng_dl_up_unchanged_present) {
        res_setup_item.ng_dl_up_unchanged.value() =
            e1ap_asn1_ng_dl_up_unchanged_to_bool(asn1_res_setup_item.ng_dl_up_unchanged);
      }

      res.pdu_session_resource_setup_list.emplace(res_setup_item.pdu_session_id, res_setup_item);
    }

    // Add pdu session res failed list
    if (asn1_bearer_context_setup_response.pdu_session_res_failed_list_present) {
      for (const auto& asn1_failed_item : asn1_bearer_context_setup_response.pdu_session_res_failed_list) {
        e1ap_pdu_session_resource_failed_item failed_item;

        failed_item.pdu_session_id = uint_to_pdu_session_id(asn1_failed_item.pdu_session_id);
        failed_item.cause          = e1ap_cause_to_cause(asn1_failed_item.cause);

        res.pdu_session_resource_failed_list.emplace(failed_item.pdu_session_id, failed_item);
      }
    }
  }
}

inline void
fill_e1ap_bearer_context_setup_response(e1ap_bearer_context_setup_response&            res,
                                        const asn1::e1ap::bearer_context_setup_fail_s& e1ap_bearer_context_setup_fail)
{
  res.success = false;
  res.cause   = e1ap_cause_to_cause(e1ap_bearer_context_setup_fail->cause);
  if (e1ap_bearer_context_setup_fail->crit_diagnostics_present) {
    // TODO: Add crit diagnostics
  }
}

inline void fill_asn1_bearer_context_modification_request(asn1::e1ap::bearer_context_mod_request_s&       asn1_request,
                                                          const e1ap_bearer_context_modification_request& request)
{
  if (request.new_ul_tnl_info_required.has_value()) {
    asn1_request->new_ul_tnl_info_required_present = true;
    asn1::bool_to_enum(asn1_request->new_ul_tnl_info_required, request.new_ul_tnl_info_required.value());
  }

  // ng ran bearer context mod
  if (request.ng_ran_bearer_context_mod_request.has_value()) {
    asn1_request->sys_bearer_context_mod_request_present = true;
    asn1_request->sys_bearer_context_mod_request.set_ng_ran_bearer_context_mod_request();
    auto& asn1_bearer_context_mod = asn1_request->sys_bearer_context_mod_request.ng_ran_bearer_context_mod_request();

    if (!request.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list.empty()) {
      asn1_bearer_context_mod.pdu_session_res_to_modify_list_present = true;
      for (const auto& res_to_mod_item :
           request.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list) {
        asn1::e1ap::pdu_session_res_to_modify_item_s asn1_res_to_mod_item;
        asn1_res_to_mod_item.pdu_session_id = pdu_session_id_to_uint(res_to_mod_item.pdu_session_id);

        for (const auto& drb_to_setup_item : res_to_mod_item.drb_to_setup_list_ng_ran) {
          asn1::e1ap::drb_to_setup_item_ng_ran_s asn1_drb_to_setup_item;
          fill_asn1_drb_to_setup_item(asn1_drb_to_setup_item, drb_to_setup_item);
          asn1_res_to_mod_item.drb_to_setup_list_ng_ran.push_back(asn1_drb_to_setup_item);
        }

        for (const auto& drb_to_mod_item : res_to_mod_item.drb_to_modify_list_ng_ran) {
          asn1::e1ap::drb_to_modify_item_ng_ran_s asn1_drb_to_mod_item;

          asn1_drb_to_mod_item.drb_id = drb_id_to_uint(drb_to_mod_item.drb_id);

          for (const auto& dl_up_param : drb_to_mod_item.dl_up_params) {
            asn1::e1ap::up_params_item_s asn1_dl_up_param;
            up_transport_layer_info_to_asn1(asn1_dl_up_param.up_tnl_info, dl_up_param.up_tnl_info);
            asn1_dl_up_param.cell_group_id = dl_up_param.cell_group_id;
            asn1_drb_to_mod_item.dl_up_params.push_back(asn1_dl_up_param);
          }

          if (drb_to_mod_item.pdcp_cfg.has_value()) {
            asn1_drb_to_mod_item.pdcp_cfg_present = true;
            asn1_drb_to_mod_item.pdcp_cfg         = pdcp_config_to_e1ap_asn1(drb_to_mod_item.pdcp_cfg.value());
          }

          asn1_res_to_mod_item.drb_to_modify_list_ng_ran.push_back(asn1_drb_to_mod_item);
        }

        for (const auto& drb_to_rem_item : res_to_mod_item.drb_to_rem_list_ng_ran) {
          asn1::e1ap::drb_to_rem_item_ng_ran_s asn1_drb_to_rem_item;
          asn1_drb_to_rem_item.drb_id = drb_id_to_uint(drb_to_rem_item);
          asn1_res_to_mod_item.drb_to_rem_list_ng_ran.push_back(asn1_drb_to_rem_item);
        }

        asn1_bearer_context_mod.pdu_session_res_to_modify_list.push_back(asn1_res_to_mod_item);
      }
    }

    if (!request.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_setup_mod_list.empty()) {
      asn1_bearer_context_mod.pdu_session_res_to_setup_mod_list_present = true;

      for (const auto& res_to_setup_mod_item :
           request.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_setup_mod_list) {
        asn1::e1ap::pdu_session_res_to_setup_mod_item_s asn1_res_to_setup_mod_item;
        asn1_res_to_setup_mod_item.pdu_session_id = pdu_session_id_to_uint(res_to_setup_mod_item.pdu_session_id);

        // pdu session type
        asn1::string_to_enum(asn1_res_to_setup_mod_item.pdu_session_type, res_to_setup_mod_item.pdu_session_type);

        // s-nssai
        asn1_res_to_setup_mod_item.snssai = snssai_to_e1ap_asn1(res_to_setup_mod_item.snssai);

        // ng ul up transport layer information
        up_transport_layer_info_to_asn1(asn1_res_to_setup_mod_item.ng_ul_up_tnl_info,
                                        res_to_setup_mod_item.ng_ul_up_tnl_info);

        // security indication
        security_indication_to_asn1(asn1_res_to_setup_mod_item.security_ind, res_to_setup_mod_item.security_ind);

        for (const auto& drb_to_setup_mod_item : res_to_setup_mod_item.drb_to_setup_list_ng_ran) {
          asn1::e1ap::drb_to_setup_mod_item_ng_ran_s asn1_drb_to_setup_mod_item;
          asn1_drb_to_setup_mod_item.drb_id = drb_id_to_uint(drb_to_setup_mod_item.drb_id);

          // sdap config
          asn1_drb_to_setup_mod_item.sdap_cfg = sdap_config_to_e1ap_asn1(drb_to_setup_mod_item.sdap_cfg);

          // pdcp config
          asn1_drb_to_setup_mod_item.pdcp_cfg = pdcp_config_to_e1ap_asn1(drb_to_setup_mod_item.pdcp_cfg);

          for (const auto& cell_group_info_item : drb_to_setup_mod_item.cell_group_info) {
            asn1::e1ap::cell_group_info_item_s asn1_cell_group_info_item;

            asn1_cell_group_info_item.cell_group_id = cell_group_info_item.cell_group_id;

            if (cell_group_info_item.ul_cfg.has_value()) {
              asn1_cell_group_info_item.ul_cfg_present = true;
              asn1::string_to_enum(asn1_cell_group_info_item.ul_cfg, cell_group_info_item.ul_cfg.value());
            }

            if (cell_group_info_item.dl_tx_stop.has_value()) {
              asn1_cell_group_info_item.dl_tx_stop_present = true;
              asn1::string_to_enum(asn1_cell_group_info_item.dl_tx_stop, cell_group_info_item.dl_tx_stop.value());
            }

            if (cell_group_info_item.rat_type.has_value()) {
              asn1_cell_group_info_item.rat_type_present = true;
              asn1::string_to_enum(asn1_cell_group_info_item.rat_type, cell_group_info_item.rat_type.value());
            }

            asn1_drb_to_setup_mod_item.cell_group_info.push_back(asn1_cell_group_info_item);
          }

          // qos flow_map_info
          for (const auto& qos_flow_info_item : drb_to_setup_mod_item.qos_flow_info_to_be_setup) {
            asn1::e1ap::qos_flow_qos_param_item_s asn1_qos_flow_info_item;
            fill_asn1_qos_flow_info_item(asn1_qos_flow_info_item, qos_flow_info_item);
            asn1_drb_to_setup_mod_item.flow_map_info.push_back(asn1_qos_flow_info_item);
          }

          // TODO: DL/UL tunnel info missing?
          asn1_res_to_setup_mod_item.drb_to_setup_mod_list_ng_ran.push_back(asn1_drb_to_setup_mod_item);
        }

        asn1_bearer_context_mod.pdu_session_res_to_setup_mod_list.push_back(asn1_res_to_setup_mod_item);
      }
    }
    if (!request.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_rem_list.empty()) {
      asn1_bearer_context_mod.pdu_session_res_to_rem_list_present = true;
      for (const auto& psi_to_rem : request.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_rem_list) {
        asn1::e1ap::pdu_session_res_to_rem_item_s asn1_res_to_rem_item;
        asn1_res_to_rem_item.pdu_session_id = pdu_session_id_to_uint(psi_to_rem);
        asn1_bearer_context_mod.pdu_session_res_to_rem_list.push_back(asn1_res_to_rem_item);
      }
    }
  }
}

inline void fill_e1ap_bearer_context_modification_response(
    e1ap_bearer_context_modification_response&   res,
    const asn1::e1ap::bearer_context_mod_resp_s& asn1_bearer_context_modification_resp)
{
  res.success = true;
  if (asn1_bearer_context_modification_resp->sys_bearer_context_mod_resp_present) {
    // Fail if E-UTRAN bearer context setup is returned
    if (asn1_bearer_context_modification_resp->sys_bearer_context_mod_resp.type() ==
        asn1::e1ap::sys_bearer_context_mod_resp_c::types::e_utran_bearer_context_mod_resp) {
      res.success = false;
      res.cause   = cause_protocol_t::unspecified;
      return;
    } else {
      // Add NG RAN bearer context modification response
      auto& asn1_bearer_context_mod_response =
          asn1_bearer_context_modification_resp->sys_bearer_context_mod_resp.ng_ran_bearer_context_mod_resp();

      // Add PDU session resource setup list
      if (asn1_bearer_context_mod_response.pdu_session_res_setup_mod_list_present) {
        for (const auto& asn1_res_mod_item : asn1_bearer_context_mod_response.pdu_session_res_setup_mod_list) {
          e1ap_pdu_session_resource_setup_modification_item res_mod_item;
          res_mod_item.pdu_session_id = uint_to_pdu_session_id(asn1_res_mod_item.pdu_session_id);

          // Add NG DL UP TNL Info
          res_mod_item.ng_dl_up_tnl_info = asn1_to_up_transport_layer_info(asn1_res_mod_item.ng_dl_up_tnl_info);

          // Add DRB Setup List NG RAN
          for (const auto& asn1_drb_setup_item : asn1_res_mod_item.drb_setup_mod_list_ng_ran) {
            e1ap_drb_setup_item_ng_ran drb_setup_item;
            drb_setup_item.drb_id = uint_to_drb_id(asn1_drb_setup_item.drb_id);

            // Add UL UP Transport Params
            for (const auto& asn1_ul_up_transport_param : asn1_drb_setup_item.ul_up_transport_params) {
              e1ap_up_params_item ue_params_item;
              ue_params_item.up_tnl_info   = asn1_to_up_transport_layer_info(asn1_ul_up_transport_param.up_tnl_info);
              ue_params_item.cell_group_id = asn1_ul_up_transport_param.cell_group_id;

              drb_setup_item.ul_up_transport_params.push_back(ue_params_item);
            }

            // Add Flow setup List
            for (const auto& asn1_qos_flow_item : asn1_drb_setup_item.flow_setup_list) {
              e1ap_qos_flow_item qos_flow_item;
              qos_flow_item.qos_flow_id = uint_to_qos_flow_id(asn1_qos_flow_item.qos_flow_id);

              drb_setup_item.flow_setup_list.emplace(qos_flow_item.qos_flow_id, qos_flow_item);
            }

            // Add Flow Failed List
            for (const auto& asn1_failed_qos_flow_item : asn1_drb_setup_item.flow_failed_list) {
              e1ap_qos_flow_failed_item failed_qos_flow_item;

              failed_qos_flow_item.qos_flow_id = uint_to_qos_flow_id(asn1_failed_qos_flow_item.qos_flow_id);
              failed_qos_flow_item.cause       = e1ap_cause_to_cause(asn1_failed_qos_flow_item.cause);

              drb_setup_item.flow_failed_list.emplace(failed_qos_flow_item.qos_flow_id, failed_qos_flow_item);
            }

            // Add DRB Data Forwarding Info Response
            if (asn1_drb_setup_item.drb_data_forwarding_info_resp_present) {
              if (asn1_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding_present) {
                drb_setup_item.drb_data_forwarding_info_resp.value().ul_data_forwarding =
                    asn1_to_up_transport_layer_info(
                        asn1_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding);
              }
              if (asn1_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding_present) {
                drb_setup_item.drb_data_forwarding_info_resp.value().dl_data_forwarding =
                    asn1_to_up_transport_layer_info(
                        asn1_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding);
              }
            }

            res_mod_item.drb_setup_list_ng_ran.emplace(drb_setup_item.drb_id, drb_setup_item);
          }

          // Add DRB session resource failed list NG RAN
          for (const auto& asn1_drb_failed_item : asn1_res_mod_item.drb_failed_mod_list_ng_ran) {
            e1ap_drb_failed_item_ng_ran drb_failed_item;
            drb_failed_item.drb_id = uint_to_drb_id(asn1_drb_failed_item.drb_id);
            drb_failed_item.cause  = e1ap_cause_to_cause(asn1_drb_failed_item.cause);

            res_mod_item.drb_failed_list_ng_ran.emplace(drb_failed_item.drb_id, drb_failed_item);
          }

          // Add Security Result
          if (asn1_res_mod_item.security_result_present) {
            asn1_to_security_result(res_mod_item.security_result.value(), asn1_res_mod_item.security_result);
          }

          // Add PDU Session Data Forwarding Info Response
          if (asn1_res_mod_item.pdu_session_data_forwarding_info_resp_present) {
            if (asn1_res_mod_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding_present) {
              res_mod_item.pdu_session_data_forwarding_info_resp.value().ul_data_forwarding =
                  asn1_to_up_transport_layer_info(
                      asn1_res_mod_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding);
            }
            if (asn1_res_mod_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding_present) {
              res_mod_item.pdu_session_data_forwarding_info_resp.value().dl_data_forwarding =
                  asn1_to_up_transport_layer_info(
                      asn1_res_mod_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding);
            }
          }

          res.pdu_session_resource_setup_list.emplace(res_mod_item.pdu_session_id, res_mod_item);
        }
      }

      // Add PDU session resource failed list
      if (asn1_bearer_context_mod_response.pdu_session_res_failed_mod_list_present) {
        for (const auto& asn1_failed_item : asn1_bearer_context_mod_response.pdu_session_res_failed_mod_list) {
          e1ap_pdu_session_resource_failed_item failed_item;

          failed_item.pdu_session_id = uint_to_pdu_session_id(asn1_failed_item.pdu_session_id);
          failed_item.cause          = e1ap_cause_to_cause(asn1_failed_item.cause);

          res.pdu_session_resource_failed_list.emplace(failed_item.pdu_session_id, failed_item);
        }
      }

      // Add PDU session resource modified list
      if (asn1_bearer_context_mod_response.pdu_session_res_modified_list_present) {
        for (const auto& asn1_res_mod_item : asn1_bearer_context_mod_response.pdu_session_res_modified_list) {
          e1ap_pdu_session_resource_modified_item res_mod_item;

          res_mod_item.pdu_session_id = uint_to_pdu_session_id(asn1_res_mod_item.pdu_session_id);

          // Add NG DL UP TNL Info
          if (asn1_res_mod_item.ng_dl_up_tnl_info_present) {
            res_mod_item.ng_dl_up_tnl_info = asn1_to_up_transport_layer_info(asn1_res_mod_item.ng_dl_up_tnl_info);
          }

          // Add DRB Setup List NG RAN
          for (const auto& asn1_drb_setup_item : asn1_res_mod_item.drb_setup_list_ng_ran) {
            e1ap_drb_setup_item_ng_ran drb_setup_item;
            drb_setup_item.drb_id = uint_to_drb_id(asn1_drb_setup_item.drb_id);

            // Add UL UP Transport Params
            for (const auto& asn1_ul_up_transport_param : asn1_drb_setup_item.ul_up_transport_params) {
              e1ap_up_params_item ue_params_item;
              ue_params_item.up_tnl_info   = asn1_to_up_transport_layer_info(asn1_ul_up_transport_param.up_tnl_info);
              ue_params_item.cell_group_id = asn1_ul_up_transport_param.cell_group_id;

              drb_setup_item.ul_up_transport_params.push_back(ue_params_item);
            }

            // Add Flow setup List
            for (const auto& asn1_qos_flow_item : asn1_drb_setup_item.flow_setup_list) {
              e1ap_qos_flow_item qos_flow_item;
              qos_flow_item.qos_flow_id = uint_to_qos_flow_id(asn1_qos_flow_item.qos_flow_id);

              drb_setup_item.flow_setup_list.emplace(qos_flow_item.qos_flow_id, qos_flow_item);
            }

            // Add Flow Failed List
            for (const auto& asn1_failed_qos_flow_item : asn1_drb_setup_item.flow_failed_list) {
              e1ap_qos_flow_failed_item failed_qos_flow_item;

              failed_qos_flow_item.qos_flow_id = uint_to_qos_flow_id(asn1_failed_qos_flow_item.qos_flow_id);
              failed_qos_flow_item.cause       = e1ap_cause_to_cause(asn1_failed_qos_flow_item.cause);

              drb_setup_item.flow_failed_list.emplace(failed_qos_flow_item.qos_flow_id, failed_qos_flow_item);
            }

            // Add DRB Data Forwarding Info Response
            if (asn1_drb_setup_item.drb_data_forwarding_info_resp_present) {
              if (asn1_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding_present) {
                drb_setup_item.drb_data_forwarding_info_resp.value().ul_data_forwarding =
                    asn1_to_up_transport_layer_info(
                        asn1_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding);
              }
              if (asn1_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding_present) {
                drb_setup_item.drb_data_forwarding_info_resp.value().dl_data_forwarding =
                    asn1_to_up_transport_layer_info(
                        asn1_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding);
              }
            }

            res_mod_item.drb_setup_list_ng_ran.emplace(drb_setup_item.drb_id, drb_setup_item);
          }

          // Add DRB session resource failed list NG RAN
          for (const auto& asn1_drb_failed_item : asn1_res_mod_item.drb_failed_list_ng_ran) {
            e1ap_drb_failed_item_ng_ran drb_failed_item;
            drb_failed_item.drb_id = uint_to_drb_id(asn1_drb_failed_item.drb_id);
            drb_failed_item.cause  = e1ap_cause_to_cause(asn1_drb_failed_item.cause);

            res_mod_item.drb_failed_list_ng_ran.emplace(drb_failed_item.drb_id, drb_failed_item);
          }

          // Add DRB modified list NG RAN
          for (const auto& asn1_drb_mod_item : asn1_res_mod_item.drb_modified_list_ng_ran) {
            e1ap_drb_modified_item_ng_ran drb_mod_item;
            drb_mod_item.drb_id = uint_to_drb_id(asn1_drb_mod_item.drb_id);

            // Add UL UP Transport Params
            for (const auto& asn1_ul_up_transport_param : asn1_drb_mod_item.ul_up_transport_params) {
              e1ap_up_params_item ue_params_item;
              ue_params_item.up_tnl_info   = asn1_to_up_transport_layer_info(asn1_ul_up_transport_param.up_tnl_info);
              ue_params_item.cell_group_id = asn1_ul_up_transport_param.cell_group_id;

              drb_mod_item.ul_up_transport_params.push_back(ue_params_item);
            }

            // Add Flow setup List
            for (const auto& asn1_qos_flow_item : asn1_drb_mod_item.flow_setup_list) {
              e1ap_qos_flow_item qos_flow_item;
              qos_flow_item.qos_flow_id = uint_to_qos_flow_id(asn1_qos_flow_item.qos_flow_id);

              drb_mod_item.flow_setup_list.emplace(qos_flow_item.qos_flow_id, qos_flow_item);
            }

            // Add Flow Failed List
            for (const auto& asn1_failed_qos_flow_item : asn1_drb_mod_item.flow_failed_list) {
              e1ap_qos_flow_failed_item failed_qos_flow_item;

              failed_qos_flow_item.qos_flow_id = uint_to_qos_flow_id(asn1_failed_qos_flow_item.qos_flow_id);
              failed_qos_flow_item.cause       = e1ap_cause_to_cause(asn1_failed_qos_flow_item.cause);

              drb_mod_item.flow_failed_list.emplace(failed_qos_flow_item.qos_flow_id, failed_qos_flow_item);
            }

            // Add PDCP SN status info
            if (asn1_drb_mod_item.pdcp_sn_status_info_present) {
              auto& asn1_pdcp_sn_status_info = asn1_drb_mod_item.pdcp_sn_status_info;

              drb_mod_item.pdcp_sn_status_info.value().pdcp_status_transfer_ul.count_value =
                  e1ap_asn1_pdcp_count_to_pdcp_count(asn1_pdcp_sn_status_info.pdcp_status_transfer_ul.count_value);

              if (asn1_pdcp_sn_status_info.pdcp_status_transfer_ul.receive_statusof_pdcp_sdu_present) {
                drb_mod_item.pdcp_sn_status_info.value().pdcp_status_transfer_ul.receive_status_of_pdcp_sdu =
                    asn1_pdcp_sn_status_info.pdcp_status_transfer_ul.receive_statusof_pdcp_sdu.to_number();
              }

              drb_mod_item.pdcp_sn_status_info.value().pdcp_status_transfer_dl =
                  e1ap_asn1_pdcp_count_to_pdcp_count(asn1_pdcp_sn_status_info.pdcp_status_transfer_dl);
            }

            res_mod_item.drb_modified_list_ng_ran.emplace(drb_mod_item.drb_id, drb_mod_item);
          }

          // Add DRB failed to modify list NG RAN
          for (const auto& asn1_drb_failed_item : asn1_res_mod_item.drb_failed_to_modify_list_ng_ran) {
            e1ap_drb_failed_item_ng_ran drb_failed_item;
            drb_failed_item.drb_id = uint_to_drb_id(asn1_drb_failed_item.drb_id);
            drb_failed_item.cause  = e1ap_cause_to_cause(asn1_drb_failed_item.cause);

            res_mod_item.drb_failed_to_modify_list_ng_ran.emplace(drb_failed_item.drb_id, drb_failed_item);
          }

          // Add Security Result
          if (asn1_res_mod_item.security_result_present) {
            asn1_to_security_result(res_mod_item.security_result.value(), asn1_res_mod_item.security_result);
          }

          // Add PDU Session Data Forwarding Info Response
          if (asn1_res_mod_item.pdu_session_data_forwarding_info_resp_present) {
            if (asn1_res_mod_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding_present) {
              res_mod_item.pdu_session_data_forwarding_info_resp.value().ul_data_forwarding =
                  asn1_to_up_transport_layer_info(
                      asn1_res_mod_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding);
            }
            if (asn1_res_mod_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding_present) {
              res_mod_item.pdu_session_data_forwarding_info_resp.value().dl_data_forwarding =
                  asn1_to_up_transport_layer_info(
                      asn1_res_mod_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding);
            }
          }

          res.pdu_session_resource_modified_list.emplace(res_mod_item.pdu_session_id, res_mod_item);
        }
      }

      // Add PDU session resource failed to modify list
      if (asn1_bearer_context_mod_response.pdu_session_res_failed_to_modify_list_present) {
        for (const auto& asn1_failed_item : asn1_bearer_context_mod_response.pdu_session_res_failed_to_modify_list) {
          e1ap_pdu_session_resource_failed_item failed_item;

          failed_item.pdu_session_id = uint_to_pdu_session_id(asn1_failed_item.pdu_session_id);
          failed_item.cause          = e1ap_cause_to_cause(asn1_failed_item.cause);

          res.pdu_session_resource_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
        }
      }
    }
  }
}

inline void fill_e1ap_bearer_context_modification_response(
    e1ap_bearer_context_modification_response&   res,
    const asn1::e1ap::bearer_context_mod_fail_s& asn1_bearer_context_modification_fail)
{
  res.success = false;
  res.cause   = e1ap_cause_to_cause(asn1_bearer_context_modification_fail->cause);
  if (asn1_bearer_context_modification_fail->crit_diagnostics_present) {
    // TODO: Add crit diagnostics
  }
}

inline void fill_asn1_bearer_context_release_command(asn1::e1ap::bearer_context_release_cmd_s&  asn1_command,
                                                     const e1ap_bearer_context_release_command& command)
{
  asn1_command->cause = cause_to_asn1_cause(command.cause);
}

} // namespace srs_cu_cp
} // namespace srsran
