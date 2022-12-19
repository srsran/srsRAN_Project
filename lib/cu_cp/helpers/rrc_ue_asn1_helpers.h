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
#include "../converters/f1ap_asn1_converters.h"
#include "../converters/rrc_ue_asn1_converters.h"
#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/f1c/cu_cp/f1c_cu.h"

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
    e1ap_pdu_session_res_item.ng_ul_up_tnl_info =
        cu_cp_ul_ngu_up_tnl_info_to_e1ap_up_tnl_info(pdu_session_res_item.ul_ngu_up_tnl_info);

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
      rrc_ue_pdu_session_resource_setup_item res_setup_item;
      res_setup_item.pdu_session_id = e1ap_res_setup_item.pdu_session_id;

      // Add NG DL UP TNL Info
      res_setup_item.ng_dl_up_tnl_info = e1ap_up_tnl_info_to_cu_cp_gtp_tunnel(e1ap_res_setup_item.ng_dl_up_tnl_info);

      // Add DRB Setup List NG RAN
      for (auto e1ap_drb_setup_item : e1ap_res_setup_item.drb_setup_list_ng_ran) {
        rrc_ue_drb_setup_item_ng_ran drb_setup_item;
        drb_setup_item.drb_id = uint_to_drb_id(e1ap_drb_setup_item.drb_id);

        // Add UL UP Transport Params
        for (auto ul_up_transport_param : e1ap_drb_setup_item.ul_up_transport_params) {
          rrc_ue_up_params_item ue_params_item;
          ue_params_item.up_tnl_info   = e1ap_up_tnl_info_to_cu_cp_gtp_tunnel(ul_up_transport_param.up_tnl_info);
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
                e1ap_up_tnl_info_to_cu_cp_gtp_tunnel(
                    e1ap_drb_setup_item.drb_data_forwarding_info_resp.ul_data_forwarding);
          }
          if (e1ap_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding_present) {
            drb_setup_item.drb_data_forwarding_info_resp.value().dl_data_forwarding =
                e1ap_up_tnl_info_to_cu_cp_gtp_tunnel(
                    e1ap_drb_setup_item.drb_data_forwarding_info_resp.dl_data_forwarding);
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
              e1ap_up_tnl_info_to_cu_cp_gtp_tunnel(
                  e1ap_res_setup_item.pdu_session_data_forwarding_info_resp.ul_data_forwarding);
        }
        if (e1ap_res_setup_item.pdu_session_data_forwarding_info_resp.dl_data_forwarding_present) {
          res_setup_item.pdu_session_data_forwarding_info_resp.value().dl_data_forwarding =
              e1ap_up_tnl_info_to_cu_cp_gtp_tunnel(
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

inline void fill_f1ap_ue_context_modification_request(f1ap_ue_context_modification_request_message&         f1c_request,
                                                      const rrc_ue_ue_context_modification_request_message& msg)
{
  // drb to be setup mod list
  f1c_request.msg->drbs_to_be_setup_mod_list_present = true;

  for (auto drb_to_be_setup : msg.rrc_ue_drb_setup_msgs) {
    asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_mod_item_ies_o> f1ap_setup_item;
    auto& f1ap_drb_to_setup_item = f1ap_setup_item->drbs_to_be_setup_mod_item();

    f1ap_drb_to_setup_item.drbid = drb_to_be_setup.drb_id;

    // Add uLUPTNLInformation_ToBeSetup
    for (auto cu_cp_gtp_tunnel_item : drb_to_be_setup.gtp_tunnels) {
      asn1::f1ap::uluptnl_info_to_be_setup_item_s uluptnl_item =
          cu_cp_gtp_tunnel_to_f1ap_uluptnl_info_to_be_setup_item(cu_cp_gtp_tunnel_item);

      f1ap_drb_to_setup_item.uluptnl_info_to_be_setup_list.push_back(uluptnl_item);
    }

    // TODO: Add qos information
    f1ap_drb_to_setup_item.qo_sinfo.set_choice_ext();
    auto& choice_ext = f1ap_drb_to_setup_item.qo_sinfo.choice_ext();

    choice_ext.load_info_obj(ASN1_F1AP_ID_DRB_INFO);
    // auto& drb_info = choice_ext.value().drb_info();

    // TODO: Add rlc mode

    // TODO: Add optional values (ulcfg, dupl_activation, ie_exts)

    f1c_request.msg->drbs_to_be_setup_mod_list.value.push_back(f1ap_setup_item);
  }

  // Add ue aggregate maximum bit rate
  if (msg.ue_aggregate_maximum_bit_rate_ul.has_value()) {
    f1c_request.msg->gnb_du_ue_ambr_ul_present = true;
    f1c_request.msg->gnb_du_ue_ambr_ul.value   = msg.ue_aggregate_maximum_bit_rate_ul.value();
  }
}

inline void fill_rrc_ue_ue_context_modification_response_message(
    rrc_ue_ue_context_modification_response_message&     res,
    const f1ap_ue_context_modification_response_message& f1ap_ue_context_mod_resp_msg)
{
  if (f1ap_ue_context_mod_resp_msg.success) {
    auto& f1ap_response_item = f1ap_ue_context_mod_resp_msg.response;

    // DUtoCURRCInformation
    if (f1ap_response_item->duto_currc_info_present) {
      // CellGroupConfig
      if (!f1ap_response_item->duto_currc_info->cell_group_cfg.empty()) {
        auto& cell_group_cfg = f1ap_response_item->duto_currc_info->cell_group_cfg;
        res.du_to_cu_rrc_info.cell_group_cfg.resize(cell_group_cfg.size());
        std::copy(cell_group_cfg.begin(), cell_group_cfg.end(), res.du_to_cu_rrc_info.cell_group_cfg.begin());
      }

      // MeasGapConfig
      if (!f1ap_response_item->duto_currc_info->meas_gap_cfg.empty()) {
        auto& meas_gap_cfg = f1ap_response_item->duto_currc_info->meas_gap_cfg;
        res.du_to_cu_rrc_info.meas_gap_cfg.resize(meas_gap_cfg.size());
        std::copy(meas_gap_cfg.begin(), meas_gap_cfg.end(), res.du_to_cu_rrc_info.meas_gap_cfg.begin());
      }

      // RequestedPMaxFr1
      if (!f1ap_response_item->duto_currc_info->requested_p_max_fr1.empty()) {
        auto& requested_p_max_fr1 = f1ap_response_item->duto_currc_info->requested_p_max_fr1;
        res.du_to_cu_rrc_info.requested_p_max_fr1.resize(requested_p_max_fr1.size());
        std::copy(
            requested_p_max_fr1.begin(), requested_p_max_fr1.end(), res.du_to_cu_rrc_info.requested_p_max_fr1.begin());
      }
    }

    // Add DRBs setup mod list
    if (f1ap_response_item->drbs_setup_mod_list_present) {
      for (auto f1ap_drb_setup_mod_list_item : f1ap_response_item->drbs_setup_mod_list.value) {
        auto& f1ap_drb_mod_item = f1ap_drb_setup_mod_list_item.value().drbs_setup_mod_item();

        rrc_ue_drbs_setup_modified_item drb_setup_mod_item;
        drb_setup_mod_item.drb_id = uint_to_drb_id(f1ap_drb_mod_item.drbid);

        // Add DL UP TNL to be setup list
        for (auto f1ap_dluptnl_info_to_be_setup_item : f1ap_drb_mod_item.dluptnl_info_to_be_setup_list) {
          rrc_ue_dluptnl_info_to_be_setup_item dluptnl_info_to_be_setup_item;
          dluptnl_info_to_be_setup_item.dluptnl_info =
              f1ap_dluptnl_info_to_be_setup_item_to_cu_cp_gtp_tunnel(f1ap_dluptnl_info_to_be_setup_item);
          drb_setup_mod_item.dluptnl_info_to_be_setup_list.push_back(dluptnl_info_to_be_setup_item);
        }

        if (f1ap_drb_mod_item.lcid_present) {
          drb_setup_mod_item.lcid.value() = uint_to_lcid(f1ap_drb_mod_item.lcid);
        }
      }
    }

    // Add DRBs modified list
    if (f1ap_response_item->drbs_modified_list_present) {
      for (auto f1ap_drbs_modified_list_item : f1ap_response_item->drbs_modified_list.value) {
        auto& f1ap_drb_mod_item = f1ap_drbs_modified_list_item.value().drbs_modified_item();

        rrc_ue_drbs_setup_modified_item drb_setup_mod_item;
        drb_setup_mod_item.drb_id = uint_to_drb_id(f1ap_drb_mod_item.drbid);

        // Add DL UP TNL to be setup list
        for (auto f1ap_dluptnl_info_to_be_setup_item : f1ap_drb_mod_item.dluptnl_info_to_be_setup_list) {
          rrc_ue_dluptnl_info_to_be_setup_item dluptnl_info_to_be_setup_item;
          dluptnl_info_to_be_setup_item.dluptnl_info =
              f1ap_dluptnl_info_to_be_setup_item_to_cu_cp_gtp_tunnel(f1ap_dluptnl_info_to_be_setup_item);
          drb_setup_mod_item.dluptnl_info_to_be_setup_list.push_back(dluptnl_info_to_be_setup_item);
        }

        if (f1ap_drb_mod_item.lcid_present) {
          drb_setup_mod_item.lcid.value() = uint_to_lcid(f1ap_drb_mod_item.lcid);
        }
      }
    }

    // Add SRBs failed to be setup mod list
    if (f1ap_response_item->srbs_failed_to_be_setup_mod_list_present) {
      for (auto f1ap_srbs_failed_setup_mod_list_item : f1ap_response_item->srbs_failed_to_be_setup_mod_list.value) {
        auto& f1ap_srb_failed_item = f1ap_srbs_failed_setup_mod_list_item.value().srbs_failed_to_be_setup_mod_item();

        rrc_ue_srbs_failed_to_be_setup_mod_item srb_failed_item;
        srb_failed_item.srb_id = int_to_srb_id(f1ap_srb_failed_item.srbid);
        if (f1ap_srb_failed_item.cause_present) {
          srb_failed_item.cause.value() = f1ap_cause_to_cu_cp_cause(f1ap_srb_failed_item.cause);
        }
        res.srbs_failed_to_be_setup_mod_list.push_back(srb_failed_item);
      }
    }

    // Add DRBs failed to be setup mod list
    if (f1ap_response_item->drbs_failed_to_be_setup_mod_list_present) {
      for (auto f1ap_drbs_failed_setup_mod_list_item : f1ap_response_item->drbs_failed_to_be_setup_mod_list.value) {
        auto& f1ap_drb_failed_item = f1ap_drbs_failed_setup_mod_list_item.value().drbs_failed_to_be_setup_mod_item();

        rrc_ue_drbs_failed_to_be_setup_modified_item drb_failed_item;
        drb_failed_item.drb_id = uint_to_drb_id(f1ap_drb_failed_item.drbid);
        if (f1ap_drb_failed_item.cause_present) {
          drb_failed_item.cause.value() = f1ap_cause_to_cu_cp_cause(f1ap_drb_failed_item.cause);
        }
        res.drbs_failed_to_be_setup_mod_list.push_back(drb_failed_item);
      }
    }

    // Add SCell failed to be setup mod list
    if (f1ap_response_item->scell_failedto_setup_mod_list_present) {
      for (auto f1ap_scell_failed_setup_mod_list_item : f1ap_response_item->scell_failedto_setup_mod_list.value) {
        auto& f1ap_scell_failed_item = f1ap_scell_failed_setup_mod_list_item.value().scell_failedto_setup_mod_item();

        rrc_ue_scell_failed_to_setup_mod_item scell_failed_item;
        scell_failed_item.scell_id = f1ap_nrcgi_to_nr_cell_identity(f1ap_scell_failed_item.scell_id);
        if (f1ap_scell_failed_item.cause_present) {
          scell_failed_item.cause.value() = f1ap_cause_to_cu_cp_cause(f1ap_scell_failed_item.cause);
        }
        res.scell_failed_to_setup_mod_list.push_back(scell_failed_item);
      }
    }

    // Add DRBs failed to be modified list
    if (f1ap_response_item->drbs_failed_to_be_modified_list_present) {
      for (auto f1ap_drbs_failed_modified_list_item : f1ap_response_item->drbs_failed_to_be_modified_list.value) {
        auto& f1ap_drb_failed_item = f1ap_drbs_failed_modified_list_item.value().drbs_failed_to_be_modified_item();

        rrc_ue_drbs_failed_to_be_setup_modified_item drb_failed_item;
        drb_failed_item.drb_id = uint_to_drb_id(f1ap_drb_failed_item.drbid);
        if (f1ap_drb_failed_item.cause_present) {
          drb_failed_item.cause.value() = f1ap_cause_to_cu_cp_cause(f1ap_drb_failed_item.cause);
        }
        res.drbs_failed_to_be_modified_list.push_back(drb_failed_item);
      }
    }

    // Add inactivity monitoring response
    if (f1ap_response_item->inactivity_monitoring_resp_present) {
      res.inactivity_monitoring_resp.value() = f1ap_response_item->inactivity_monitoring_resp.value.to_string();
    }

    // Add C-RNTI
    if (f1ap_response_item->c_rnti_present) {
      res.c_rnti.value() = to_rnti(f1ap_response_item->c_rnti.value);
    }

    // Add associated SCell list
    if (f1ap_response_item->associated_scell_list_present) {
      for (auto f1ap_associated_scell_list_item : f1ap_response_item->associated_scell_list.value) {
        auto& f1ap_associated_scell_item = f1ap_associated_scell_list_item.value().associated_scell_item();

        rrc_ue_associated_scell_item associated_scell_item;
        associated_scell_item.scell_id = f1ap_nrcgi_to_nr_cell_identity(f1ap_associated_scell_item.scell_id);

        res.associated_scell_list.push_back(associated_scell_item);
      }
    }

    // Add SRBs setup mod list
    if (f1ap_response_item->srbs_setup_mod_list_present) {
      for (auto f1ap_srbs_setup_mod_list_item : f1ap_response_item->srbs_setup_mod_list.value) {
        auto& f1ap_srbs_setup_mod_item = f1ap_srbs_setup_mod_list_item.value().srbs_setup_mod_item();

        rrc_ue_srbs_setup_modified_item srbs_setup_mod_item;
        srbs_setup_mod_item.srb_id = int_to_srb_id(f1ap_srbs_setup_mod_item.srbid);
        srbs_setup_mod_item.lcid   = uint_to_lcid(f1ap_srbs_setup_mod_item.lcid);

        res.srbs_setup_mod_list.push_back(srbs_setup_mod_item);
      }
    }

    // Add SRBs modified list
    if (f1ap_response_item->srbs_modified_list_present) {
      for (auto f1ap_srbs_modified_list_item : f1ap_response_item->srbs_modified_list.value) {
        auto& f1ap_srbs_modified_item = f1ap_srbs_modified_list_item.value().srbs_modified_item();

        rrc_ue_srbs_setup_modified_item srbs_modified_item;
        srbs_modified_item.srb_id = int_to_srb_id(f1ap_srbs_modified_item.srbid);
        srbs_modified_item.lcid   = uint_to_lcid(f1ap_srbs_modified_item.lcid);

        res.srbs_modified_list.push_back(srbs_modified_item);
      }
    }

    // Add full configuration
    if (f1ap_response_item->full_cfg_present) {
      res.full_cfg.value() = f1ap_response_item->full_cfg.value.to_string();
    }

  } else {
    res.success = false;
    res.cause   = f1ap_cause_to_cu_cp_cause(f1ap_ue_context_mod_resp_msg.failure->cause.value);
    if (f1ap_ue_context_mod_resp_msg.failure->crit_diagnostics_present) {
      // TODO: Add crit diagnostics
    }
  }
}

} // namespace srs_cu_cp
} // namespace srsgnb
