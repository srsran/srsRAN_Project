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

#include "../converters/f1ap_asn1_converters.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/rrc/rrc_ue.h"

namespace srsgnb {
namespace srs_cu_cp {

inline void fill_f1ap_ue_context_modification_request(f1ap_ue_context_modification_request&                 f1c_request,
                                                      const rrc_ue_ue_context_modification_request_message& msg)
{
  f1c_request.ue_index = msg.ue_index;

  // drb to be setup mod list
  f1c_request.msg->drbs_to_be_setup_mod_list_present = msg.rrc_ue_drb_setup_msgs.size() > 0;
  for (const auto& drb_to_be_setup : msg.rrc_ue_drb_setup_msgs) {
    asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_mod_item_ies_o> f1ap_setup_item;
    auto& f1ap_drb_to_setup_item = f1ap_setup_item->drbs_to_be_setup_mod_item();

    f1ap_drb_to_setup_item.drbid = drb_to_be_setup.drb_id;
    switch (drb_to_be_setup.rlc) {
      case rlc_mode::am:
        f1ap_drb_to_setup_item.rlc_mode.value = asn1::f1ap::rlc_mode_opts::rlc_am;
        break;
      case rlc_mode::um_bidir:
        f1ap_drb_to_setup_item.rlc_mode.value = asn1::f1ap::rlc_mode_opts::rlc_um_bidirectional;
        break;
      case rlc_mode::um_unidir_dl:
        f1ap_drb_to_setup_item.rlc_mode.value = asn1::f1ap::rlc_mode_opts::rlc_um_unidirectional_dl;
        break;
      case rlc_mode::um_unidir_ul:
        f1ap_drb_to_setup_item.rlc_mode.value = asn1::f1ap::rlc_mode_opts::rlc_um_unidirectional_ul;
        break;
      case rlc_mode::tm:
        // TM not supported for DRBs
        report_fatal_error("Invalid RLC mode {}", drb_to_be_setup.rlc);
        break;
    }

    // Add uLUPTNLInformation_ToBeSetup
    for (const auto& gtp_tunnel_item : drb_to_be_setup.gtp_tunnels) {
      asn1::f1ap::uluptnl_info_to_be_setup_item_s item;
      up_transport_layer_info_to_asn1(item.uluptnl_info, gtp_tunnel_item);
      f1ap_drb_to_setup_item.uluptnl_info_to_be_setup_list.push_back(item);
    }

    // Add qos information
    f1ap_drb_to_setup_item.qo_sinfo.set_choice_ext();
    auto& choice_ext = f1ap_drb_to_setup_item.qo_sinfo.choice_ext();
    choice_ext.load_info_obj(ASN1_F1AP_ID_DRB_INFO);

    auto& drb_info = choice_ext.value().drb_info();
    drb_info.drb_qos.qo_s_characteristics.set_non_dynamic_minus5_qi();
    drb_info.drb_qos.qo_s_characteristics.non_dynamic_minus5_qi().five_qi = drb_to_be_setup.qos_info.five_qi;
    drb_info.drb_qos.ngra_nalloc_retention_prio.prio_level                = drb_to_be_setup.qos_info.prio_level_arp;
    drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_cap =
        asn1::f1ap::pre_emption_cap_opts::shall_not_trigger_pre_emption;
    drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_vulnerability.value =
        asn1::f1ap::pre_emption_vulnerability_opts::not_pre_emptable;
    drb_info.snssai.sst.from_number(drb_to_be_setup.s_nssai.sst);
    if (drb_to_be_setup.s_nssai.sd.has_value()) {
      drb_info.snssai.sd.from_number(drb_to_be_setup.s_nssai.sd.value());
    }

    for (const auto& qos_flow : drb_to_be_setup.qos_flows_mapped_to_drb) {
      asn1::f1ap::flows_mapped_to_drb_item_s new_item;
      new_item.qo_sflow_id               = qos_flow.qos_flow_id;
      new_item.qo_sflow_level_qos_params = drb_info.drb_qos;
      drb_info.flows_mapped_to_drb_list.push_back(new_item);
    }

    f1c_request.msg->drbs_to_be_setup_mod_list.value.push_back(f1ap_setup_item);
  }

  // Add ue aggregate maximum bit rate
  if (msg.ue_aggregate_maximum_bit_rate_ul.has_value()) {
    f1c_request.msg->gnb_du_ue_ambr_ul_present = true;
    f1c_request.msg->gnb_du_ue_ambr_ul.value   = msg.ue_aggregate_maximum_bit_rate_ul.value();
  }
}

inline void fill_rrc_ue_ue_context_modification_response_message(
    rrc_ue_ue_context_modification_response_message& res,
    const f1ap_ue_context_modification_response&     f1ap_ue_context_mod_resp_msg)
{
  if (f1ap_ue_context_mod_resp_msg.success) {
    res.success = true;

    auto& f1ap_response_item = f1ap_ue_context_mod_resp_msg.response;

    // DUtoCURRCInformation
    if (f1ap_response_item->duto_currc_info_present) {
      res.du_to_cu_rrc_info.cell_group_cfg      = f1ap_response_item->duto_currc_info->cell_group_cfg.copy();
      res.du_to_cu_rrc_info.meas_gap_cfg        = f1ap_response_item->duto_currc_info->meas_gap_cfg.copy();
      res.du_to_cu_rrc_info.requested_p_max_fr1 = f1ap_response_item->duto_currc_info->requested_p_max_fr1.copy();
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
              asn1_to_up_transport_layer_info(f1ap_dluptnl_info_to_be_setup_item.dluptnl_info);
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
              asn1_to_up_transport_layer_info(f1ap_dluptnl_info_to_be_setup_item.dluptnl_info);
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
