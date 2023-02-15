/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../common/e1ap_asn1_converters.h"
#include "srsgnb/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"
#include "srsgnb/ran/bcd_helpers.h"

namespace srsgnb {
namespace srs_cu_up {

inline void fill_e1ap_bearer_context_setup_request(e1ap_bearer_context_setup_request&                request,
                                                   const asn1::e1ap::bearer_context_setup_request_s& asn1_request)
{
  // security info
  request.security_info.security_algorithm.ciphering_algo =
      e1ap_asn1_to_ciphering_algorithm(asn1_request->security_info.value.security_algorithm.ciphering_algorithm);

  if (asn1_request->security_info.value.security_algorithm.integrity_protection_algorithm_present) {
    request.security_info.security_algorithm.integrity_protection_algorithm = e1ap_asn1_to_integrity_algorithm(
        asn1_request->security_info.value.security_algorithm.integrity_protection_algorithm);
  }

  request.security_info.up_security_key.encryption_key =
      asn1_request->security_info.value.up_securitykey.encryption_key.copy();

  request.security_info.up_security_key.integrity_protection_key =
      asn1_request->security_info.value.up_securitykey.integrity_protection_key.copy();

  // ue dl aggregate maximum bit rate
  request.ue_dl_aggregate_maximum_bit_rate = asn1_request->ue_dl_aggr_max_bit_rate.value;

  // serving plmn
  request.serving_plmn = plmn_bcd_to_string(asn1_request->serving_plmn.value.to_number());

  // activity notification level
  request.activity_notif_level = asn1_request->activity_notif_level.value.to_string();

  // pdu session resource to setup list
  for (const auto& asn1_pdu_session_res_list_item :
       asn1_request->sys_bearer_context_setup_request.value.ng_ran_bearer_context_setup_request()) {
    for (const auto& asn1_pdu_session_res_item : asn1_pdu_session_res_list_item->pdu_session_res_to_setup_list()) {
      e1ap_pdu_session_res_to_setup_item pdu_session_res_item;

      // pdu session id
      pdu_session_res_item.pdu_session_id = uint_to_pdu_session_id(asn1_pdu_session_res_item.pdu_session_id);

      // pdu session type
      pdu_session_res_item.pdu_session_type = asn1_pdu_session_res_item.pdu_session_type.to_string();

      // s-nssai
      pdu_session_res_item.snssai = e1ap_asn1_to_snssai(asn1_pdu_session_res_item.snssai);

      // ng ul up transport layer information
      pdu_session_res_item.ng_ul_up_tnl_info =
          asn1_to_up_transport_layer_info(asn1_pdu_session_res_item.ng_ul_up_tnl_info);

      // security indication
      pdu_session_res_item.security_ind.confidentiality_protection_ind =
          asn1_pdu_session_res_item.security_ind.confidentiality_protection_ind.to_string();
      pdu_session_res_item.security_ind.integrity_protection_ind =
          asn1_pdu_session_res_item.security_ind.integrity_protection_ind.to_string();

      if (asn1_pdu_session_res_item.security_ind.max_ip_datarate_present) {
        pdu_session_res_item.security_ind.maximum_ipdatarate =
            asn1_pdu_session_res_item.security_ind.max_ip_datarate.max_ip_rate.to_string();
      }

      // drb to setup list ng ran
      for (const auto& asn1_drb_to_setup_item : asn1_pdu_session_res_item.drb_to_setup_list_ng_ran) {
        e1ap_drb_to_setup_item_ng_ran drb_to_setup_item;
        drb_to_setup_item.drb_id = uint_to_drb_id(asn1_drb_to_setup_item.drb_id);

        // sdap config
        drb_to_setup_item.sdap_cfg = e1ap_asn1_to_sdap_config(asn1_drb_to_setup_item.sdap_cfg);

        // pdcp config
        drb_to_setup_item.pdcp_cfg = e1ap_asn1_to_pdcp_config(asn1_drb_to_setup_item.pdcp_cfg);

        // cell group info
        for (const auto& asn1_cell_group_info_item : asn1_drb_to_setup_item.cell_group_info) {
          e1ap_cell_group_info_item cell_group_info_item;
          cell_group_info_item.cell_group_id = asn1_cell_group_info_item.cell_group_id;

          if (asn1_cell_group_info_item.ul_cfg_present) {
            cell_group_info_item.ul_cfg = asn1_cell_group_info_item.ul_cfg.to_string();
          }

          if (asn1_cell_group_info_item.dl_tx_stop_present) {
            cell_group_info_item.dl_tx_stop = asn1_cell_group_info_item.dl_tx_stop.to_string();
          }

          if (asn1_cell_group_info_item.rat_type_present) {
            cell_group_info_item.rat_type = asn1_cell_group_info_item.rat_type.to_string();
          }

          drb_to_setup_item.cell_group_info.push_back(cell_group_info_item);
        }

        // qos flow info to be setup
        for (const auto& asn1_qos_flow_info_item : asn1_drb_to_setup_item.qos_flow_info_to_be_setup) {
          e1ap_qos_flow_qos_param_item qos_flow_info_item;

          qos_flow_info_item.qos_flow_id = uint_to_qos_flow_id(asn1_qos_flow_info_item.qos_flow_id);

          // qos flow level qos params
          auto& asn1_qos_flow_level_params = asn1_qos_flow_info_item.qos_flow_level_qos_params;

          // dynamic 5qi
          if (asn1_qos_flow_level_params.qos_characteristics.type().value ==
              asn1::e1ap::qos_characteristics_c::types_opts::dyn_5qi) {
            auto& asn1_dynamic_5qi = asn1_qos_flow_level_params.qos_characteristics.dyn_5qi();

            e1ap_dynamic_5qi_descriptor dynamic_5qi    = {};
            dynamic_5qi.qos_prio_level                 = asn1_dynamic_5qi.qos_prio_level;
            dynamic_5qi.packet_delay_budget            = asn1_dynamic_5qi.packet_delay_budget;
            dynamic_5qi.packet_error_rate.per_scalar   = asn1_dynamic_5qi.packet_error_rate.per_scalar;
            dynamic_5qi.packet_error_rate.per_exponent = asn1_dynamic_5qi.packet_error_rate.per_exponent;

            if (asn1_dynamic_5qi.five_qi_present) {
              dynamic_5qi.five_qi = asn1_dynamic_5qi.five_qi;
            }
            if (asn1_dynamic_5qi.delay_crit_present) {
              dynamic_5qi.delay_crit = asn1_dynamic_5qi.delay_crit.to_string();
            }
            if (asn1_dynamic_5qi.averaging_win_present) {
              dynamic_5qi.averaging_win = asn1_dynamic_5qi.averaging_win;
            }
            if (asn1_dynamic_5qi.max_data_burst_volume_present) {
              dynamic_5qi.max_data_burst_volume = asn1_dynamic_5qi.max_data_burst_volume;
            }

            qos_flow_info_item.qos_flow_level_qos_params.qos_characteristics.dyn_5qi = dynamic_5qi;

          } else /* non dynamic 5qi */ {
            auto& asn1_non_dynamic_5qi = asn1_qos_flow_level_params.qos_characteristics.non_dyn_5qi();

            e1ap_non_dynamic_5qi_descriptor non_dynamic_5qi = {};

            non_dynamic_5qi.five_qi = asn1_non_dynamic_5qi.five_qi;

            if (asn1_non_dynamic_5qi.qos_prio_level_present) {
              non_dynamic_5qi.qos_prio_level = asn1_non_dynamic_5qi.qos_prio_level;
            }
            if (asn1_non_dynamic_5qi.averaging_win_present) {
              non_dynamic_5qi.averaging_win = asn1_non_dynamic_5qi.averaging_win;
            }
            if (asn1_non_dynamic_5qi.max_data_burst_volume_present) {
              non_dynamic_5qi.max_data_burst_volume = asn1_non_dynamic_5qi.max_data_burst_volume;
            }
          }

          // ng ran alloc retention prio
          qos_flow_info_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.prio_level =
              asn1_qos_flow_level_params.ngra_nalloc_retention_prio.prio_level;

          qos_flow_info_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_cap =
              asn1_qos_flow_info_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.pre_emption_cap.to_string();

          qos_flow_info_item.qos_flow_level_qos_params.ng_ran_alloc_retention_prio.pre_emption_vulnerability =
              asn1_qos_flow_info_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.pre_emption_vulnerability
                  .to_string();

          // gbr qos flow info
          if (asn1_qos_flow_level_params.gbr_qos_flow_info_present) {
            e1ap_gbr_qos_flow_info gbr_qos_flow_info = {};
            gbr_qos_flow_info.max_flow_bit_rate_dl =
                asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_flow_bit_rate_dl;
            gbr_qos_flow_info.max_flow_bit_rate_ul =
                asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_flow_bit_rate_ul;
            gbr_qos_flow_info.guaranteed_flow_bit_rate_dl =
                asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.guaranteed_flow_bit_rate_dl;
            gbr_qos_flow_info.guaranteed_flow_bit_rate_ul =
                asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.guaranteed_flow_bit_rate_ul;

            if (asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_dl_present) {
              gbr_qos_flow_info.max_packet_loss_rate_dl =
                  asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_dl;
            }
            if (asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_ul_present) {
              gbr_qos_flow_info.max_packet_loss_rate_ul =
                  asn1_qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info.max_packet_loss_rate_ul;
            }

            qos_flow_info_item.qos_flow_level_qos_params.gbr_qos_flow_info = gbr_qos_flow_info;
          }

          // reflective qos attribute
          if (asn1_qos_flow_level_params.reflective_qos_attribute_present) {
            qos_flow_info_item.qos_flow_level_qos_params.reflective_qos_attribute =
                asn1_qos_flow_level_params.reflective_qos_attribute.to_string();
          }

          // add qos info
          if (asn1_qos_flow_level_params.add_qos_info_present) {
            qos_flow_info_item.qos_flow_level_qos_params.add_qos_info =
                asn1_qos_flow_level_params.add_qos_info.to_string();
          }

          // paging policy ind
          if (asn1_qos_flow_level_params.paging_policy_ind_present) {
            qos_flow_info_item.qos_flow_level_qos_params.paging_policy_ind =
                asn1_qos_flow_level_params.paging_policy_ind;
          }

          // reflective qos ind
          if (asn1_qos_flow_level_params.reflective_qos_ind_present) {
            qos_flow_info_item.qos_flow_level_qos_params.reflective_qos_ind =
                asn1_qos_flow_level_params.reflective_qos_ind.to_string();
          }

          // qos flow map ind
          if (asn1_qos_flow_info_item.qos_flow_map_ind_present) {
            qos_flow_info_item.qos_flow_map_ind = asn1_qos_flow_info_item.qos_flow_map_ind.to_string();
          }

          drb_to_setup_item.qos_flow_info_to_be_setup.emplace(uint_to_qos_flow_id(asn1_qos_flow_info_item.qos_flow_id),
                                                              qos_flow_info_item);
        }

        // drb data forwarding info request
        if (asn1_drb_to_setup_item.drb_data_forwarding_info_request_present) {
          e1ap_data_forwarding_info_request drb_data_forwarding_info_request = {};
          drb_data_forwarding_info_request.data_forwarding_request =
              asn1_drb_to_setup_item.drb_data_forwarding_info_request.data_forwarding_request.to_string();

          for (const auto& asn1_qos_flow_map_item :
               asn1_drb_to_setup_item.drb_data_forwarding_info_request.qos_flows_forwarded_on_fwd_tunnels) {
            e1ap_qos_flow_map_item qos_flow_map_item = {};
            qos_flow_map_item.qos_flow_id            = uint_to_qos_flow_id(asn1_qos_flow_map_item.qos_flow_id);

            if (asn1_qos_flow_map_item.qos_flow_map_ind_present) {
              qos_flow_map_item.qos_flow_map_ind = asn1_qos_flow_map_item.qos_flow_map_ind.to_string();
            }

            drb_data_forwarding_info_request.qos_flows_forwarded_on_fwd_tunnels.emplace(
                uint_to_qos_flow_id(asn1_qos_flow_map_item.qos_flow_id), qos_flow_map_item);
          }

          drb_to_setup_item.drb_data_forwarding_info_request = drb_data_forwarding_info_request;
        }

        // drb inactivity timer
        if (asn1_drb_to_setup_item.drb_inactivity_timer_present) {
          drb_to_setup_item.drb_inactivity_timer = asn1_drb_to_setup_item.drb_inactivity_timer;
        }

        // pdcp sn status info
        if (asn1_drb_to_setup_item.pdcp_sn_status_info_present) {
          e1ap_pdcp_sn_status_info pdcp_sn_status_info = {};

          pdcp_sn_status_info.pdcp_status_transfer_ul.count_value.pdcp_sn =
              asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_ul.count_value.pdcp_sn;
          pdcp_sn_status_info.pdcp_status_transfer_ul.count_value.hfn =
              asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_ul.count_value.hfn;

          if (asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_ul.receive_statusof_pdcp_sdu_present) {
            pdcp_sn_status_info.pdcp_status_transfer_ul.receive_statusof_pdcpsdu =
                asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_ul.receive_statusof_pdcp_sdu
                    .to_number();
          }

          pdcp_sn_status_info.pdcp_status_transfer_dl.pdcp_sn =
              asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_dl.pdcp_sn;
          pdcp_sn_status_info.pdcp_status_transfer_dl.hfn =
              asn1_drb_to_setup_item.pdcp_sn_status_info.pdcp_status_transfer_dl.hfn;

          drb_to_setup_item.pdcp_sn_status_info = pdcp_sn_status_info;
        }

        pdu_session_res_item.drb_to_setup_list_ng_ran.emplace(uint_to_drb_id(asn1_drb_to_setup_item.drb_id),
                                                              drb_to_setup_item);
      }

      // pdu session resource dl aggregate maximum bit rate
      if (asn1_pdu_session_res_item.pdu_session_res_dl_ambr_present) {
        pdu_session_res_item.pdu_session_res_dl_ambr = asn1_pdu_session_res_item.pdu_session_res_dl_ambr;
      }

      // pdu session data forwarding info request
      if (asn1_pdu_session_res_item.pdu_session_data_forwarding_info_request_present) {
        e1ap_data_forwarding_info_request data_forwarding_info_req = {};
        data_forwarding_info_req.data_forwarding_request =
            asn1_pdu_session_res_item.pdu_session_data_forwarding_info_request.data_forwarding_request.to_string();

        for (const auto& asn1_qos_flow_map_item :
             asn1_pdu_session_res_item.pdu_session_data_forwarding_info_request.qos_flows_forwarded_on_fwd_tunnels) {
          e1ap_qos_flow_map_item qos_flow_map_item = {};
          qos_flow_map_item.qos_flow_id            = uint_to_qos_flow_id(asn1_qos_flow_map_item.qos_flow_id);

          if (asn1_qos_flow_map_item.qos_flow_map_ind_present) {
            qos_flow_map_item.qos_flow_map_ind = asn1_qos_flow_map_item.qos_flow_map_ind.to_string();
          }

          data_forwarding_info_req.qos_flows_forwarded_on_fwd_tunnels.emplace(
              uint_to_qos_flow_id(asn1_qos_flow_map_item.qos_flow_id), qos_flow_map_item);
        }
      }

      // pdu session inactivity timer
      if (asn1_pdu_session_res_item.pdu_session_inactivity_timer_present) {
        pdu_session_res_item.pdu_session_inactivity_timer = asn1_pdu_session_res_item.pdu_session_inactivity_timer;
      }

      // existing allocated ng dl up tnl info
      if (asn1_pdu_session_res_item.existing_allocated_ng_dl_up_tnl_info_present) {
        pdu_session_res_item.existing_allocated_ng_dl_up_tnl_info =
            asn1_to_up_transport_layer_info(asn1_pdu_session_res_item.existing_allocated_ng_dl_up_tnl_info);
      }

      // network instance
      if (asn1_pdu_session_res_item.network_instance_present) {
        pdu_session_res_item.network_instance = asn1_pdu_session_res_item.network_instance;
      }
      request.pdu_session_res_to_setup_list.emplace(uint_to_pdu_session_id(asn1_pdu_session_res_item.pdu_session_id),
                                                    pdu_session_res_item);
    }
  }

  // ue dl maximum integrity protection data rate
  if (asn1_request->ue_dl_max_integrity_protected_data_rate_present) {
    request.ue_dl_maximum_integrity_protected_data_rate = asn1_request->ue_dl_max_integrity_protected_data_rate.value;
  }

  // ue inactivity timer
  if (asn1_request->ue_inactivity_timer_present) {
    request.ue_inactivity_timer = asn1_request->ue_inactivity_timer.value;
  }

  // bearer context status change
  if (asn1_request->bearer_context_status_change_present) {
    request.bearer_context_status_change = asn1_request->bearer_context_status_change.value.to_string();
  }

  // ran ue id
  if (asn1_request->ran_ue_id_present) {
    request.ran_ue_id = uint_to_ran_ue_id(asn1_request->ran_ue_id.value.to_number());
  }

  // gnb du id
  if (asn1_request->gnb_du_id_present) {
    request.gnb_du_id = asn1_request->gnb_du_id.value;
  }
}

inline void fill_asn1_bearer_context_setup_response(asn1::e1ap::sys_bearer_context_setup_resp_c& asn1_response,
                                                    const e1ap_bearer_context_setup_response&    response)
{
  auto& asn1_bearer_context_setup_response = asn1_response.set_ng_ran_bearer_context_setup_resp();

  // Add PDU Session Resource Setup List
  for (const auto& res_setup_item : response.pdu_session_resource_setup_list) {
    asn1::e1ap::pdu_session_res_setup_item_s asn1_res_setup_item;

    // Add PDU Session ID
    asn1_res_setup_item.pdu_session_id = pdu_session_id_to_uint(res_setup_item.pdu_session_id);

    // Add NG DL UP TNL Info
    up_transport_layer_info_to_asn1(asn1_res_setup_item.ng_dl_up_tnl_info, res_setup_item.ng_dl_up_tnl_info);

    // Add DRB Setup List NG RAN
    for (const auto& drb_setup_item : res_setup_item.drb_setup_list_ng_ran) {
      asn1::e1ap::drb_setup_item_ng_ran_s asn1_drb_setup_item;

      // Add DRB ID
      asn1_drb_setup_item.drb_id = drb_id_to_uint(drb_setup_item.drb_id);

      // Add UL UP Transport Params
      for (const auto& ul_up_transport_param : drb_setup_item.ul_up_transport_params) {
        asn1::e1ap::up_params_item_s asn1_up_transport_param;
        up_transport_layer_info_to_asn1(asn1_up_transport_param.up_tnl_info, ul_up_transport_param.up_tnl_info);
        asn1_drb_setup_item.ul_up_transport_params.push_back(asn1_up_transport_param);
      }

      // Add Flow setup List
      for (const auto& qos_flow_item : drb_setup_item.flow_setup_list) {
        asn1::e1ap::qos_flow_item_s asn1_flow_item;
        asn1_flow_item.qos_flow_id = qos_flow_id_to_uint(qos_flow_item.qos_flow_id);
        asn1_drb_setup_item.flow_setup_list.push_back(asn1_flow_item);
      }

      // Add Flow Failed List
      for (const auto& flow_failed_item : drb_setup_item.flow_failed_list) {
        asn1::e1ap::qos_flow_failed_item_s asn1_flow_failed_item;
        asn1_flow_failed_item.qos_flow_id = qos_flow_id_to_uint(flow_failed_item.qos_flow_id);
        asn1_drb_setup_item.flow_failed_list.push_back(asn1_flow_failed_item);
      }

      // Add DRB Data Forwarding Info Response
      if (drb_setup_item.drb_data_forwarding_info_resp.has_value()) {
        asn1_drb_setup_item.drb_data_forwarding_info_resp_present = true;
        if (drb_setup_item.drb_data_forwarding_info_resp.value().ul_data_forwarding.has_value()) {
          asn1_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding_present = true;
          up_transport_layer_info_to_asn1(
              asn1_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding,
              drb_setup_item.drb_data_forwarding_info_resp.value().ul_data_forwarding.value());
        }
        if (drb_setup_item.drb_data_forwarding_info_resp.value().dl_data_forwarding.has_value()) {
          asn1_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding_present = true;
          up_transport_layer_info_to_asn1(
              asn1_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding,
              drb_setup_item.drb_data_forwarding_info_resp.value().dl_data_forwarding.value());
        }
      }

      asn1_res_setup_item.drb_setup_list_ng_ran.push_back(asn1_drb_setup_item);
    }

    // Add DRB Failed List NG RAN
    for (const auto& drb_failed_item : res_setup_item.drb_failed_list_ng_ran) {
      asn1::e1ap::drb_failed_item_ng_ran_s asn1_drb_failed_item;
      // Add DRB ID
      asn1_drb_failed_item.drb_id = drb_id_to_uint(drb_failed_item.drb_id);
      // Add Cause
      asn1_drb_failed_item.cause = cause_to_e1ap_cause(drb_failed_item.cause);
      asn1_res_setup_item.drb_failed_list_ng_ran.push_back(asn1_drb_failed_item);
    }

    // Add Security Result
    if (res_setup_item.security_result.has_value()) {
      asn1_res_setup_item.security_result_present = true;
      asn1::string_to_enum(asn1_res_setup_item.security_result.confidentiality_protection_result,
                           res_setup_item.security_result.value().confidentiality_protection_result);
      asn1::string_to_enum(asn1_res_setup_item.security_result.integrity_protection_result,
                           res_setup_item.security_result.value().integrity_protection_result);
    }

    // Add PDU Session Data Forwarding Info Response
    if (res_setup_item.pdu_session_data_forwarding_info_resp.has_value()) {
      asn1_res_setup_item.pdu_session_data_forwarding_info_resp_present = true;
      if (res_setup_item.pdu_session_data_forwarding_info_resp.value().ul_data_forwarding.has_value()) {
        asn1_res_setup_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding_present = true;
        up_transport_layer_info_to_asn1(
            asn1_res_setup_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding,
            res_setup_item.pdu_session_data_forwarding_info_resp.value().ul_data_forwarding.value());
      }
      if (res_setup_item.pdu_session_data_forwarding_info_resp.value().dl_data_forwarding.has_value()) {
        asn1_res_setup_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding_present = true;
        up_transport_layer_info_to_asn1(
            asn1_res_setup_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding,
            res_setup_item.pdu_session_data_forwarding_info_resp.value().dl_data_forwarding.value());
      }
    }

    // Add NG DL UP Unchanged
    if (res_setup_item.ng_dl_up_unchanged.has_value()) {
      asn1_res_setup_item.ng_dl_up_unchanged_present = true;
      if (res_setup_item.ng_dl_up_unchanged.value()) {
        asn1_res_setup_item.ng_dl_up_unchanged.value =
            asn1::e1ap::pdu_session_res_setup_item_s::ng_dl_up_unchanged_opts::options::true_value;
      } else {
        asn1_res_setup_item.ng_dl_up_unchanged.value =
            asn1::e1ap::pdu_session_res_setup_item_s::ng_dl_up_unchanged_opts::options::nulltype;
      }
    }

    asn1_bearer_context_setup_response.pdu_session_res_setup_list.value.push_back(asn1_res_setup_item);
  }

  // Add pdu session res failed list
  if (!response.pdu_session_resource_failed_list.empty()) {
    asn1_bearer_context_setup_response.pdu_session_res_failed_list_present = true;
    for (const auto& failed_item : response.pdu_session_resource_failed_list) {
      asn1::e1ap::pdu_session_res_failed_item_s asn1_failed_item;
      asn1_failed_item.pdu_session_id = pdu_session_id_to_uint(failed_item.pdu_session_id);
      asn1_failed_item.cause          = cause_to_e1ap_cause(failed_item.cause);

      asn1_bearer_context_setup_response.pdu_session_res_failed_list.value.push_back(asn1_failed_item);
    }
  }
}

} // namespace srs_cu_up
} // namespace srsgnb
