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

#include "../converters/e1ap_asn1_converters.h"

namespace srsgnb {
namespace srs_cu_cp {

inline void fill_e1ap_bearer_context_setup_request(e1ap_bearer_context_setup_request_message&         e1_request,
                                                   const rrc_ue_bearer_context_setup_request_message& msg)
{
  e1_request.sys_bearer_context_setup_request.set_ng_ran_bearer_context_setup_request();
  auto& ng_ran_bearer_context_setup_request =
      e1_request.sys_bearer_context_setup_request.ng_ran_bearer_context_setup_request();

  for (auto pdu_session_res_item : msg.pdu_session_res_setup_items) {
    asn1::protocol_ie_field_s<asn1::e1ap::ng_ran_bearer_context_setup_request_o> bearer_request_item;
    auto& e1ap_pdu_session_res_items = bearer_request_item.value().pdu_session_res_to_setup_list();

    asn1::e1ap::pdu_session_res_to_setup_item_s e1ap_pdu_session_res_item;

    // pdu session id
    e1ap_pdu_session_res_item.pdu_session_id = pdu_session_res_item.pdu_session_id;

    // s-NSSAI
    e1ap_pdu_session_res_item.snssai = common_snssai_to_e1ap_snssai(pdu_session_res_item.s_nssai);

    // pdu session type
    e1ap_pdu_session_res_item.pdu_session_type =
        cu_cp_pdu_session_type_to_e1ap_pdu_session_type(pdu_session_res_item.pdu_session_type);

    // ng ul up transport layer information
    up_transport_layer_info_to_asn1(e1ap_pdu_session_res_item.ng_ul_up_tnl_info,
                                    pdu_session_res_item.ul_ngu_up_tnl_info);

    // TODO: add optional values

    // pdu session resource dl aggregate maximum bit rate
    e1ap_pdu_session_res_item.pdu_session_res_dl_ambr_present = true;
    e1ap_pdu_session_res_item.pdu_session_res_dl_ambr = pdu_session_res_item.pdu_session_aggregate_maximum_bit_rate_dl;

    e1ap_pdu_session_res_items.push_back(e1ap_pdu_session_res_item);

    ng_ran_bearer_context_setup_request.push_back(bearer_request_item);
  }

  e1_request.uedl_aggregate_maximum_bit_rate = msg.ue_aggregate_maximum_bit_rate_dl;
}

inline void fill_rrc_ue_bearer_context_setup_response_message(
    rrc_ue_bearer_context_setup_response_message&     res,
    const e1ap_bearer_context_setup_response_message& e1_bearer_context_setup_resp_msg)
{
  if (e1_bearer_context_setup_resp_msg.success) {
    auto& bearer_context_setup_response =
        e1_bearer_context_setup_resp_msg.response->sys_bearer_context_setup_resp->ng_ran_bearer_context_setup_resp();

    for (auto e1ap_res_setup_item : bearer_context_setup_response.pdu_session_res_setup_list.value) {
      rrc_ue_pdu_session_resource_setup_modification_item res_setup_item;
      res_setup_item.pdu_session_id = e1ap_res_setup_item.pdu_session_id;

      // Add NG DL UP TNL Info
      res_setup_item.ng_dl_up_tnl_info = asn1_to_up_transport_layer_info(e1ap_res_setup_item.ng_dl_up_tnl_info);

      // Add DRB Setup List NG RAN
      for (auto e1ap_drb_setup_item : e1ap_res_setup_item.drb_setup_list_ng_ran) {
        rrc_ue_drb_setup_item_ng_ran drb_setup_item;
        drb_setup_item.drb_id = uint_to_drb_id(e1ap_drb_setup_item.drb_id);

        // Add UL UP Transport Params
        for (auto ul_up_transport_param : e1ap_drb_setup_item.ul_up_transport_params) {
          rrc_ue_up_params_item ue_params_item;
          ue_params_item.up_tnl_info   = asn1_to_up_transport_layer_info(e1ap_res_setup_item.ng_dl_up_tnl_info);
          ue_params_item.cell_group_id = ul_up_transport_param.cell_group_id;

          drb_setup_item.ul_up_transport_params.push_back(ue_params_item);
        }

        // Add Flow setup List
        for (auto e1ap_qos_flow_item : e1ap_drb_setup_item.flow_setup_list) {
          rrc_ue_qos_flow_item qos_flow_item;
          qos_flow_item.qos_flow_id = e1ap_qos_flow_item.qo_s_flow_id;

          drb_setup_item.flow_setup_list.push_back(qos_flow_item);
        }

        // Add Flow Failed List
        for (auto e1ap_failed_qos_flow_item : e1ap_drb_setup_item.flow_failed_list) {
          rrc_ue_qos_flow_failed_item failed_qos_flow_item;

          failed_qos_flow_item.qos_flow_id = e1ap_failed_qos_flow_item.qo_s_flow_id;
          failed_qos_flow_item.cause       = e1ap_cause_to_cu_cp_cause(e1ap_failed_qos_flow_item.cause);

          drb_setup_item.flow_failed_list.push_back(failed_qos_flow_item);
        }

        // Add DRB Data Forwarding Info Response
        if (e1ap_drb_setup_item.drb_data_forwarding_info_resp_present) {
          if (e1ap_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding_present) {
            drb_setup_item.drb_data_forwarding_info_resp.value().ul_data_forwarding =
                asn1_to_up_transport_layer_info(e1ap_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding);
          }
          if (e1ap_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding_present) {
            drb_setup_item.drb_data_forwarding_info_resp.value().dl_data_forwarding =
                asn1_to_up_transport_layer_info(e1ap_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding);
          }
        }

        res_setup_item.drb_setup_list_ng_ran.push_back(drb_setup_item);
      }

      // Add DRB Failed List NG RAN
      for (auto e1ap_drb_failed_item : e1ap_res_setup_item.drb_failed_list_ng_ran) {
        rrc_ue_drb_failed_item_ng_ran drb_failed_item;
        drb_failed_item.drb_id = uint_to_drb_id(e1ap_drb_failed_item.drb_id);
        drb_failed_item.cause  = e1ap_cause_to_cu_cp_cause(e1ap_drb_failed_item.cause);

        res_setup_item.drb_failed_list_ng_ran.push_back(drb_failed_item);
      }

      // Add Security Result
      if (e1ap_res_setup_item.security_result_present) {
        res_setup_item.security_result.value().confidentiality_protection_result =
            e1ap_res_setup_item.security_result.confidentiality_protection_result.to_string();
        res_setup_item.security_result.value().integrity_protection_result =
            e1ap_res_setup_item.security_result.integrity_protection_result.to_string();
      }

      // Add PDU Session Data Forwarding Info Response
      if (e1ap_res_setup_item.pdu_session_data_forwarding_info_resp_present) {
        if (e1ap_res_setup_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding_present) {
          res_setup_item.pdu_session_data_forwarding_info_resp.value().ul_data_forwarding =
              asn1_to_up_transport_layer_info(
                  e1ap_res_setup_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding);
        }
        if (e1ap_res_setup_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding_present) {
          res_setup_item.pdu_session_data_forwarding_info_resp.value().dl_data_forwarding =
              asn1_to_up_transport_layer_info(
                  e1ap_res_setup_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding);
        }
      }

      // Add NG DL UP Unchanged
      if (e1ap_res_setup_item.ng_dl_up_unchanged_present) {
        res_setup_item.ng_dl_up_unchanged.value() =
            e1ap_ng_dl_up_unchanged_to_bool(e1ap_res_setup_item.ng_dl_up_unchanged);
      }

      res.pdu_session_resource_setup_list.push_back(res_setup_item);
    }

    // Add pdu session res failed list
    if (bearer_context_setup_response.pdu_session_res_failed_list_present) {
      for (auto e1ap_failed_item : bearer_context_setup_response.pdu_session_res_failed_list.value) {
        rrc_ue_pdu_session_resource_failed_item failed_item;

        failed_item.pdu_session_id = e1ap_failed_item.pdu_session_id;
        failed_item.cause          = e1ap_cause_to_cu_cp_cause(e1ap_failed_item.cause);

        res.pdu_session_resource_failed_list.push_back(failed_item);
      }
    }
  } else {
    res.success = false;
    res.cause   = e1ap_cause_to_cu_cp_cause(e1_bearer_context_setup_resp_msg.failure->cause.value);
    if (e1_bearer_context_setup_resp_msg.failure->crit_diagnostics_present) {
      // TODO: Add crit diagnostics
    }
  }
}

inline void fill_e1ap_bearer_context_modification_request(e1ap_bearer_context_modification_request_message& e1_request,
                                                          const rrc_ue_bearer_context_modification_request_message& msg)
{
  // TODO: Add needed info to input struct and fill msg
}

inline void fill_rrc_ue_bearer_context_modification_response_message(
    rrc_ue_bearer_context_modification_response_message&     res,
    const e1ap_bearer_context_modification_response_message& e1_bearer_context_modification_resp_msg)
{
  if (e1_bearer_context_modification_resp_msg.success) {
    // Add NG RAN bearer context modification response
    if (e1_bearer_context_modification_resp_msg.response->sys_bearer_context_mod_resp_present) {
      auto& bearer_context_mod_response = e1_bearer_context_modification_resp_msg.response->sys_bearer_context_mod_resp
                                              ->ng_ran_bearer_context_mod_resp();

      // Add PDU session resource setup list
      if (bearer_context_mod_response.pdu_session_res_setup_mod_list_present) {
        for (auto e1ap_res_mod_item : bearer_context_mod_response.pdu_session_res_setup_mod_list.value) {
          rrc_ue_pdu_session_resource_setup_modification_item res_mod_item;
          res_mod_item.pdu_session_id = e1ap_res_mod_item.pdu_session_id;

          // Add NG DL UP TNL Info
          res_mod_item.ng_dl_up_tnl_info = asn1_to_up_transport_layer_info(e1ap_res_mod_item.ng_dl_up_tnl_info);

          // Add DRB Setup List NG RAN
          for (auto e1ap_drb_setup_item : e1ap_res_mod_item.drb_setup_mod_list_ng_ran) {
            rrc_ue_drb_setup_item_ng_ran drb_setup_item;
            drb_setup_item.drb_id = uint_to_drb_id(e1ap_drb_setup_item.drb_id);

            // Add UL UP Transport Params
            for (const asn1::e1ap::up_params_item_s& ul_up_transport_param :
                 e1ap_drb_setup_item.ul_up_transport_params) {
              rrc_ue_up_params_item ue_params_item;
              ue_params_item.up_tnl_info   = asn1_to_up_transport_layer_info(ul_up_transport_param.up_tnl_info);
              ue_params_item.cell_group_id = ul_up_transport_param.cell_group_id;

              drb_setup_item.ul_up_transport_params.push_back(ue_params_item);
            }

            // Add Flow setup List
            for (auto e1ap_qos_flow_item : e1ap_drb_setup_item.flow_setup_list) {
              rrc_ue_qos_flow_item qos_flow_item;
              qos_flow_item.qos_flow_id = e1ap_qos_flow_item.qo_s_flow_id;

              drb_setup_item.flow_setup_list.push_back(qos_flow_item);
            }

            // Add Flow Failed List
            for (auto e1ap_failed_qos_flow_item : e1ap_drb_setup_item.flow_failed_list) {
              rrc_ue_qos_flow_failed_item failed_qos_flow_item;

              failed_qos_flow_item.qos_flow_id = e1ap_failed_qos_flow_item.qo_s_flow_id;
              failed_qos_flow_item.cause       = e1ap_cause_to_cu_cp_cause(e1ap_failed_qos_flow_item.cause);

              drb_setup_item.flow_failed_list.push_back(failed_qos_flow_item);
            }

            // Add DRB Data Forwarding Info Response
            if (e1ap_drb_setup_item.drb_data_forwarding_info_resp_present) {
              if (e1ap_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding_present) {
                drb_setup_item.drb_data_forwarding_info_resp.value().ul_data_forwarding =
                    asn1_to_up_transport_layer_info(
                        e1ap_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding);
              }
              if (e1ap_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding_present) {
                drb_setup_item.drb_data_forwarding_info_resp.value().dl_data_forwarding =
                    asn1_to_up_transport_layer_info(
                        e1ap_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding);
              }
            }

            res_mod_item.drb_setup_list_ng_ran.push_back(drb_setup_item);
          }

          // Add DRB session resource failed list NG RAN
          for (auto e1ap_drb_failed_item : e1ap_res_mod_item.drb_failed_mod_list_ng_ran) {
            rrc_ue_drb_failed_item_ng_ran drb_failed_item;
            drb_failed_item.drb_id = uint_to_drb_id(e1ap_drb_failed_item.drb_id);
            drb_failed_item.cause  = e1ap_cause_to_cu_cp_cause(e1ap_drb_failed_item.cause);

            res_mod_item.drb_failed_list_ng_ran.push_back(drb_failed_item);
          }

          // Add Security Result
          if (e1ap_res_mod_item.security_result_present) {
            res_mod_item.security_result.value().confidentiality_protection_result =
                e1ap_res_mod_item.security_result.confidentiality_protection_result.to_string();
            res_mod_item.security_result.value().integrity_protection_result =
                e1ap_res_mod_item.security_result.integrity_protection_result.to_string();
          }

          // Add PDU Session Data Forwarding Info Response
          if (e1ap_res_mod_item.pdu_session_data_forwarding_info_resp_present) {
            if (e1ap_res_mod_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding_present) {
              res_mod_item.pdu_session_data_forwarding_info_resp.value().ul_data_forwarding =
                  asn1_to_up_transport_layer_info(
                      e1ap_res_mod_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding);
            }
            if (e1ap_res_mod_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding_present) {
              res_mod_item.pdu_session_data_forwarding_info_resp.value().dl_data_forwarding =
                  asn1_to_up_transport_layer_info(
                      e1ap_res_mod_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding);
            }
          }

          res.pdu_session_resource_setup_list.push_back(res_mod_item);
        }
      }

      // Add PDU session resource failed list
      if (bearer_context_mod_response.pdu_session_res_failed_mod_list_present) {
        for (auto e1ap_failed_item : bearer_context_mod_response.pdu_session_res_failed_mod_list.value) {
          rrc_ue_pdu_session_resource_failed_item failed_item;

          failed_item.pdu_session_id = e1ap_failed_item.pdu_session_id;
          failed_item.cause          = e1ap_cause_to_cu_cp_cause(e1ap_failed_item.cause);

          res.pdu_session_resource_failed_list.push_back(failed_item);
        }
      }

      // Add PDU session resource modified list
      if (bearer_context_mod_response.pdu_session_res_modified_list_present) {
        for (auto e1ap_res_mod_item : bearer_context_mod_response.pdu_session_res_modified_list.value) {
          rrc_ue_pdu_session_resource_modified_item res_mod_item;

          res_mod_item.pdu_session_id = e1ap_res_mod_item.pdu_session_id;

          // Add NG DL UP TNL Info
          res_mod_item.ng_dl_up_tnl_info = asn1_to_up_transport_layer_info(e1ap_res_mod_item.ng_dl_up_tnl_info);

          // Add DRB Setup List NG RAN
          for (auto e1ap_drb_setup_item : e1ap_res_mod_item.drb_setup_list_ng_ran) {
            rrc_ue_drb_setup_item_ng_ran drb_setup_item;
            drb_setup_item.drb_id = uint_to_drb_id(e1ap_drb_setup_item.drb_id);

            // Add UL UP Transport Params
            for (const auto& ul_up_transport_param : e1ap_drb_setup_item.ul_up_transport_params) {
              rrc_ue_up_params_item ue_params_item;
              ue_params_item.up_tnl_info   = asn1_to_up_transport_layer_info(ul_up_transport_param.up_tnl_info);
              ue_params_item.cell_group_id = ul_up_transport_param.cell_group_id;

              drb_setup_item.ul_up_transport_params.push_back(ue_params_item);
            }

            // Add Flow setup List
            for (auto e1ap_qos_flow_item : e1ap_drb_setup_item.flow_setup_list) {
              rrc_ue_qos_flow_item qos_flow_item;
              qos_flow_item.qos_flow_id = e1ap_qos_flow_item.qo_s_flow_id;

              drb_setup_item.flow_setup_list.push_back(qos_flow_item);
            }

            // Add Flow Failed List
            for (auto e1ap_failed_qos_flow_item : e1ap_drb_setup_item.flow_failed_list) {
              rrc_ue_qos_flow_failed_item failed_qos_flow_item;

              failed_qos_flow_item.qos_flow_id = e1ap_failed_qos_flow_item.qo_s_flow_id;
              failed_qos_flow_item.cause       = e1ap_cause_to_cu_cp_cause(e1ap_failed_qos_flow_item.cause);

              drb_setup_item.flow_failed_list.push_back(failed_qos_flow_item);
            }

            // Add DRB Data Forwarding Info Response
            if (e1ap_drb_setup_item.drb_data_forwarding_info_resp_present) {
              if (e1ap_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding_present) {
                drb_setup_item.drb_data_forwarding_info_resp.value().ul_data_forwarding =
                    asn1_to_up_transport_layer_info(
                        e1ap_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding);
              }
              if (e1ap_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding_present) {
                drb_setup_item.drb_data_forwarding_info_resp.value().dl_data_forwarding =
                    asn1_to_up_transport_layer_info(
                        e1ap_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding);
              }
            }

            res_mod_item.drb_setup_list_ng_ran.push_back(drb_setup_item);
          }

          // Add DRB session resource failed list NG RAN
          for (auto e1ap_drb_failed_item : e1ap_res_mod_item.drb_failed_list_ng_ran) {
            rrc_ue_drb_failed_item_ng_ran drb_failed_item;
            drb_failed_item.drb_id = uint_to_drb_id(e1ap_drb_failed_item.drb_id);
            drb_failed_item.cause  = e1ap_cause_to_cu_cp_cause(e1ap_drb_failed_item.cause);

            res_mod_item.drb_failed_list_ng_ran.push_back(drb_failed_item);
          }

          // Add DRB modified list NG RAN
          for (auto e1ap_drb_mod_item : e1ap_res_mod_item.drb_modified_list_ng_ran) {
            rrc_ue_drb_modified_item_ng_ran drb_mod_item;
            drb_mod_item.drb_id = uint_to_drb_id(e1ap_drb_mod_item.drb_id);

            // Add UL UP Transport Params
            for (const auto& ul_up_transport_param : e1ap_drb_mod_item.ul_up_transport_params) {
              rrc_ue_up_params_item ue_params_item;
              ue_params_item.up_tnl_info   = asn1_to_up_transport_layer_info(ul_up_transport_param.up_tnl_info);
              ue_params_item.cell_group_id = ul_up_transport_param.cell_group_id;

              drb_mod_item.ul_up_transport_params.push_back(ue_params_item);
            }

            // Add Flow setup List
            for (auto e1ap_qos_flow_item : e1ap_drb_mod_item.flow_setup_list) {
              rrc_ue_qos_flow_item qos_flow_item;
              qos_flow_item.qos_flow_id = e1ap_qos_flow_item.qo_s_flow_id;

              drb_mod_item.flow_setup_list.push_back(qos_flow_item);
            }

            // Add Flow Failed List
            for (auto e1ap_failed_qos_flow_item : e1ap_drb_mod_item.flow_failed_list) {
              rrc_ue_qos_flow_failed_item failed_qos_flow_item;

              failed_qos_flow_item.qos_flow_id = e1ap_failed_qos_flow_item.qo_s_flow_id;
              failed_qos_flow_item.cause       = e1ap_cause_to_cu_cp_cause(e1ap_failed_qos_flow_item.cause);

              drb_mod_item.flow_failed_list.push_back(failed_qos_flow_item);
            }

            // Add PDCP SN status info
            if (e1ap_drb_mod_item.pdcp_sn_status_info_present) {
              auto& e1ap_pdcp_sn_status_info = e1ap_drb_mod_item.pdcp_sn_status_info;

              drb_mod_item.pdcp_sn_status_info.value().pdcp_status_transfer_ul.count_value =
                  e1ap_pdcp_count_to_rrc_ue_pdcp_count(e1ap_pdcp_sn_status_info.pdcp_status_transfer_ul.count_value);

              if (e1ap_pdcp_sn_status_info.pdcp_status_transfer_ul.receive_statusof_pdcpsdu_present) {
                drb_mod_item.pdcp_sn_status_info.value().pdcp_status_transfer_ul.receive_statusof_pdcpsdu =
                    e1ap_pdcp_sn_status_info.pdcp_status_transfer_ul.receive_statusof_pdcpsdu.to_number();
              }

              drb_mod_item.pdcp_sn_status_info.value().pdcp_status_transfer_dl =
                  e1ap_pdcp_count_to_rrc_ue_pdcp_count(e1ap_pdcp_sn_status_info.pdcp_status_transfer_dl);
            }

            res_mod_item.drb_modified_list_ng_ran.push_back(drb_mod_item);
          }

          // Add DRB failed to modify list NG RAN
          for (auto e1ap_drb_failed_item : e1ap_res_mod_item.drb_failed_to_modify_list_ng_ran) {
            rrc_ue_drb_failed_item_ng_ran drb_failed_item;
            drb_failed_item.drb_id = uint_to_drb_id(e1ap_drb_failed_item.drb_id);
            drb_failed_item.cause  = e1ap_cause_to_cu_cp_cause(e1ap_drb_failed_item.cause);

            res_mod_item.drb_failed_to_modify_list_ng_ran.push_back(drb_failed_item);
          }

          // Add Security Result
          if (e1ap_res_mod_item.security_result_present) {
            res_mod_item.security_result.value().confidentiality_protection_result =
                e1ap_res_mod_item.security_result.confidentiality_protection_result.to_string();
            res_mod_item.security_result.value().integrity_protection_result =
                e1ap_res_mod_item.security_result.integrity_protection_result.to_string();
          }

          // Add PDU Session Data Forwarding Info Response
          if (e1ap_res_mod_item.pdu_session_data_forwarding_info_resp_present) {
            if (e1ap_res_mod_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding_present) {
              res_mod_item.pdu_session_data_forwarding_info_resp.value().ul_data_forwarding =
                  asn1_to_up_transport_layer_info(
                      e1ap_res_mod_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding);
            }
            if (e1ap_res_mod_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding_present) {
              res_mod_item.pdu_session_data_forwarding_info_resp.value().dl_data_forwarding =
                  asn1_to_up_transport_layer_info(
                      e1ap_res_mod_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding);
            }
          }

          res.pdu_session_resource_modified_list.push_back(res_mod_item);
        }
      }

      // Add PDU session resource failed to modify list
      if (bearer_context_mod_response.pdu_session_res_failed_to_modify_list_present) {
        for (auto e1ap_failed_item : bearer_context_mod_response.pdu_session_res_failed_to_modify_list.value) {
          rrc_ue_pdu_session_resource_failed_item failed_item;

          failed_item.pdu_session_id = e1ap_failed_item.pdu_session_id;
          failed_item.cause          = e1ap_cause_to_cu_cp_cause(e1ap_failed_item.cause);

          res.pdu_session_resource_failed_to_modify_list.push_back(failed_item);
        }
      }
    }
  } else {
    res.success = false;
    res.cause   = e1ap_cause_to_cu_cp_cause(e1_bearer_context_modification_resp_msg.failure->cause.value);
    if (e1_bearer_context_modification_resp_msg.failure->crit_diagnostics_present) {
      // TODO: Add crit diagnostics
    }
  }
}

} // namespace srs_cu_cp
} // namespace srsgnb
