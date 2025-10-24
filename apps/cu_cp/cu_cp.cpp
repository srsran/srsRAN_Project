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

#include "apps/cu_cp/cu_cp_appconfig_cli11_schema.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/services/app_execution_metrics/executor_metrics_manager.h"
#include "apps/services/app_resource_usage/app_resource_usage.h"
#include "apps/services/application_message_banners.h"
#include "apps/services/application_tracer.h"
#include "apps/services/buffer_pool/buffer_pool_manager.h"
#include "apps/services/cmdline/cmdline_command_dispatcher.h"
#include "apps/services/metrics/metrics_manager.h"
#include "apps/services/metrics/metrics_notifier_proxy.h"
#include "apps/services/remote_control/remote_server.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "apps/units/o_cu_cp/o_cu_cp_application_unit.h"
#include "apps/units/o_cu_cp/o_cu_cp_unit_config.h"
#include "apps/units/o_cu_cp/pcap_factory.h"
#include "cu_cp_appconfig.h"
#include "srsran/adt/scope_exit.h"
#include "srsran/cu_cp/cu_cp_operation_controller.h"
#include "srsran/e1ap/gateways/e1_network_server_factory.h"
#include "srsran/e2/e2ap_config_translators.h"
#include "srsran/f1ap/gateways/f1c_network_server_factory.h"
#include "srsran/support/backtrace.h"
#include "srsran/support/config_parsers.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/io/io_broker.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/io/io_timer_source.h"
#include "srsran/support/signal_handling.h"
#include "srsran/support/signal_observer.h"
#include "srsran/support/sysinfo.h"
#include "srsran/support/timers.h"
#include "srsran/support/tracing/event_tracing.h"
#include "srsran/support/versioning/build_info.h"
#include "srsran/support/versioning/version.h"
#include <atomic>
#include <thread>

using namespace srsran;

/// \file
/// \brief Application of a Central Unit Control Plane (CU-CP).
///
/// This application runs a CU-CP with the N2, F1-C and E1 interfaces over the standard
/// SCTP ports.
///
/// The app serves as an example for an stand-alone CU-CP.

static std::string config_file;

/// Flag that indicates if the application is running or being shutdown.
static std::atomic<bool> is_app_running = {true};
/// Maximum number of configuration files allowed to be concatenated in the command line.
static constexpr unsigned MAX_CONFIG_FILES = 10;

static void populate_cli11_generic_args(CLI::App& app)
{
  fmt::memory_buffer buffer;
  format_to(std::back_inserter(buffer), "srsRAN 5G CU-CP version {} ({})", get_version(), get_build_hash());
  app.set_version_flag("-v,--version", srsran::to_c_str(buffer));
  app.set_config("-c,", config_file, "Read config from file", false)->expected(1, MAX_CONFIG_FILES);
}

/// Function to call when the application is interrupted.
static void interrupt_signal_handler(int signal)
{
  is_app_running = false;
}

static signal_dispatcher cleanup_signal_dispatcher;

/// Function to call when the application is going to be forcefully shutdown.
static void cleanup_signal_handler(int signal)
{
  cleanup_signal_dispatcher.notify_signal(signal);
  srslog::fetch_basic_logger("APP").error("Emergency flush of the logger");
  srslog::flush();
}

/// Function to call when an error is reported by the application.
static void app_error_report_handler()
{
  srslog::fetch_basic_logger("APP").error("Emergency flush of the logger");
  srslog::flush();
}

static void initialize_log(const std::string& filename)
{
  srslog::sink* log_sink = (filename == "stdout") ? srslog::create_stdout_sink() : srslog::create_file_sink(filename);
  if (log_sink == nullptr) {
    report_error("Could not create application main log sink.\n");
  }
  srslog::set_default_sink(*log_sink);
  srslog::init();
}

static void register_app_logs(const cu_cp_appconfig& cu_cp_cfg, o_cu_cp_application_unit& cu_cp_app_unit)
{
  const logger_appconfig& log_cfg = cu_cp_cfg.log_cfg;
  // Set log-level of app and all non-layer specific components to app level.
  for (const auto& id : {"ALL", "SCTP-GW", "IO-EPOLL", "PCAP", "ASN1"}) {
    auto& logger = srslog::fetch_basic_logger(id, false);
    logger.set_level(log_cfg.lib_level);
    logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& app_logger = srslog::fetch_basic_logger("CU-CP", false);
  app_logger.set_level(srslog::basic_levels::info);
  app_services::application_message_banners::log_build_info(app_logger);
  app_logger.set_level(log_cfg.all_level);
  app_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  {
    auto& logger = srslog::fetch_basic_logger("APP", false);
    logger.set_level(log_cfg.all_level);
    logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& config_logger = srslog::fetch_basic_logger("CONFIG", false);
  config_logger.set_level(log_cfg.config_level);
  config_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  // Metrics log channels.
  const app_helpers::metrics_config& metrics_cfg = cu_cp_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg;
  app_helpers::initialize_metrics_log_channels(metrics_cfg, log_cfg.hex_max_size);
  if (metrics_cfg.enable_json_metrics) {
    app_services::initialize_json_channel();
  }

  // Register units logs.
  cu_cp_app_unit.on_loggers_registration();
}

static void fill_cu_cp_worker_manager_config(worker_manager_config& config, const cu_cp_appconfig& app_cfg)
{
  config.nof_main_pool_threads     = app_cfg.expert_execution_cfg.threads.main_pool.nof_threads;
  config.main_pool_task_queue_size = app_cfg.expert_execution_cfg.threads.main_pool.task_queue_size;
  config.main_pool_backoff_period =
      std::chrono::microseconds{app_cfg.expert_execution_cfg.threads.main_pool.backoff_period};
  config.main_pool_affinity_cfg = app_cfg.expert_execution_cfg.affinities.main_pool_cpu_cfg;
}

int main(int argc, char** argv)
{
  // Set the application error handler.
  set_error_handler(app_error_report_handler);

  static constexpr std::string_view app_name = "CU-CP";
  app_services::application_message_banners::announce_app_and_version(app_name);

  // Set interrupt and cleanup signal handlers.
  register_interrupt_signal_handler(interrupt_signal_handler);
  register_cleanup_signal_handler(cleanup_signal_handler);

  // Enable backtrace.
  enable_backtrace();

  // Setup and configure config parsing.
  CLI::App app("srsCU-CP application");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  // Fill the generic application arguments to parse.
  populate_cli11_generic_args(app);

  // Configure CLI11 with the CU application configuration schema.
  cu_cp_appconfig cu_cp_cfg;
  configure_cli11_with_cu_cp_appconfig_schema(app, cu_cp_cfg);

  auto o_cu_cp_app_unit = create_o_cu_cp_application_unit("cucp");
  o_cu_cp_app_unit->on_parsing_configuration_registration(app);

  // Set the callback for the app calling all the autoderivation functions.
  app.callback([&app, &o_cu_cp_app_unit]() { o_cu_cp_app_unit->on_configuration_parameters_autoderivation(app); });

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  // Dry run mode, exit.
  if (cu_cp_cfg.enable_dryrun) {
    return 0;
  }

  if (cu_cp_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg.enable_json_metrics &&
      !cu_cp_cfg.remote_control_config.enabled) {
    fmt::println("NOTE: No JSON metrics will be generated as the remote server is disabled");
  }

  // TODO: validate appconfig

  // Set up logging.
  initialize_log(cu_cp_cfg.log_cfg.filename);
  auto log_flusher = make_scope_exit([]() { srslog::flush(); });
  register_app_logs(cu_cp_cfg, *o_cu_cp_app_unit);

  // Check the metrics and metrics consumers.
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");
  bool                  metrics_enabled =
      o_cu_cp_app_unit->are_metrics_enabled() || cu_cp_cfg.metrics_cfg.rusage_config.enable_app_usage;

  if (!metrics_enabled && cu_cp_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg.enabled()) {
    cu_cp_logger.warning(
        "Logger or JSON metrics output enabled but no metrics will be reported as no layer was enabled");
    fmt::println("Logger or JSON metrics output enabled but no metrics will be reported as no layer was enabled");
  }

  // TODO: Log input configuration.

  app_services::application_tracer app_tracer;
  if (not cu_cp_cfg.trace_cfg.filename.empty()) {
    app_tracer.enable_tracer(cu_cp_cfg.trace_cfg.filename,
                             cu_cp_cfg.trace_cfg.max_tracing_events_per_file,
                             cu_cp_cfg.trace_cfg.nof_tracing_events_after_severe,
                             cu_cp_logger);
  }

  // configure cgroups
  // TODO

  // Setup size of byte buffer pool.
  app_services::buffer_pool_manager buffer_pool_service(cu_cp_cfg.buffer_pool_config);

  // Log CPU architecture.
  // TODO

  // Check and log included CPU features and check support by current CPU
  if (cpu_supports_included_features()) {
    cu_cp_logger.debug("Required CPU features: {}", get_cpu_feature_info());
  } else {
    // Quit here until we complete selection of the best matching implementation for the current CPU at runtime.
    cu_cp_logger.error(
        "The CPU does not support the required CPU features that were configured during compile time: {}",
        get_cpu_feature_info());
    report_error("The CPU does not support the required CPU features that were configured during compile time: {}\n",
                 get_cpu_feature_info());
  }

  // Check some common causes of performance issues and print a warning if required.
  check_cpu_governor(cu_cp_logger);
  check_drm_kms_polling(cu_cp_logger);

  // Create manager of timers for CU-CP and CU-UP, which will be driven by the system timer slot ticks.
  timer_manager  app_timers{256};
  timer_manager* cu_timers = &app_timers;

  app_services::metrics_notifier_proxy_impl metrics_notifier_forwarder;

  // Instantiate executor metrics service.
  app_services::executor_metrics_service_and_metrics exec_metrics_service = build_executor_metrics_service(
      metrics_notifier_forwarder, app_timers, cu_cp_cfg.metrics_cfg.executors_metrics_cfg);
  std::vector<app_services::metrics_config> metrics_configs = std::move(exec_metrics_service.metrics);

  // Create worker manager.
  worker_manager_config worker_manager_cfg;
  fill_cu_cp_worker_manager_config(worker_manager_cfg, cu_cp_cfg);
  o_cu_cp_app_unit->fill_worker_manager_config(worker_manager_cfg);
  worker_manager_cfg.app_timers                    = &app_timers;
  worker_manager_cfg.exec_metrics_channel_registry = exec_metrics_service.channel_registry;
  worker_manager workers{worker_manager_cfg};

  // Create IO broker.
  const auto&                main_pool_cpu_mask = cu_cp_cfg.expert_execution_cfg.affinities.main_pool_cpu_cfg.mask;
  io_broker_config           io_broker_cfg(os_thread_realtime_priority::min() + 5, main_pool_cpu_mask);
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll, io_broker_cfg);

  // Create time source that ticks the timers.
  std::optional<io_timer_source> time_source(
      std::in_place_t{}, app_timers, *epoll_broker, workers.get_timer_source_executor(), std::chrono::milliseconds{1});

  // Create layer specific PCAPs.
  auto on_pcap_close = make_scope_exit([&cu_cp_logger]() { cu_cp_logger.info("PCAP files successfully closed."); });
  o_cu_cp_dlt_pcaps cu_cp_dlt_pcaps = create_o_cu_cp_dlt_pcap(
      o_cu_cp_app_unit->get_o_cu_cp_unit_config(), workers.get_cu_cp_pcap_executors(), cleanup_signal_dispatcher);
  auto on_pcap_close_init = make_scope_exit([&cu_cp_logger]() { cu_cp_logger.info("Closing PCAP files..."); });

  // Create F1-C GW (TODO cleanup port and PPID args with factory)
  sctp_network_gateway_config f1c_sctp_cfg = {};
  f1c_sctp_cfg.if_name                     = "F1-C";
  f1c_sctp_cfg.bind_address                = cu_cp_cfg.f1ap_cfg.bind_addr;
  f1c_sctp_cfg.bind_port                   = F1AP_PORT;
  f1c_sctp_cfg.ppid                        = F1AP_PPID;
  f1c_cu_sctp_gateway_config f1c_server_cfg(
      {f1c_sctp_cfg, *epoll_broker, workers.get_cu_cp_executor_mapper().f1c_rx_executor(), *cu_cp_dlt_pcaps.f1ap});
  std::unique_ptr<srs_cu_cp::f1c_connection_server> cu_f1c_gw = srsran::create_f1c_gateway_server(f1c_server_cfg);

  // Instantiate E1 GW
  // > Create E1 config
  sctp_network_gateway_config e1_sctp_cfg{};
  e1_sctp_cfg.if_name      = "E1";
  e1_sctp_cfg.bind_address = cu_cp_cfg.e1ap_cfg.bind_addr;
  e1_sctp_cfg.bind_port    = E1AP_PORT;
  e1_sctp_cfg.ppid         = E1AP_PPID;
  // > Create E1 gateway
  std::unique_ptr<srs_cu_cp::e1_connection_server> e1_gw = create_e1_gateway_server(e1_cu_cp_sctp_gateway_config{
      e1_sctp_cfg, *epoll_broker, workers.get_cu_cp_executor_mapper().e1_rx_executor(), *cu_cp_dlt_pcaps.e1ap});

  // Instantiate E2AP client gateway.
  std::unique_ptr<e2_connection_client> e2_gw_cu_cp = create_e2_gateway_client(
      generate_e2_client_gateway_config(o_cu_cp_app_unit->get_o_cu_cp_unit_config().e2_cfg.base_config,
                                        *epoll_broker,
                                        workers.get_cu_cp_executor_mapper().e2_rx_executor(),
                                        *cu_cp_dlt_pcaps.e2ap,
                                        E2_CP_PPID));

  // Create app-level resource usage service and metrics.
  auto app_resource_usage_service = app_services::build_app_resource_usage_service(
      metrics_notifier_forwarder, cu_cp_cfg.metrics_cfg.rusage_config, cu_cp_logger);

  for (auto& metric : app_resource_usage_service.metrics) {
    metrics_configs.push_back(std::move(metric));
  }

  buffer_pool_service.add_metrics_to_metrics_service(
      metrics_configs, cu_cp_cfg.buffer_pool_config.metrics_config, metrics_notifier_forwarder);

  // Create O-CU-CP dependencies.
  o_cu_cp_unit_dependencies o_cucp_deps;
  o_cucp_deps.executor_mapper  = &workers.get_cu_cp_executor_mapper();
  o_cucp_deps.timers           = cu_timers;
  o_cucp_deps.ngap_pcap        = cu_cp_dlt_pcaps.ngap.get();
  o_cucp_deps.broker           = epoll_broker.get();
  o_cucp_deps.e2_gw            = e2_gw_cu_cp.get();
  o_cucp_deps.metrics_notifier = &metrics_notifier_forwarder;

  // Create O-CU-CP.
  auto                o_cucp_unit = o_cu_cp_app_unit->create_o_cu_cp(o_cucp_deps);
  srs_cu_cp::o_cu_cp& o_cucp_obj  = *o_cucp_unit.unit;

  if (std::unique_ptr<app_services::cmdline_command> cmd = app_services::create_stdout_metrics_app_command(
          {{o_cucp_unit.commands.cmdline.metrics_subcommands}}, false)) {
    o_cucp_unit.commands.cmdline.commands.push_back(std::move(cmd));
  }

  // Create console helper object for commands and metrics printing.
  app_services::cmdline_command_dispatcher command_parser(
      *epoll_broker, workers.get_cmd_line_executor(), o_cucp_unit.commands.cmdline.commands);

  for (auto& metric : o_cucp_unit.metrics) {
    metrics_configs.push_back(std::move(metric));
  }

  // Connect E1AP to O-CU-CP.
  e1_gw->attach_cu_cp(o_cucp_obj.get_cu_cp().get_e1_handler());

  // start O-CU-CP
  cu_cp_logger.info("Starting CU-CP...");
  o_cucp_obj.get_operation_controller().start();
  cu_cp_logger.info("CU-CP started successfully");

  // Check connection to AMF
  if (not o_cucp_obj.get_cu_cp().get_ng_handler().amfs_are_connected()) {
    report_error("CU-CP failed to connect to AMF");
  }

  // Connect F1-C to O-CU-CP and start listening for new F1-C connection requests.
  cu_f1c_gw->attach_cu_cp(o_cucp_obj.get_cu_cp().get_f1c_handler());

  app_services::metrics_manager metrics_mngr(
      srslog::fetch_basic_logger("CU"),
      workers.get_metrics_executor(),
      metrics_configs,
      app_timers,
      std::chrono::milliseconds(cu_cp_cfg.metrics_cfg.metrics_service_cfg.app_usage_report_period));

  // Connect the forwarder to the metrics manager.
  metrics_notifier_forwarder.connect(metrics_mngr);

  metrics_mngr.start();

  std::unique_ptr<app_services::remote_server> remote_control_server =
      app_services::create_remote_server(cu_cp_cfg.remote_control_config, {});

  {
    app_services::application_message_banners app_banner(
        app_name, cu_cp_cfg.log_cfg.filename == "stdout" ? std::string_view() : cu_cp_cfg.log_cfg.filename);

    auto exec_metrics_session = exec_metrics_service.service
                                    ? exec_metrics_service.service->create_session(workers.get_metrics_executor())
                                    : app_services::app_executor_metrics_service::create_dummy_session();

    while (is_app_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }

  metrics_mngr.stop();

  if (remote_control_server) {
    remote_control_server->stop();
  }

  // Stop O-CU-CP activity.
  o_cucp_obj.get_operation_controller().stop();

  // FIXME: closing the E1 gateway should be part of the E1 Release procedure
  e1_gw.reset();

  return 0;
}
