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

#include "cu_up_impl.h"
#include "routines/initial_cu_up_setup_routine.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/gtpu/gtpu_demux_factory.h"
#include "srsran/gtpu/gtpu_echo_factory.h"
#include "srsran/gtpu/gtpu_teid_pool_factory.h"
#include "srsran/support/io/io_broker.h"
#include <condition_variable>
#include <future>

using namespace srsran;
using namespace srs_cu_up;

void assert_cu_up_configuration_valid(const cu_up_configuration& cfg)
{
  srsran_assert(cfg.ue_exec_pool != nullptr, "Invalid CU-UP UE executor pool");
  srsran_assert(cfg.io_ul_executor != nullptr, "Invalid CU-UP IO UL executor");
  srsran_assert(cfg.e1ap.e1ap_conn_client != nullptr, "Invalid E1AP connection client");
  srsran_assert(cfg.f1u_gateway != nullptr, "Invalid F1-U connector");
  srsran_assert(cfg.epoll_broker != nullptr, "Invalid IO broker");
  srsran_assert(cfg.gtpu_pcap != nullptr, "Invalid GTP-U pcap");
}

void fill_sec_as_config(security::sec_as_config& sec_as_config, const e1ap_security_info& sec_info);

cu_up::cu_up(const cu_up_configuration& config_) : cfg(config_), main_ctrl_loop(128)
{
  assert_cu_up_configuration_valid(cfg);

  /// > Create and connect upper layers

  // Create NG-U gateway, bind/open later (after GTP-U demux is connected)
  udp_network_gateway_config ngu_gw_config = {};
  ngu_gw_config.bind_address               = cfg.net_cfg.n3_bind_addr;
  ngu_gw_config.bind_port                  = cfg.net_cfg.n3_bind_port;
  ngu_gw_config.bind_interface             = cfg.net_cfg.n3_bind_interface;
  ngu_gw_config.rx_max_mmsg                = cfg.net_cfg.n3_rx_max_mmsg;
  // other params
  udp_network_gateway_creation_message ngu_gw_msg = {ngu_gw_config, gw_data_gtpu_demux_adapter, *cfg.io_ul_executor};
  ngu_gw                                          = create_udp_network_gateway(ngu_gw_msg);

  // Create GTP-U demux
  gtpu_demux_creation_request demux_msg = {};
  demux_msg.cfg.warn_on_drop            = cfg.n3_cfg.warn_on_drop;
  demux_msg.gtpu_pcap                   = cfg.gtpu_pcap;
  ngu_demux                             = create_gtpu_demux(demux_msg);

  ctrl_exec_mapper = cfg.ue_exec_pool->create_ue_executor_mapper();
  report_error_if_not(ctrl_exec_mapper != nullptr, "Could not create CU-UP executor for control TEID");

  // Create GTP-U echo and register it at demux
  gtpu_echo_creation_message ngu_echo_msg = {};
  ngu_echo_msg.gtpu_pcap                  = cfg.gtpu_pcap;
  ngu_echo_msg.tx_upper                   = &gtpu_gw_adapter;
  ngu_echo                                = create_gtpu_echo(ngu_echo_msg);
  ngu_demux->add_tunnel(
      GTPU_PATH_MANAGEMENT_TEID, ctrl_exec_mapper->dl_pdu_executor(), ngu_echo->get_rx_upper_layer_interface());

  // Connect layers
  gw_data_gtpu_demux_adapter.connect_gtpu_demux(*ngu_demux);
  gtpu_gw_adapter.connect_network_gateway(*ngu_gw);

  // Bind/open the gateway, start handling of incoming traffic from UPF, e.g. echo
  if (not ngu_gw->create_and_bind()) {
    logger.error("Failed to create and connect N3 gateway");
  }
  bool success = cfg.epoll_broker->register_fd(ngu_gw->get_socket_fd(), [this](int fd) { ngu_gw->receive(); });
  if (!success) {
    logger.error("Failed to register N3 (GTP-U) network gateway at IO broker. socket_fd={}", ngu_gw->get_socket_fd());
  }

  // Create TEID allocator
  gtpu_allocator_creation_request f1u_alloc_msg = {};
  f1u_alloc_msg.max_nof_teids                   = MAX_NOF_UES * MAX_NOF_PDU_SESSIONS;
  f1u_teid_allocator                            = create_gtpu_allocator(f1u_alloc_msg);

  /// > Create e1ap
  e1ap = create_e1ap(*cfg.e1ap.e1ap_conn_client, e1ap_cu_up_ev_notifier, *cfg.timers, *cfg.ctrl_executor);
  e1ap_cu_up_ev_notifier.connect_cu_up(*this);

  cfg.e1ap.e1ap_conn_mng = e1ap.get();

  /// > Create UE manager
  ue_mng = std::make_unique<ue_manager>(cfg.net_cfg,
                                        cfg.n3_cfg,
                                        *e1ap,
                                        *cfg.timers,
                                        *cfg.f1u_gateway,
                                        gtpu_gw_adapter,
                                        *ngu_demux,
                                        *f1u_teid_allocator,
                                        *cfg.ue_exec_pool,
                                        *cfg.gtpu_pcap,
                                        logger);

  // Start statistics report timer
  if (cfg.statistics_report_period.count() > 0) {
    statistics_report_timer = cfg.timers->create_unique_timer(*cfg.ctrl_executor);
    statistics_report_timer.set(cfg.statistics_report_period,
                                [this](timer_id_t /*tid*/) { on_statistics_report_timer_expired(); });
    statistics_report_timer.run();
  }
}

void cu_up::start()
{
  logger.info("CU-UP starting...");

  std::unique_lock<std::mutex> lock(mutex);
  if (std::exchange(running, true)) {
    logger.warning("CU-UP already started. Ignoring start request");
    return;
  }

  std::promise<void> p;
  std::future<void>  fut = p.get_future();

  if (not cfg.ctrl_executor->execute([this, &p]() {
        main_ctrl_loop.schedule([this, &p](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          // Connect to CU-CP and send E1 Setup Request and await for E1 setup response.
          CORO_AWAIT(launch_async<initial_cu_up_setup_routine>(cfg));

          // Signal start() caller thread that the operation is complete.
          p.set_value();

          CORO_RETURN();
        });
      })) {
    report_fatal_error("Unable to initiate CU-UP setup routine");
  }

  // Block waiting for CU-UP setup to complete.
  fut.wait();

  logger.info("CU-UP started successfully");
}

void cu_up::stop()
{
  std::unique_lock<std::mutex> lock(mutex);
  if (not std::exchange(running, false)) {
    return;
  }
  logger.debug("CU-UP stopping...");

  // CU-UP stops listening to new GTPU Rx PDUs.
  if (ngu_gw) {
    if (not cfg.epoll_broker->unregister_fd(ngu_gw->get_socket_fd())) {
      logger.warning("Failed to stop NG-U gateway socket");
    }
  }

  eager_async_task<void> main_loop;
  std::atomic<bool>      main_loop_stopped{false};

  auto stop_cu_up_main_loop = [this, &main_loop, &main_loop_stopped]() mutable {
    if (main_loop.empty()) {
      // First call. Initiate shutdown operations.

      // Stop main control loop and communicate back with the caller thread.
      main_loop = main_ctrl_loop.request_stop();
    }

    if (main_loop.ready()) {
      // If the main loop finished, return back to the caller.
      main_loop_stopped = true;
    }
  };

  // Wait until the all tasks of the main loop are completed and main loop has stopped.
  while (not main_loop_stopped) {
    if (not cfg.ctrl_executor->execute(stop_cu_up_main_loop)) {
      logger.error("Unable to stop CU-UP");
      return;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  logger.info("CU-UP stopped successfully");
}

cu_up::~cu_up()
{
  stop();
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

e1ap_bearer_context_setup_response
cu_up::handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg)
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

  response.success = true;

  return response;
}

e1ap_bearer_context_modification_response
cu_up::handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg)
{
  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("Could not find UE context");
    return {};
  }

  ue_ctxt->get_logger().log_debug("Handling BearerContextModificationRequest");

  e1ap_bearer_context_modification_response response = {};
  response.ue_index                                  = ue_ctxt->get_index();
  response.success                                   = true;

  bool new_ul_tnl_info_required = msg.new_ul_tnl_info_required == std::string("required");

  if (msg.ng_ran_bearer_context_mod_request.has_value()) {
    // Traverse list of PDU sessions to be setup/modified
    for (const auto& pdu_session_item :
         msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_setup_mod_list) {
      ue_ctxt->get_logger().log_debug("Setup/Modification of {}", pdu_session_item.pdu_session_id);
      pdu_session_setup_result session_result = ue_ctxt->setup_pdu_session(pdu_session_item);
      process_successful_pdu_resource_setup_mod_outcome(response.pdu_session_resource_setup_list, session_result);
      response.success &= session_result.success; // Update final result.
    }

    // Traverse list of PDU sessions to be modified.
    for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list) {
      ue_ctxt->get_logger().log_debug("Modifying {}", pdu_session_item.pdu_session_id);
      pdu_session_modification_result session_result =
          ue_ctxt->modify_pdu_session(pdu_session_item, new_ul_tnl_info_required);
      process_successful_pdu_resource_modification_outcome(response.pdu_session_resource_modified_list,
                                                           response.pdu_session_resource_failed_to_modify_list,
                                                           session_result,
                                                           logger);
      ue_ctxt->get_logger().log_debug("Modification {}", session_result.success ? "successful" : "failed");

      response.success &= session_result.success; // Update final result.
    }

    // Traverse list of PDU sessions to be removed.
    for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_rem_list) {
      ue_ctxt->get_logger().log_info("Removing {}", pdu_session_item);
      ue_ctxt->remove_pdu_session(pdu_session_item);
      // There is no IE to confirm successful removal.
    }
  } else {
    ue_ctxt->get_logger().log_warning("Ignoring empty Bearer Context Modification Request");
  }

  // 3. Create response

  response.success = true;

  return response;
}

void cu_up::handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg)
{
  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("ue={}: Discarding E1 Bearer Context Release Command. UE context not found", msg.ue_index);
    return;
  }

  ue_ctxt->get_logger().log_debug("Received E1 Bearer Context Release Command");

  ue_mng->remove_ue(msg.ue_index);
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

void cu_up::on_e1ap_connection_establish()
{
  e1ap_connected = true;
}

void cu_up::on_e1ap_connection_drop()
{
  e1ap_connected = false;
}

void cu_up::on_statistics_report_timer_expired()
{
  // Log statistics
  logger.debug("num_e1ap_ues={} num_cu_up_ues={}", e1ap->get_nof_ues(), ue_mng->get_nof_ues());

  // Restart timer
  statistics_report_timer.set(cfg.statistics_report_period,
                              [this](timer_id_t /*tid*/) { on_statistics_report_timer_expired(); });
  statistics_report_timer.run();
}
