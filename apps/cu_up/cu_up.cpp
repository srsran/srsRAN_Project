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

#include "apps/cu_up/cu_up_appconfig_cli11_schema.h"
#include "apps/helpers/f1u/f1u_appconfig.h"
#include "apps/helpers/metrics/metrics_helpers.h"
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
#include "apps/units/o_cu_up/o_cu_up_application_unit.h"
#include "apps/units/o_cu_up/o_cu_up_unit_config.h"
#include "apps/units/o_cu_up/pcap_factory.h"
#include "cu_up_appconfig.h"
#include "srsran/adt/scope_exit.h"
#include "srsran/e1ap/gateways/e1_network_client_factory.h"
#include "srsran/e2/e2ap_config_translators.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/f1u/cu_up/split_connector/f1u_split_connector_factory.h"
#include "srsran/f1u/split_connector/f1u_five_qi_gw_maps.h"
#include "srsran/gateways/udp_network_gateway.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_demux_factory.h"
#include "srsran/gtpu/gtpu_gateway.h"
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
/// \brief Application of a Central Unit User-Plane (CU-UP).
///
/// This application runs a CU-UP with the E1 connection between the CU-CP and CU-UP, N3 connection to the UPF
/// and F1-U interface to the DU.
///
/// The app serves an stand-alone CU-UP.

static std::string config_file;

/// Flag that indicates if the application is running or being shutdown.
static std::atomic<bool> is_app_running = {true};
/// Maximum number of configuration files allowed to be concatenated in the command line.
static constexpr unsigned MAX_CONFIG_FILES = 10;

static void populate_cli11_generic_args(CLI::App& app)
{
  fmt::memory_buffer buffer;
  format_to(std::back_inserter(buffer), "srsRAN 5G CU-UP version {} ({})", get_version(), get_build_hash());
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

static void register_app_logs(const cu_up_appconfig& cu_up_cfg, o_cu_up_application_unit& cu_up_app_unit)
{
  const logger_appconfig& log_cfg = cu_up_cfg.log_cfg;
  // Set log-level of app and all non-layer specific components to app level.
  for (const auto& id : {"ALL", "SCTP-GW", "IO-EPOLL", "UDP-GW", "PCAP", "ASN1"}) {
    auto& logger = srslog::fetch_basic_logger(id, false);
    logger.set_level(log_cfg.lib_level);
    logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& app_logger = srslog::fetch_basic_logger("CU-UP", false);
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
  const app_helpers::metrics_config& metrics_cfg = cu_up_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg;
  app_helpers::initialize_metrics_log_channels(metrics_cfg, log_cfg.hex_max_size);
  if (metrics_cfg.enable_json_metrics) {
    app_services::initialize_json_channel();
  }

  // Register units logs.
  cu_up_app_unit.on_loggers_registration();
}

static void fill_cu_worker_manager_config(worker_manager_config& config, const cu_up_appconfig& unit_cfg)
{
  config.nof_main_pool_threads     = unit_cfg.expert_execution_cfg.threads.main_pool.nof_threads;
  config.main_pool_task_queue_size = unit_cfg.expert_execution_cfg.threads.main_pool.task_queue_size;
  config.main_pool_affinity_cfg    = unit_cfg.expert_execution_cfg.affinities.main_pool_cpu_cfg;
}

static void autoderive_cu_up_parameters_after_parsing(cu_up_appconfig& cu_up_config, o_cu_up_unit_config& o_cu_up_cfg)
{
  // If no UPF is configured, we set a default configuration.
  if (o_cu_up_cfg.cu_up_cfg.ngu_cfg.ngu_socket_cfg.empty()) {
    cu_up_unit_ngu_socket_config sock_cfg;
    o_cu_up_cfg.cu_up_cfg.ngu_cfg.ngu_socket_cfg.push_back(sock_cfg);
  }
  // If no F1-U socket configuration is derived, we set a default configuration.
  if (cu_up_config.f1u_cfg.f1u_socket_cfg.empty()) {
    f1u_socket_appconfig sock_cfg;
    cu_up_config.f1u_cfg.f1u_socket_cfg.push_back(sock_cfg);
  }
}

int main(int argc, char** argv)
{
  // Set the application error handler.
  set_error_handler(app_error_report_handler);

  static constexpr std::string_view app_name = "CU-UP";
  app_services::application_message_banners::announce_app_and_version(app_name);

  // Set interrupt and cleanup signal handlers.
  register_interrupt_signal_handler(interrupt_signal_handler);
  register_cleanup_signal_handler(cleanup_signal_handler);

  // Enable backtrace.
  enable_backtrace();

  // Setup and configure config parsing.
  CLI::App app("srsCU-UP application");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  // Fill the generic application arguments to parse.
  populate_cli11_generic_args(app);

  // Configure CLI11 with the CU application configuration schema.
  cu_up_appconfig cu_up_cfg;
  configure_cli11_with_cu_appconfig_schema(app, cu_up_cfg);

  auto o_cu_up_app_unit = create_o_cu_up_application_unit("cu-up");
  o_cu_up_app_unit->on_parsing_configuration_registration(app);

  // Set the callback for the app calling all the autoderivation functions.
  app.callback([&app, &cu_up_cfg, &o_cu_up_app_unit]() {
    o_cu_up_app_unit->on_configuration_parameters_autoderivation(app);

    autoderive_cu_up_parameters_after_parsing(cu_up_cfg, o_cu_up_app_unit->get_o_cu_up_unit_config());
  });

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  // Dry run mode, exit.
  if (cu_up_cfg.enable_dryrun) {
    return 0;
  }

  if (cu_up_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg.enable_json_metrics &&
      !cu_up_cfg.remote_control_config.enabled) {
    fmt::println("NOTE: No JSON metrics will be generated as the remote server is disabled");
  }

  // TODO: validate appconfig

  // Set up logging.
  initialize_log(cu_up_cfg.log_cfg.filename);
  auto log_flusher = make_scope_exit([]() { srslog::flush(); });
  register_app_logs(cu_up_cfg, *o_cu_up_app_unit);

  // Check the metrics and metrics consumers.
  srslog::basic_logger& cu_up_logger = srslog::fetch_basic_logger("CU-UP");
  bool                  metrics_enabled =
      o_cu_up_app_unit->are_metrics_enabled() || cu_up_cfg.metrics_cfg.rusage_config.enable_app_usage;

  if (!metrics_enabled && cu_up_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg.enabled()) {
    cu_up_logger.warning(
        "Logger or JSON metrics output enabled but no metrics will be reported as no layer was enabled");
    fmt::println("Logger or JSON metrics output enabled but no metrics will be reported as no layer was enabled");
  }

  // TODO: Log input configuration.

  app_services::application_tracer app_tracer;
  if (not cu_up_cfg.log_cfg.tracing_filename.empty()) {
    app_tracer.enable_tracer(cu_up_cfg.log_cfg.tracing_filename, cu_up_logger);
  }

  // configure cgroups
  // TODO

  // Setup size of byte buffer pool.
  app_services::buffer_pool_manager buffer_pool_service(cu_up_cfg.buffer_pool_config);

  // Log CPU architecture.
  // TODO

  // Check and log included CPU features and check support by current CPU
  if (cpu_supports_included_features()) {
    cu_up_logger.debug("Required CPU features: {}", get_cpu_feature_info());
  } else {
    // Quit here until we complete selection of the best matching implementation for the current CPU at runtime.
    cu_up_logger.error(
        "The CPU does not support the required CPU features that were configured during compile time: {}",
        get_cpu_feature_info());
    report_error("The CPU does not support the required CPU features that were configured during compile time: {}\n",
                 get_cpu_feature_info());
  }

  // Check some common causes of performance issues and print a warning if required.
  check_cpu_governor(cu_up_logger);
  check_drm_kms_polling(cu_up_logger);

  // Create manager of timers for CU-CP and CU-UP, which will be driven by the system timer slot ticks.
  timer_manager  app_timers{256};
  timer_manager* cu_up_timers = &app_timers;

  // Create worker manager.
  worker_manager_config worker_manager_cfg;
  fill_cu_worker_manager_config(worker_manager_cfg, cu_up_cfg);
  o_cu_up_app_unit->fill_worker_manager_config(worker_manager_cfg);
  worker_manager_cfg.app_timers = &app_timers;
  worker_manager workers{worker_manager_cfg};

  // Create IO broker.
  const auto&                main_pool_cpu_mask = cu_up_cfg.expert_execution_cfg.affinities.main_pool_cpu_cfg.mask;
  io_broker_config           io_broker_cfg(os_thread_realtime_priority::min() + 5, main_pool_cpu_mask);
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll, io_broker_cfg);

  // Create time source that ticks the timers.
  std::optional<io_timer_source> time_source(
      std::in_place_t{}, app_timers, *epoll_broker, workers.get_timer_source_executor(), std::chrono::milliseconds{1});

  // Create layer specific PCAPs.
  auto on_pcap_close = make_scope_exit([&cu_up_logger]() { cu_up_logger.info("PCAP files successfully closed."); });
  o_cu_up_dlt_pcaps cu_up_dlt_pcaps = create_o_cu_up_dlt_pcaps(
      o_cu_up_app_unit->get_o_cu_up_unit_config(), workers.get_cu_up_pcap_executors(), cleanup_signal_dispatcher);
  auto on_pcap_close_init = make_scope_exit([&cu_up_logger]() { cu_up_logger.info("Closing PCAP files..."); });

  // Create F1-U GW.
  // > Create GTP-U Demux.
  gtpu_demux_creation_request cu_f1u_gtpu_msg   = {};
  cu_f1u_gtpu_msg.cfg.warn_on_drop              = true;
  cu_f1u_gtpu_msg.gtpu_pcap                     = cu_up_dlt_pcaps.f1u.get();
  std::unique_ptr<gtpu_demux> cu_f1u_gtpu_demux = create_gtpu_demux(cu_f1u_gtpu_msg);
  // > Create UDP gateway(s).
  gtpu_gateway_maps f1u_gw_maps;
  for (const f1u_socket_appconfig& sock_cfg : cu_up_cfg.f1u_cfg.f1u_socket_cfg) {
    udp_network_gateway_config cu_f1u_gw_config = {};
    cu_f1u_gw_config.bind_address               = sock_cfg.bind_addr;
    cu_f1u_gw_config.ext_bind_addr              = sock_cfg.udp_config.ext_addr;
    cu_f1u_gw_config.bind_port                  = cu_up_cfg.f1u_cfg.bind_port;
    cu_f1u_gw_config.reuse_addr                 = false;
    cu_f1u_gw_config.pool_occupancy_threshold   = sock_cfg.udp_config.pool_threshold;
    cu_f1u_gw_config.rx_max_mmsg                = sock_cfg.udp_config.rx_max_msgs;
    cu_f1u_gw_config.dscp                       = sock_cfg.udp_config.dscp;
    std::unique_ptr<gtpu_gateway> cu_f1u_gw =
        create_udp_gtpu_gateway(cu_f1u_gw_config,
                                *epoll_broker,
                                workers.get_cu_up_executor_mapper().io_ul_executor(),
                                workers.get_cu_up_executor_mapper().f1u_rx_executor());
    if (not sock_cfg.five_qi.has_value()) {
      f1u_gw_maps.default_gws.push_back(std::move(cu_f1u_gw));
    } else {
      f1u_gw_maps.five_qi_gws[sock_cfg.five_qi.value()].push_back(std::move(cu_f1u_gw));
    }
  }
  std::unique_ptr<f1u_cu_up_udp_gateway> cu_f1u_conn = srs_cu_up::create_split_f1u_gw(
      {f1u_gw_maps, *cu_f1u_gtpu_demux, *cu_up_dlt_pcaps.f1u, cu_up_cfg.f1u_cfg.peer_port});

  // Instantiate E1 client gateway.
  // > Create E1 config
  sctp_network_connector_config e1_sctp{};
  e1_sctp.if_name         = "E1";
  e1_sctp.dest_name       = "CU-CP";
  e1_sctp.connect_address = cu_up_cfg.e1ap_cfg.cu_cp_address;
  e1_sctp.connect_port    = E1AP_PORT;
  e1_sctp.ppid            = E1AP_PPID;
  e1_sctp.bind_address    = cu_up_cfg.e1ap_cfg.bind_address;
  // > Create E1 gateway
  std::unique_ptr<srs_cu_up::e1_connection_client> e1_gw = create_e1_gateway_client(e1_cu_up_sctp_gateway_config{
      e1_sctp, *epoll_broker, workers.get_cu_up_executor_mapper().e1_rx_executor(), *cu_up_dlt_pcaps.e1ap});

  // Instantiate E2AP client gateway.
  std::unique_ptr<e2_connection_client> e2_gw_cu_up = create_e2_gateway_client(
      generate_e2_client_gateway_config(o_cu_up_app_unit->get_o_cu_up_unit_config().e2_cfg.base_config,
                                        *epoll_broker,
                                        workers.get_cu_up_executor_mapper().e2_rx_executor(),
                                        *cu_up_dlt_pcaps.e2ap,
                                        E2_UP_PPID));

  app_services::metrics_notifier_proxy_impl metrics_notifier_forwarder;

  // Create app-level resource usage service and metrics.
  auto app_resource_usage_service = app_services::build_app_resource_usage_service(
      metrics_notifier_forwarder, cu_up_cfg.metrics_cfg.rusage_config, cu_up_logger);

  std::vector<app_services::metrics_config> metrics_configs = std::move(app_resource_usage_service.metrics);

  // TODO: Create console helper object for commands and metrics printing.

  // Create and start O-CU-UP
  o_cu_up_unit_dependencies o_cuup_unit_deps;
  o_cuup_unit_deps.workers          = &workers;
  o_cuup_unit_deps.e1ap_conn_client = e1_gw.get();
  o_cuup_unit_deps.f1u_gateway      = cu_f1u_conn.get();
  o_cuup_unit_deps.gtpu_pcap        = cu_up_dlt_pcaps.n3.get();
  o_cuup_unit_deps.timers           = cu_up_timers;
  o_cuup_unit_deps.io_brk           = epoll_broker.get();
  o_cuup_unit_deps.e2_gw            = e2_gw_cu_up.get();
  o_cuup_unit_deps.metrics_notifier = &metrics_notifier_forwarder;

  auto o_cuup_unit = o_cu_up_app_unit->create_o_cu_up_unit(o_cuup_unit_deps);
  for (auto& metric : o_cuup_unit.metrics) {
    metrics_configs.push_back(std::move(metric));
  }
  app_services::metrics_manager metrics_mngr(
      srslog::fetch_basic_logger("CU-UP"),
      workers.get_metrics_executor(),
      metrics_configs,
      app_timers,
      std::chrono::milliseconds(cu_up_cfg.metrics_cfg.metrics_service_cfg.app_usage_report_period));

  // Connect the forwarder to the metrics manager.
  metrics_notifier_forwarder.connect(metrics_mngr);

  o_cuup_unit.unit->get_operation_controller().start();

  metrics_mngr.start();

  std::unique_ptr<app_services::remote_server> remote_control_server =
      app_services::create_remote_server(cu_up_cfg.remote_control_config, {});

  {
    app_services::application_message_banners app_banner(
        app_name, cu_up_cfg.log_cfg.filename == "stdout" ? std::string_view() : cu_up_cfg.log_cfg.filename);

    while (is_app_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }

  metrics_mngr.stop();

  if (remote_control_server) {
    remote_control_server->stop();
  }

  // Stop O-CU-UP activity.
  o_cuup_unit.unit->get_operation_controller().stop();

  // FIXME: closing the E1 gateway should be part of the E1 Release procedure
  e1_gw.reset();

  return 0;
}
