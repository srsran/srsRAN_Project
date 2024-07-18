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
#include "cu_up_manager_impl.h"
#include "routines/initial_cu_up_setup_routine.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "srsran/gtpu/gtpu_demux_factory.h"
#include "srsran/gtpu/gtpu_echo_factory.h"
#include "srsran/gtpu/gtpu_teid_pool_factory.h"
#include <future>

using namespace srsran;
using namespace srs_cu_up;

void assert_cu_up_configuration_valid(const cu_up_configuration& cfg)
{
  srsran_assert(cfg.ue_exec_pool != nullptr, "Invalid CU-UP UE executor pool");
  srsran_assert(cfg.io_ul_executor != nullptr, "Invalid CU-UP IO UL executor");
  srsran_assert(cfg.e1ap.e1_conn_client != nullptr, "Invalid E1 connection client");
  srsran_assert(cfg.f1u_gateway != nullptr, "Invalid F1-U connector");
  srsran_assert(cfg.ngu_gw != nullptr, "Invalid N3 gateway");
  srsran_assert(cfg.gtpu_pcap != nullptr, "Invalid GTP-U pcap");
}

void fill_sec_as_config(security::sec_as_config& sec_as_config, const e1ap_security_info& sec_info);

cu_up::cu_up(const cu_up_configuration& config_) : cfg(config_), main_ctrl_loop(128)
{
  assert_cu_up_configuration_valid(cfg);

  /// > Create and connect upper layers

  // Create GTP-U demux
  gtpu_demux_creation_request demux_msg = {};
  demux_msg.cfg.warn_on_drop            = cfg.n3_cfg.warn_on_drop;
  demux_msg.cfg.test_mode               = cfg.test_mode_cfg.enabled;
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

  // Connect GTP-U DEMUX to adapter.
  gw_data_gtpu_demux_adapter.connect_gtpu_demux(*ngu_demux);

  // Establish new NG-U session and connect the instantiated session to the GTP-U DEMUX adapter, so that the latter
  // is called when new NG-U DL PDUs are received.
  ngu_session = cfg.ngu_gw->create(gw_data_gtpu_demux_adapter);
  if (ngu_session == nullptr) {
    report_error("Unable to allocate the required NG-U network resources");
  }

  // Connect GTPU GW adapter to NG-U session in order to send UL PDUs.
  gtpu_gw_adapter.connect_network_gateway(*ngu_session);

  // Create N3 TEID allocator
  gtpu_allocator_creation_request n3_alloc_msg = {};
  n3_alloc_msg.max_nof_teids                   = MAX_NOF_UES * MAX_NOF_PDU_SESSIONS;
  n3_teid_allocator                            = create_gtpu_allocator(n3_alloc_msg);

  // Create F1-U TEID allocator
  gtpu_allocator_creation_request f1u_alloc_msg = {};
  f1u_alloc_msg.max_nof_teids                   = MAX_NOF_UES * MAX_NOF_PDU_SESSIONS;
  f1u_teid_allocator                            = create_gtpu_allocator(f1u_alloc_msg);

  /// > Create e1ap
  e1ap = create_e1ap(*cfg.e1ap.e1_conn_client, e1ap_cu_up_mng_adapter, *cfg.timers, *cfg.ctrl_executor);

  cfg.e1ap.e1ap_conn_mng = e1ap.get();

  /// > Create CU-UP manager
  cu_up_mng = std::make_unique<cu_up_manager_impl>(
      cfg, *e1ap, gtpu_gw_adapter, *ngu_demux, *n3_teid_allocator, *f1u_teid_allocator);

  /// > Connect E1AP to CU-UP manager
  e1ap_cu_up_mng_adapter.connect_cu_up_manager(*cu_up_mng);

  // Start statistics report timer
  if (cfg.statistics_report_period.count() > 0) {
    statistics_report_timer = cfg.timers->create_unique_timer(*cfg.ctrl_executor);
    statistics_report_timer.set(cfg.statistics_report_period,
                                [this](timer_id_t /*tid*/) { on_statistics_report_timer_expired(); });
    statistics_report_timer.run();
  }
}

cu_up::~cu_up()
{
  stop();
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

          if (cfg.test_mode_cfg.enabled) {
            logger.info("enabling test mode...");
            CORO_AWAIT(cu_up_mng->enable_test_mode());
            logger.info("test mode enabled");
          }

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

  eager_async_task<void> main_loop;
  std::atomic<bool>      main_loop_stopped{false};

  auto stop_cu_up_main_loop = [this, &main_loop, &main_loop_stopped]() mutable {
    if (main_loop.empty()) {
      // First call. Initiate shutdown operations.

      // Stop statistics report timer.
      statistics_report_timer.stop();

      // CU-UP stops listening to new GTPU Rx PDUs and stops pushing UL PDUs.
      disconnect();

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

  // CU-UP stops listening to new GTPU Rx PDUs.
  ngu_session.reset();

  logger.info("CU-UP stopped successfully");
}

void cu_up::disconnect()
{
  gw_data_gtpu_demux_adapter.disconnect();
  gtpu_gw_adapter.disconnect();
  e1ap_cu_up_mng_adapter.disconnect();
}

void cu_up::on_statistics_report_timer_expired()
{
  // Log statistics
  logger.debug("num_e1ap_ues={} num_cu_up_ues={}", e1ap->get_nof_ues(), cu_up_mng->get_nof_ues());

  // Restart timer
  statistics_report_timer.set(cfg.statistics_report_period,
                              [this](timer_id_t /*tid*/) { on_statistics_report_timer_expired(); });
  statistics_report_timer.run();
}
