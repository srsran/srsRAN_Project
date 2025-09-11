/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "ngu_session_manager_impl.h"
#include "routines/initial_cu_up_setup_routine.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "srsran/gtpu/gtpu_demux_factory.h"
#include "srsran/gtpu/gtpu_echo_factory.h"
#include "srsran/gtpu/gtpu_teid_pool_factory.h"
#include "srsran/support/executors/execute_until_success.h"
#include <future>

using namespace srsran;
using namespace srs_cu_up;

static void assert_cu_up_dependencies_valid(const cu_up_dependencies& dependencies)
{
  srsran_assert(dependencies.exec_mapper != nullptr, "Invalid CU-UP UE executor pool");
  srsran_assert(dependencies.e1_conn_client != nullptr, "Invalid E1 connection client");
  srsran_assert(dependencies.f1u_gateway != nullptr, "Invalid F1-U connector");
  srsran_assert(not dependencies.ngu_gws.empty(), "Invalid N3 gateway list");
  for (auto* gw : dependencies.ngu_gws) {
    srsran_assert(gw != nullptr, "Invalid N3 gateway");
  }
  srsran_assert(dependencies.gtpu_pcap != nullptr, "Invalid GTP-U pcap");
}

static cu_up_manager_impl_config generate_cu_up_manager_impl_config(const cu_up_config& config)
{
  return {config.qos, config.n3_cfg, config.test_mode_cfg};
}

static cu_up_manager_impl_dependencies
generate_cu_up_manager_impl_dependencies(const cu_up_dependencies&  dependencies,
                                         e1ap_interface&            e1ap,
                                         gtpu_demux&                ngu_demux,
                                         ngu_session_manager&       ngu_session_mngr,
                                         gtpu_teid_pool&            n3_teid_allocator,
                                         gtpu_teid_pool&            f1u_teid_allocator,
                                         fifo_async_task_scheduler& main_ctrl_loop)
{
  return {e1ap,
          ngu_demux,
          ngu_session_mngr,
          n3_teid_allocator,
          f1u_teid_allocator,
          *dependencies.exec_mapper,
          *dependencies.f1u_gateway,
          *dependencies.timers,
          *dependencies.gtpu_pcap,
          main_ctrl_loop};
}

cu_up::cu_up(const cu_up_config& config_, const cu_up_dependencies& dependencies) :
  cfg(config_),
  ctrl_executor(dependencies.exec_mapper->ctrl_executor()),
  timers(*dependencies.timers),
  main_ctrl_loop(128)
{
  assert_cu_up_dependencies_valid(dependencies);

  /// > Create and connect upper layers

  // Create GTP-U demux
  gtpu_demux_creation_request demux_msg = {};
  demux_msg.cfg.warn_on_drop            = cfg.n3_cfg.warn_on_drop;
  demux_msg.cfg.queue_size              = cfg.n3_cfg.gtpu_queue_size;
  demux_msg.cfg.batch_size              = cfg.n3_cfg.gtpu_batch_size;
  demux_msg.cfg.test_mode               = cfg.test_mode_cfg.enabled;
  demux_msg.gtpu_pcap                   = dependencies.gtpu_pcap;
  ngu_demux                             = create_gtpu_demux(demux_msg);

  echo_exec_mapper = dependencies.exec_mapper->create_ue_executor_mapper();
  report_error_if_not(echo_exec_mapper != nullptr, "Could not create CU-UP executor for control TEID");

  // Create GTP-U echo and register it at demux
  gtpu_echo_creation_message ngu_echo_msg                      = {};
  ngu_echo_msg.gtpu_pcap                                       = dependencies.gtpu_pcap;
  ngu_echo_msg.tx_upper                                        = &gtpu_gw_adapter;
  ngu_echo                                                     = create_gtpu_echo(ngu_echo_msg);
  expected<std::unique_ptr<gtpu_demux_dispatch_queue>> batch_q = ngu_demux->add_tunnel(
      GTPU_PATH_MANAGEMENT_TEID, echo_exec_mapper->dl_pdu_executor(), ngu_echo->get_rx_upper_layer_interface());
  report_error_if_not(batch_q.has_value(), "Could not create GTP-U echo tunnel.");
  echo_batched_queue = std::move(batch_q.value());

  // Connect GTP-U DEMUX to adapter.
  gw_data_gtpu_demux_adapter.connect_gtpu_demux(*ngu_demux);

  // Establish new NG-U session and connect the instantiated session to the GTP-U DEMUX adapter, so that the latter
  // is called when new NG-U DL PDUs are received.
  for (gtpu_gateway* gw : dependencies.ngu_gws) {
    std::unique_ptr<gtpu_tnl_pdu_session> ngu_session = gw->create(gw_data_gtpu_demux_adapter);
    if (ngu_session == nullptr) {
      report_error("Unable to allocate the required NG-U network resources");
    }
    ngu_sessions.push_back(std::move(ngu_session));
  }
  ngu_session_mngr = std::make_unique<ngu_session_manager_impl>(ngu_sessions);

  // Connect GTPU GW adapter to NG-U session in order to send UL PDUs.
  // We use the first UDP GW for UL.
  gtpu_gw_adapter.connect_network_gateway(*ngu_sessions[0]);

  // Create N3 TEID allocator
  gtpu_allocator_creation_request n3_alloc_msg = {};
  n3_alloc_msg.max_nof_teids                   = MAX_NOF_UES * MAX_NOF_PDU_SESSIONS;
  n3_teid_allocator                            = create_gtpu_allocator(n3_alloc_msg);

  // Create F1-U TEID allocator
  gtpu_allocator_creation_request f1u_alloc_msg = {};
  f1u_alloc_msg.max_nof_teids                   = MAX_NOF_UES * MAX_NOF_PDU_SESSIONS;
  f1u_teid_allocator                            = create_gtpu_allocator(f1u_alloc_msg);

  /// > Create e1ap
  e1ap = create_e1ap(cfg.e1ap,
                     *dependencies.e1_conn_client,
                     e1ap_cu_up_mng_adapter,
                     *dependencies.timers,
                     dependencies.exec_mapper->ctrl_executor());

  /// > Create CU-UP manager
  cu_up_mng = std::make_unique<cu_up_manager_impl>(
      generate_cu_up_manager_impl_config(cfg),
      generate_cu_up_manager_impl_dependencies(
          dependencies, *e1ap, *ngu_demux, *ngu_session_mngr, *n3_teid_allocator, *f1u_teid_allocator, main_ctrl_loop));

  /// > Connect E1AP to CU-UP manager
  e1ap_cu_up_mng_adapter.connect_cu_up_manager(*cu_up_mng);
  // Start statistics report timer
  if (cfg.statistics_report_period.count() > 0) {
    statistics_report_timer = dependencies.timers->create_unique_timer(dependencies.exec_mapper->ctrl_executor());
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

  if (not ctrl_executor.execute([this, &p]() {
        main_ctrl_loop.schedule([this, &p](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          // Connect to CU-CP and send E1 Setup Request and await for E1 setup response.
          CORO_AWAIT(launch_async<initial_cu_up_setup_routine>(cfg, *e1ap));

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
  if (not running) {
    return;
  }

  logger.debug("CU-UP stopping...");

  std::condition_variable cvar;

  auto stop_cu_up_main_loop = [this, &cvar]() mutable {
    // Dispatch coroutine to stop CU-UP.
    main_ctrl_loop.schedule(launch_async([this, &cvar](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);

      if (not running) {
        // Already stopped.
        CORO_EARLY_RETURN();
      }

      // Run E1 Release Procedure.
      CORO_AWAIT(e1ap->handle_cu_up_e1ap_release_request());

      // CU-UP stops listening to new GTPU Rx PDUs and stops pushing UL PDUs.
      CORO_AWAIT(handle_stop_command());

      // We defer main ctrl loop stop to let the current coroutine complete successfully.
      defer_until_success(ctrl_executor, timers, [this, &cvar]() {
        // Stop main control loop and communicate back with the caller thread.
        auto main_loop = main_ctrl_loop.request_stop();

        std::lock_guard<std::mutex> lock2(mutex);
        running = false;
        cvar.notify_all();
      });

      CORO_RETURN();
    }));
  };

  // Dispatch task to stop CU-UP main loop.
  defer_until_success(ctrl_executor, timers, stop_cu_up_main_loop);

  // Wait until the all tasks of the main loop are completed and main loop has stopped.
  cvar.wait(lock, [this]() { return not running; });

  // CU-UP stops listening to new GTPU Rx PDUs.
  ngu_sessions.clear();

  logger.info("CU-UP stopped successfully");
}

async_task<void> cu_up::handle_stop_command()
{
  // Stop statistics report timer.
  statistics_report_timer.stop();

  gtpu_gw_adapter.disconnect();

  e1ap_cu_up_mng_adapter.disconnect();
  // Do not disconnect GTP-U Demux as it is being concurrently accessed from the thread pool.
  // It will be safely stopped from inside the CU-UP manager.

  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // Stop dedicated executor for control TEID.
    CORO_AWAIT(echo_exec_mapper->stop());
    echo_exec_mapper = nullptr;

    // Stop CU-UP manager and remove all UEs.
    CORO_AWAIT(cu_up_mng->stop());

    CORO_RETURN();
  });
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
