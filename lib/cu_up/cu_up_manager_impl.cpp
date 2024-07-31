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

#include "cu_up_manager_impl.h"
#include "srsran/support/async/execute_on.h"

using namespace srsran;
using namespace srs_cu_up;

/// Helper functions
void fill_sec_as_config(security::sec_as_config& sec_as_config, const e1ap_security_info& sec_info);
void process_successful_pdu_resource_modification_outcome(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item>& pdu_session_resource_modified_list,
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item>&
                                           pdu_session_resource_failed_to_modify_list,
    const pdu_session_modification_result& result,
    const srslog::basic_logger&            logger);
void process_successful_pdu_resource_setup_mod_outcome(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>&
                                    pdu_session_resource_setup_list,
    const pdu_session_setup_result& result);

cu_up_manager_impl::cu_up_manager_impl(const cu_up_configuration&    config_,
                                       e1ap_interface&               e1ap,
                                       gtpu_network_gateway_adapter& gtpu_gw_adapter,
                                       gtpu_demux&                   ngu_demux,
                                       gtpu_teid_pool&               n3_teid_allocator,
                                       gtpu_teid_pool&               f1u_teid_allocator) :
  cfg(config_)
{
  /// > Create UE manager
  ue_mng = std::make_unique<ue_manager>(cfg.net_cfg,
                                        cfg.n3_cfg,
                                        cfg.test_mode_cfg,
                                        e1ap,
                                        *cfg.timers,
                                        *cfg.f1u_gateway,
                                        gtpu_gw_adapter,
                                        ngu_demux,
                                        n3_teid_allocator,
                                        f1u_teid_allocator,
                                        *cfg.ue_exec_pool,
                                        *cfg.ctrl_executor,
                                        *cfg.gtpu_pcap,
                                        logger);
}

void cu_up_manager_impl::schedule_ue_async_task(ue_index_t ue_index, async_task<void> task)
{
  ue_mng->schedule_ue_async_task(ue_index, std::move(task));
}

e1ap_bearer_context_setup_response
cu_up_manager_impl::handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg)
{
  e1ap_bearer_context_setup_response response = {};
  response.ue_index                           = INVALID_UE_INDEX;
  response.success                            = false;

  // 1. Create new UE context
  ue_context_cfg ue_cfg = {};
  fill_sec_as_config(ue_cfg.security_info, msg.security_info);
  ue_cfg.activity_level        = msg.activity_notif_level;
  ue_cfg.ue_inactivity_timeout = msg.ue_inactivity_timer;
  ue_cfg.qos                   = cfg.qos;
  ue_context* ue_ctxt          = ue_mng->add_ue(ue_cfg);
  if (ue_ctxt == nullptr) {
    logger.error("Could not create UE context");
    return response;
  }
  ue_ctxt->get_logger().log_info("UE created");

  // 2. Handle bearer context setup request
  for (const auto& pdu_session : msg.pdu_session_res_to_setup_list) {
    pdu_session_setup_result result = ue_ctxt->setup_pdu_session(pdu_session);
    if (result.success) {
      process_successful_pdu_resource_setup_mod_outcome(response.pdu_session_resource_setup_list, result);
    } else {
      e1ap_pdu_session_resource_failed_item res_failed_item;

      res_failed_item.pdu_session_id = result.pdu_session_id;
      res_failed_item.cause          = result.cause;

      response.pdu_session_resource_failed_list.emplace(result.pdu_session_id, res_failed_item);
    }
  }

  // 3. Create response
  response.ue_index = ue_ctxt->get_index();
  response.success  = true;
  return response;
}

async_task<e1ap_bearer_context_modification_response>
cu_up_manager_impl::handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg)
{
  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("Could not find UE context");
    return launch_async([](coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx) {
      CORO_BEGIN(ctx);
      e1ap_bearer_context_modification_response res;
      res.success = false;
      CORO_RETURN(res);
    });
  }
  return execute_and_continue_on_blocking(
      ue_ctxt->ue_exec_mapper->ctrl_executor(), *cfg.ctrl_executor, [this, ue_ctxt, msg]() {
        return handle_bearer_context_modification_request_impl(*ue_ctxt, msg);
      });
}

e1ap_bearer_context_modification_response
cu_up_manager_impl::handle_bearer_context_modification_request_impl(ue_context& ue_ctxt,
                                                                    const e1ap_bearer_context_modification_request& msg)
{
  ue_ctxt.get_logger().log_debug("Handling BearerContextModificationRequest");

  e1ap_bearer_context_modification_response response = {};
  response.ue_index                                  = ue_ctxt.get_index();
  response.success                                   = true;

  bool new_ul_tnl_info_required = msg.new_ul_tnl_info_required == std::string("required");

  if (msg.security_info.has_value()) {
    security::sec_as_config security_info;
    fill_sec_as_config(security_info, msg.security_info.value());
    ue_ctxt.set_security_config(security_info);
  }

  if (msg.ng_ran_bearer_context_mod_request.has_value()) {
    // Traverse list of PDU sessions to be setup/modified
    for (const auto& pdu_session_item :
         msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_setup_mod_list) {
      ue_ctxt.get_logger().log_debug("Setup/Modification of {}", pdu_session_item.pdu_session_id);
      pdu_session_setup_result session_result = ue_ctxt.setup_pdu_session(pdu_session_item);
      process_successful_pdu_resource_setup_mod_outcome(response.pdu_session_resource_setup_list, session_result);
      response.success &= session_result.success; // Update final result.
    }

    // Traverse list of PDU sessions to be modified.
    for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list) {
      ue_ctxt.get_logger().log_debug("Modifying {}", pdu_session_item.pdu_session_id);
      pdu_session_modification_result session_result =
          ue_ctxt.modify_pdu_session(pdu_session_item, new_ul_tnl_info_required);
      process_successful_pdu_resource_modification_outcome(response.pdu_session_resource_modified_list,
                                                           response.pdu_session_resource_failed_to_modify_list,
                                                           session_result,
                                                           logger);
      ue_ctxt.get_logger().log_debug("Modification {}", session_result.success ? "successful" : "failed");

      response.success &= session_result.success; // Update final result.
    }

    // Traverse list of PDU sessions to be removed.
    for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_rem_list) {
      ue_ctxt.get_logger().log_info("Removing {}", pdu_session_item);
      ue_ctxt.remove_pdu_session(pdu_session_item);
      // There is no IE to confirm successful removal.
    }
  } else {
    ue_ctxt.get_logger().log_warning("Ignoring empty Bearer Context Modification Request");
  }

  // 3. Create response
  response.success = true;
  return response;
}

async_task<void>
cu_up_manager_impl::handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg)
{
  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("ue={}: Discarding E1 Bearer Context Release Command. UE context not found", msg.ue_index);
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  ue_ctxt->get_logger().log_debug("Received E1 Bearer Context Release Command");

  return ue_mng->remove_ue(msg.ue_index);
}

async_task<e1ap_bearer_context_modification_response> cu_up_manager_impl::enable_test_mode()
{
  // Convert to common type
  e1ap_bearer_context_setup_request bearer_context_setup = {};

  bearer_context_setup.security_info.security_algorithm.ciphering_algo =
      static_cast<srsran::security::ciphering_algorithm>(cfg.test_mode_cfg.nea_algo);
  bearer_context_setup.security_info.security_algorithm.integrity_protection_algorithm =
      static_cast<srsran::security::integrity_algorithm>(cfg.test_mode_cfg.nia_algo);
  bearer_context_setup.security_info.up_security_key.encryption_key =
      make_byte_buffer("0001020304050607080910111213141516171819202122232425262728293031").value();
  bearer_context_setup.security_info.up_security_key.integrity_protection_key =
      make_byte_buffer("0001020304050607080910111213141516171819202122232425262728293031").value();
  bearer_context_setup.ue_inactivity_timer = std::chrono::seconds(3600);

  /// Setup test PDU session
  pdu_session_id_t                   psi{1};
  e1ap_pdu_session_res_to_setup_item pdu_session          = {};
  pdu_session.pdu_session_id                              = psi;
  pdu_session.ng_ul_up_tnl_info.tp_address                = transport_layer_address::create_from_string("127.0.5.2");
  pdu_session.ng_ul_up_tnl_info.gtp_teid                  = int_to_gtpu_teid(0x02);
  pdu_session.security_ind.integrity_protection_ind       = cfg.test_mode_cfg.integrity_enabled
                                                                ? integrity_protection_indication_t::required
                                                                : integrity_protection_indication_t::not_needed;
  pdu_session.security_ind.confidentiality_protection_ind = cfg.test_mode_cfg.ciphering_enabled
                                                                ? confidentiality_protection_indication_t::required
                                                                : confidentiality_protection_indication_t::not_needed;

  /// Setup test DRB
  e1ap_drb_to_setup_item_ng_ran drb_to_setup = {};
  drb_to_setup.drb_id                        = uint_to_drb_id(1);

  // sdap config
  drb_to_setup.sdap_cfg.default_drb = true;

  // pdcp config
  drb_to_setup.pdcp_cfg.pdcp_sn_size_ul    = pdcp_sn_size::size18bits;
  drb_to_setup.pdcp_cfg.pdcp_sn_size_dl    = pdcp_sn_size::size18bits;
  drb_to_setup.pdcp_cfg.rlc_mod            = pdcp_rlc_mode::um;
  drb_to_setup.pdcp_cfg.discard_timer      = pdcp_discard_timer::infinity;
  drb_to_setup.pdcp_cfg.t_reordering_timer = pdcp_t_reordering::ms200;

  e1ap_qos_flow_qos_param_item qos_item                              = {};
  qos_item.qos_flow_id                                               = uint_to_qos_flow_id(0x01);
  qos_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi = {uint_to_five_qi(9), {}, {}, {}};

  drb_to_setup.qos_flow_info_to_be_setup.emplace(qos_item.qos_flow_id, qos_item);
  pdu_session.drb_to_setup_list_ng_ran.emplace(drb_to_setup.drb_id, drb_to_setup);
  bearer_context_setup.pdu_session_res_to_setup_list.emplace(psi, pdu_session);

  // Setup bearer
  e1ap_bearer_context_setup_response setup_resp = handle_bearer_context_setup_request(bearer_context_setup);

  // Modifiy bearer
  e1ap_bearer_context_modification_request bearer_modify = {};
  bearer_modify.ue_index                                 = setup_resp.ue_index;

  e1ap_ng_ran_bearer_context_mod_request bearer_mod_item = {};

  e1ap_pdu_session_res_to_modify_item pdu_session_to_mod = {};
  pdu_session_to_mod.pdu_session_id = setup_resp.pdu_session_resource_setup_list.begin()->pdu_session_id;

  e1ap_drb_to_modify_item_ng_ran drb_to_mod = {};
  drb_to_mod.dl_up_params.resize(1);
  drb_to_mod.drb_id = setup_resp.pdu_session_resource_setup_list.begin()->drb_setup_list_ng_ran.begin()->drb_id;
  drb_to_mod.dl_up_params[0].up_tnl_info.tp_address = transport_layer_address::create_from_string("127.0.10.2");
  drb_to_mod.dl_up_params[0].up_tnl_info.gtp_teid   = int_to_gtpu_teid(0x02);

  pdu_session_to_mod.drb_to_modify_list_ng_ran.emplace(drb_to_mod.drb_id, drb_to_mod);
  bearer_mod_item.pdu_session_res_to_modify_list.emplace(pdu_session_to_mod.pdu_session_id, pdu_session_to_mod);
  bearer_modify.ng_ran_bearer_context_mod_request = bearer_mod_item;

  return handle_bearer_context_modification_request(bearer_modify);
}

/// Helper functions
void process_successful_pdu_resource_modification_outcome(
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

void process_successful_pdu_resource_setup_mod_outcome(
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

void fill_sec_as_config(security::sec_as_config& sec_as_config, const e1ap_security_info& sec_info)
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
