/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_manager_impl.h"
#include "cu_up_manager_helpers.h"
#include "routines/cu_up_bearer_context_modification_routine.h"
#include "srsran/support/async/execute_on_blocking.h"

using namespace srsran;
using namespace srs_cu_up;

/// Helper functions
static ue_manager_config generate_ue_manager_config(const n3_interface_config&    n3_config,
                                                    const cu_up_test_mode_config& test_mode_config)
{
  return {n3_config, test_mode_config};
}

static ue_manager_dependencies generate_ue_manager_dependencies(const cu_up_manager_impl_dependencies& dependencies,
                                                                srslog::basic_logger&                  logger)
{
  return {dependencies.e1ap,
          dependencies.timers,
          dependencies.f1u_gateway,
          dependencies.ngu_session_mngr,
          dependencies.ngu_demux,
          dependencies.n3_teid_allocator,
          dependencies.f1u_teid_allocator,
          dependencies.exec_mapper,
          dependencies.gtpu_pcap,
          logger};
}

cu_up_manager_impl::cu_up_manager_impl(const cu_up_manager_impl_config&       config,
                                       const cu_up_manager_impl_dependencies& dependencies) :
  qos(config.qos),
  n3_cfg(config.n3_cfg),
  test_mode_cfg(config.test_mode_cfg),
  exec_mapper(dependencies.exec_mapper),
  timers(dependencies.timers)
{
  /// > Create UE manager
  ue_mng = std::make_unique<ue_manager>(generate_ue_manager_config(n3_cfg, test_mode_cfg),
                                        generate_ue_manager_dependencies(dependencies, logger));
}

async_task<void> cu_up_manager_impl::stop()
{
  return ue_mng->stop();
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
  ue_cfg.activity_level                   = msg.activity_notif_level;
  ue_cfg.ue_inactivity_timeout            = msg.ue_inactivity_timer;
  ue_cfg.qos                              = qos;
  ue_cfg.ue_dl_aggregate_maximum_bit_rate = msg.ue_dl_aggregate_maximum_bit_rate;
  ue_context* ue_ctxt                     = ue_mng->add_ue(ue_cfg);
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
  return execute_and_continue_on_blocking(ue_ctxt->ue_exec_mapper->ctrl_executor(),
                                          exec_mapper.ctrl_executor(),
                                          timers,
                                          launch_async<cu_up_bearer_context_modification_routine>(*ue_ctxt, msg));
}

async_task<void>
cu_up_manager_impl::handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg)
{
  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("ue={}: Discarding E1 Bearer Context Release Command. UE context not found",
                 fmt::underlying(msg.ue_index));
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
      static_cast<srsran::security::ciphering_algorithm>(test_mode_cfg.nea_algo);
  bearer_context_setup.security_info.security_algorithm.integrity_protection_algorithm =
      static_cast<srsran::security::integrity_algorithm>(test_mode_cfg.nia_algo);
  bearer_context_setup.security_info.up_security_key.encryption_key =
      make_byte_buffer("0001020304050607080910111213141516171819202122232425262728293031").value();
  bearer_context_setup.security_info.up_security_key.integrity_protection_key =
      make_byte_buffer("0001020304050607080910111213141516171819202122232425262728293031").value();
  bearer_context_setup.ue_inactivity_timer = std::chrono::seconds(3600);

  bearer_context_setup.ue_dl_aggregate_maximum_bit_rate = test_mode_cfg.ue_ambr;

  /// Setup test PDU session
  pdu_session_id_t                   psi{1};
  e1ap_pdu_session_res_to_setup_item pdu_session          = {};
  pdu_session.pdu_session_id                              = psi;
  pdu_session.ng_ul_up_tnl_info.tp_address                = transport_layer_address::create_from_string("127.0.5.2");
  pdu_session.ng_ul_up_tnl_info.gtp_teid                  = int_to_gtpu_teid(0x02);
  pdu_session.security_ind.integrity_protection_ind       = test_mode_cfg.integrity_enabled
                                                                ? integrity_protection_indication_t::required
                                                                : integrity_protection_indication_t::not_needed;
  pdu_session.security_ind.confidentiality_protection_ind = test_mode_cfg.ciphering_enabled
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

  e1ap_qos_flow_qos_param_item qos_item       = {};
  qos_item.qos_flow_id                        = uint_to_qos_flow_id(0x01);
  qos_item.qos_flow_level_qos_params.qos_desc = non_dyn_5qi_descriptor{uint_to_five_qi(9), {}, {}, {}};

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
