/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "pdu_session_manager.h"
#include "srsran/e1ap/common/e1ap_types.h"

namespace srsran::srs_cu_up {

inline void process_successful_pdu_resource_modification_outcome(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item>& pdu_session_resource_modified_list,
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item>&
                                           pdu_session_resource_failed_to_modify_list,
    const pdu_session_modification_result& result,
    const srslog::basic_logger&            logger)
{
  if (result.success) {
    e1ap_pdu_session_resource_modified_item modified_item;
    modified_item.pdu_session_id = result.pdu_session_id;

    for (const auto& drb_setup_item : result.drb_setup_results) {
      logger.debug("Adding DRB setup result item. {}, success={}", drb_setup_item.drb_id, drb_setup_item.success);
      if (drb_setup_item.success) {
        e1ap_drb_setup_item_ng_ran res_drb_setup_item;
        res_drb_setup_item.drb_id = drb_setup_item.drb_id;

        e1ap_up_params_item up_param_item;
        up_param_item.up_tnl_info = drb_setup_item.gtp_tunnel;
        res_drb_setup_item.ul_up_transport_params.push_back(up_param_item);

        for (const auto& flow_item : drb_setup_item.qos_flow_results) {
          if (flow_item.success) {
            e1ap_qos_flow_item res_flow_setup_item;
            res_flow_setup_item.qos_flow_id = flow_item.qos_flow_id;
            res_drb_setup_item.flow_setup_list.emplace(flow_item.qos_flow_id, res_flow_setup_item);
          } else {
            e1ap_qos_flow_failed_item res_flow_failed_item;
            res_flow_failed_item.qos_flow_id = flow_item.qos_flow_id;
            res_flow_failed_item.cause       = flow_item.cause;
            res_drb_setup_item.flow_failed_list.emplace(flow_item.qos_flow_id, res_flow_failed_item);
          }
        }
        modified_item.drb_setup_list_ng_ran.emplace(drb_setup_item.drb_id, res_drb_setup_item);
      } else {
        e1ap_drb_failed_item_ng_ran asn1_drb_failed_item;
        asn1_drb_failed_item.drb_id = drb_setup_item.drb_id;
        asn1_drb_failed_item.cause  = drb_setup_item.cause;

        modified_item.drb_failed_list_ng_ran.emplace(drb_setup_item.drb_id, asn1_drb_failed_item);
      }
    }
    for (const auto& drb_modified_item : result.drb_modification_results) {
      logger.debug(
          "Adding DRB modified result item. {}, success={}", drb_modified_item.drb_id, drb_modified_item.success);
      e1ap_drb_modified_item_ng_ran e1ap_mod_item;
      e1ap_mod_item.drb_id = drb_modified_item.drb_id;

      e1ap_up_params_item up_param_item;
      up_param_item.up_tnl_info = drb_modified_item.gtp_tunnel;
      e1ap_mod_item.ul_up_transport_params.push_back(up_param_item);
      modified_item.drb_modified_list_ng_ran.emplace(e1ap_mod_item.drb_id, e1ap_mod_item);
    }

    pdu_session_resource_modified_list.emplace(modified_item.pdu_session_id, modified_item);
  } else {
    e1ap_pdu_session_resource_failed_item failed_item;
    failed_item.pdu_session_id = result.pdu_session_id;
    failed_item.cause          = e1ap_cause_radio_network_t::unspecified;
    pdu_session_resource_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
  }
}

inline void process_successful_pdu_resource_setup_mod_outcome(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>&
                                    pdu_session_resource_setup_list,
    const pdu_session_setup_result& result)
{
  if (result.success) {
    e1ap_pdu_session_resource_setup_modification_item res_setup_item;
    res_setup_item.pdu_session_id    = result.pdu_session_id;
    res_setup_item.ng_dl_up_tnl_info = result.gtp_tunnel;
    res_setup_item.security_result   = result.security_result;
    for (const auto& drb_setup_item : result.drb_setup_results) {
      if (drb_setup_item.success) {
        e1ap_drb_setup_item_ng_ran res_drb_setup_item;
        res_drb_setup_item.drb_id = drb_setup_item.drb_id;

        e1ap_up_params_item up_param_item;
        up_param_item.up_tnl_info = drb_setup_item.gtp_tunnel;
        res_drb_setup_item.ul_up_transport_params.push_back(up_param_item);

        for (const auto& flow_item : drb_setup_item.qos_flow_results) {
          if (flow_item.success) {
            e1ap_qos_flow_item res_flow_setup_item;
            res_flow_setup_item.qos_flow_id = flow_item.qos_flow_id;
            res_drb_setup_item.flow_setup_list.emplace(flow_item.qos_flow_id, res_flow_setup_item);
          } else {
            e1ap_qos_flow_failed_item res_flow_failed_item;
            res_flow_failed_item.qos_flow_id = flow_item.qos_flow_id;
            res_flow_failed_item.cause       = flow_item.cause;
            res_drb_setup_item.flow_failed_list.emplace(flow_item.qos_flow_id, res_flow_failed_item);
          }
        }
        res_setup_item.drb_setup_list_ng_ran.emplace(drb_setup_item.drb_id, res_drb_setup_item);
      } else {
        e1ap_drb_failed_item_ng_ran asn1_drb_failed_item;
        asn1_drb_failed_item.drb_id = drb_setup_item.drb_id;
        asn1_drb_failed_item.cause  = drb_setup_item.cause;

        res_setup_item.drb_failed_list_ng_ran.emplace(drb_setup_item.drb_id, asn1_drb_failed_item);
      }
    }
    pdu_session_resource_setup_list.emplace(result.pdu_session_id, res_setup_item);
  }
}

inline void fill_sec_as_config(security::sec_as_config& sec_as_config, const e1ap_security_info& sec_info)
{
  sec_as_config.domain = security::sec_domain::up;
  if (!sec_info.up_security_key.integrity_protection_key.empty()) {
    sec_as_config.k_int = security::sec_key{};
    std::copy(sec_info.up_security_key.integrity_protection_key.begin(),
              sec_info.up_security_key.integrity_protection_key.end(),
              sec_as_config.k_int.value().begin());
  }
  std::copy(sec_info.up_security_key.encryption_key.begin(),
            sec_info.up_security_key.encryption_key.end(),
            sec_as_config.k_enc.begin());
  sec_as_config.integ_algo  = sec_info.security_algorithm.integrity_protection_algorithm;
  sec_as_config.cipher_algo = sec_info.security_algorithm.ciphering_algo;
}
} // namespace srsran::srs_cu_up
