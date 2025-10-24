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

#include "adapters/f1_gateways.h"
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
#include "apps/units/flexible_o_du/flexible_o_du_application_unit.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_pcap_factory.h"
#include "du_appconfig.h"
#include "du_appconfig_cli11_schema.h"
#include "du_appconfig_translators.h"
#include "du_appconfig_validators.h"
#include "du_appconfig_yaml_writer.h"
#include "srsran/adt/scope_exit.h"
#include "srsran/du/du_high/du_high_clock_controller.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/e2/e2ap_config_translators.h"
#include "srsran/e2/gateways/e2_connection_client.h"
#include "srsran/e2/gateways/e2_network_client_factory.h"
#include "srsran/f1u/du/split_connector/f1u_split_connector_factory.h"
#include "srsran/f1u/split_connector/f1u_five_qi_gw_maps.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_demux_factory.h"
#include "srsran/support/backtrace.h"
#include "srsran/support/config_parsers.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/signal_handling.h"
#include "srsran/support/signal_observer.h"
#include "srsran/support/sysinfo.h"
#include "srsran/support/tracing/event_tracing.h"
#include "srsran/support/versioning/build_info.h"
#include "srsran/support/versioning/version.h"
#include <atomic>
#ifdef DPDK_FOUND
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#endif
// Include ThreadSanitizer (TSAN) options if thread sanitization is enabled.
// This include is not unused - it helps prevent false alarms from the thread sanitizer.
#include "srsran/support/tsan_options.h"

using namespace srsran;

/// \file
/// \brief Application of a distributed unit (DU) that is split from the CU-CP and CU-UP via the F1 interface.

static std::string config_file;

/// Flag that indicates if the application is running or being shutdown.
static std::atomic<bool> is_app_running = {true};
/// Maximum number of configuration files allowed to be concatenated in the command line.
static constexpr unsigned MAX_CONFIG_FILES = 10;

static void populate_cli11_generic_args(CLI::App& app)
{
  fmt::memory_buffer buffer;
  format_to(std::back_inserter(buffer), "srsRAN 5G DU version {} ({})", get_version(), get_build_hash());
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

static void register_app_logs(const du_appconfig& du_cfg, flexible_o_du_application_unit& du_app_unit)
{
  const logger_appconfig& log_cfg = du_cfg.log_cfg;
  // Set log-level of app and all non-layer specific components to app level.
  for (const auto& id : {"ALL", "SCTP-GW", "IO-EPOLL", "UDP-GW", "PCAP", "ASN1"}) {
    auto& logger = srslog::fetch_basic_logger(id, false);
    logger.set_level(log_cfg.lib_level);
    logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& app_logger = srslog::fetch_basic_logger("GNB", false);
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
  const app_helpers::metrics_config& metrics_cfg = du_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg;
  app_helpers::initialize_metrics_log_channels(metrics_cfg, log_cfg.hex_max_size);
  if (metrics_cfg.enable_json_metrics) {
    app_services::initialize_json_channel();
  }

  auto& e2ap_logger = srslog::fetch_basic_logger("E2AP", false);
  e2ap_logger.set_level(log_cfg.e2ap_level);
  e2ap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  // Register units logs.
  du_app_unit.on_loggers_registration();
}

int main(int argc, char** argv)
{
  // Set the application error handler.
  set_error_handler(app_error_report_handler);

  static constexpr std::string_view app_name = "DU";
  app_services::application_message_banners::announce_app_and_version(app_name);

  // Set interrupt and cleanup signal handlers.
  register_interrupt_signal_handler(interrupt_signal_handler);
  register_cleanup_signal_handler(cleanup_signal_handler);

  // Enable backtrace.
  enable_backtrace();

  // Setup and configure config parsing.
  CLI::App app("srsDU application");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  // Fill the generic application arguments to parse.
  populate_cli11_generic_args(app);

  du_appconfig du_cfg;
  // Configure CLI11 with the DU application configuration schema.
  configure_cli11_with_du_appconfig_schema(app, du_cfg);

  auto o_du_app_unit = create_flexible_o_du_application_unit("du");
  o_du_app_unit->on_parsing_configuration_registration(app);

  // Set the callback for the app calling all the autoderivation functions.
  app.callback([&app, &du_cfg, &o_du_app_unit]() {
    autoderive_du_parameters_after_parsing(app, du_cfg);
    o_du_app_unit->on_configuration_parameters_autoderivation(app);
  });

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  // Dry run mode, exit.
  if (du_cfg.enable_dryrun) {
    return 0;
  }

  if (du_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg.enable_json_metrics &&
      !du_cfg.remote_control_config.enabled) {
    fmt::println("NOTE: No JSON metrics will be generated as the remote server is disabled");
  }

  // Check the modified configuration.
  if (!validate_appconfig(du_cfg) || !o_du_app_unit->on_configuration_validation()) {
    report_error("Invalid configuration detected.\n");
  }

  // Set up logging.
  initialize_log(du_cfg.log_cfg.filename);
  auto log_flusher = make_scope_exit([]() { srslog::flush(); });
  register_app_logs(du_cfg, *o_du_app_unit);

  // Check the metrics and metrics consumers.
  srslog::basic_logger& gnb_logger = srslog::fetch_basic_logger("GNB");
  bool metrics_enabled = o_du_app_unit->are_metrics_enabled() || du_cfg.metrics_cfg.rusage_config.enable_app_usage;

  if (!metrics_enabled && du_cfg.metrics_cfg.rusage_config.metrics_consumers_cfg.enabled()) {
    gnb_logger.warning("Logger or JSON metrics output enabled but no metrics will be reported as no layer was enabled");
    fmt::println("Logger or JSON metrics output enabled but no metrics will be reported as no layer was enabled");
  }

  // Log input configuration.
  srslog::basic_logger& config_logger = srslog::fetch_basic_logger("CONFIG");
  if (config_logger.debug.enabled()) {
    YAML::Node node;
    fill_du_appconfig_in_yaml_schema(node, du_cfg);
    o_du_app_unit->dump_config(node);
    config_logger.debug("Input configuration (all values): \n{}", YAML::Dump(node));
  } else {
    config_logger.info("Input configuration (only non-default values): \n{}", app.config_to_str(false, false));
  }

  srslog::basic_logger&            du_logger = srslog::fetch_basic_logger("DU");
  app_services::application_tracer app_tracer;
  if (not du_cfg.trace_cfg.filename.empty()) {
    app_tracer.enable_tracer(du_cfg.trace_cfg.filename,
                             du_cfg.trace_cfg.max_tracing_events_per_file,
                             du_cfg.trace_cfg.nof_tracing_events_after_severe,
                             du_logger);
  }

#ifdef DPDK_FOUND
  std::unique_ptr<dpdk::dpdk_eal> eal;
  if (du_cfg.hal_config) {
    // Prepend the application name in argv[0] as it is expected by EAL.
    eal = dpdk::create_dpdk_eal(std::string(argv[0]) + " " + du_cfg.hal_config->eal_args,
                                srslog::fetch_basic_logger("EAL", false));
  }
#endif

  // Setup size of byte buffer pool.
  app_services::buffer_pool_manager buffer_pool_service(du_cfg.buffer_pool_config);

  // Log CPU architecture.
  cpu_architecture_info::get().print_cpu_info(du_logger);

  // Check and log included CPU features and check support by current CPU
  if (cpu_supports_included_features()) {
    du_logger.debug("Required CPU features: {}", get_cpu_feature_info());
  } else {
    // Quit here until we complete selection of the best matching implementation for the current CPU at runtime.
    du_logger.error("The CPU does not support the required CPU features that were configured during compile time: {}",
                    get_cpu_feature_info());
    report_error("The CPU does not support the required CPU features that were configured during compile time: {}\n",
                 get_cpu_feature_info());
  }

  // Check some common causes of performance issues and print a warning if required.
  check_cpu_governor(du_logger);
  check_drm_kms_polling(du_logger);

  // Create manager of timers for DU, which will be driven by the PHY slot ticks.
  timer_manager app_timers{256};

  app_services::metrics_notifier_proxy_impl metrics_notifier_forwarder;

  // Instantiate executor metrics service.
  app_services::executor_metrics_service_and_metrics exec_metrics_service =
      build_executor_metrics_service(metrics_notifier_forwarder, app_timers, du_cfg.metrics_cfg.executors_metrics_cfg);
  std::vector<app_services::metrics_config> app_metrics = std::move(exec_metrics_service.metrics);

  // Instantiate worker manager.
  worker_manager_config worker_manager_cfg;
  o_du_app_unit->fill_worker_manager_config(worker_manager_cfg);
  fill_du_worker_manager_config(worker_manager_cfg, du_cfg);
  worker_manager_cfg.app_timers                    = &app_timers;
  worker_manager_cfg.exec_metrics_channel_registry = exec_metrics_service.channel_registry;

  worker_manager workers{worker_manager_cfg};

  // Create IO broker.
  const auto&                main_pool_cpu_mask = du_cfg.expert_execution_cfg.affinities.main_pool_cpu_cfg.mask;
  io_broker_config           io_broker_cfg(os_thread_realtime_priority::min() + 5, main_pool_cpu_mask);
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll, io_broker_cfg);

  // Create a IO timer source.
  auto time_ctrl =
      srs_du::create_du_high_clock_controller(app_timers, *epoll_broker, workers.get_timer_source_executor());

  auto on_pcap_close = make_scope_exit([&gnb_logger]() { gnb_logger.info("PCAP files successfully closed."); });
  flexible_o_du_pcaps du_pcaps = create_o_du_pcaps(
      o_du_app_unit->get_o_du_high_unit_config(), workers.get_du_pcap_executors(), cleanup_signal_dispatcher);
  auto on_pcap_close_init = make_scope_exit([&gnb_logger]() { gnb_logger.info("Closing PCAP files..."); });

  // Instantiate F1-C client gateway.
  std::unique_ptr<srs_du::f1c_connection_client> f1c_gw =
      create_f1c_client_gateway(du_cfg.f1ap_cfg.cu_cp_address,
                                du_cfg.f1ap_cfg.bind_address,
                                *epoll_broker,
                                workers.get_du_high_executor_mapper().f1c_rx_executor(),
                                *du_pcaps.f1ap);

  // Create F1-U GW.
  // > Create GTP-U Demux.
  gtpu_demux_creation_request du_f1u_gtpu_msg   = {};
  du_f1u_gtpu_msg.cfg.warn_on_drop              = true;
  du_f1u_gtpu_msg.cfg.queue_size                = du_cfg.f1u_cfg.pdu_queue_size;
  du_f1u_gtpu_msg.gtpu_pcap                     = du_pcaps.f1u.get();
  std::unique_ptr<gtpu_demux> du_f1u_gtpu_demux = create_gtpu_demux(du_f1u_gtpu_msg);

  // > Create UDP gateway(s).
  gtpu_gateway_maps f1u_gw_maps;
  for (const f1u_socket_appconfig& sock_cfg : du_cfg.f1u_cfg.f1u_sockets.f1u_socket_cfg) {
    udp_network_gateway_config f1u_gw_config = {};
    f1u_gw_config.bind_address               = sock_cfg.bind_addr;
    f1u_gw_config.ext_bind_addr              = sock_cfg.udp_config.ext_addr;
    f1u_gw_config.bind_port                  = du_cfg.f1u_cfg.f1u_sockets.bind_port;
    f1u_gw_config.reuse_addr                 = false;
    f1u_gw_config.pool_occupancy_threshold   = sock_cfg.udp_config.pool_threshold;
    f1u_gw_config.rx_max_mmsg                = sock_cfg.udp_config.rx_max_msgs;
    f1u_gw_config.dscp                       = sock_cfg.udp_config.dscp;
    std::unique_ptr<gtpu_gateway> f1u_gw     = create_udp_gtpu_gateway(
        f1u_gw_config,
        *epoll_broker,
        workers.get_du_high_executor_mapper().ue_mapper().mac_ul_pdu_executor(to_du_ue_index(0)),
        workers.get_du_high_executor_mapper().f1u_rx_executor());
    if (not sock_cfg.five_qi.has_value()) {
      f1u_gw_maps.default_gws.push_back(std::move(f1u_gw));
    } else {
      f1u_gw_maps.five_qi_gws[sock_cfg.five_qi.value()].push_back(std::move(f1u_gw));
    }
  }

  // > Create F1-U split connector.
  std::unique_ptr<srs_du::f1u_du_udp_gateway> du_f1u_conn = srs_du::create_split_f1u_gw(
      {f1u_gw_maps, du_f1u_gtpu_demux.get(), *du_pcaps.f1u, du_cfg.f1u_cfg.f1u_sockets.peer_port});

  // Instantiate E2AP client gateway.
  std::unique_ptr<e2_connection_client> e2_gw = create_e2_gateway_client(
      generate_e2_client_gateway_config(o_du_app_unit->get_o_du_high_unit_config().e2_cfg.base_cfg,
                                        *epoll_broker,
                                        workers.get_du_high_executor_mapper().e2_rx_executor(),
                                        *du_pcaps.e2ap,
                                        E2_DU_PPID));

  // Create app-level resource usage service and metrics.
  auto app_resource_usage_service = app_services::build_app_resource_usage_service(
      metrics_notifier_forwarder, du_cfg.metrics_cfg.rusage_config, srslog::fetch_basic_logger("GNB"));

  for (auto& metric : app_resource_usage_service.metrics) {
    app_metrics.push_back(std::move(metric));
  }

  buffer_pool_service.add_metrics_to_metrics_service(
      app_metrics, du_cfg.buffer_pool_config.metrics_config, metrics_notifier_forwarder);

  o_du_unit_dependencies du_dependencies;
  du_dependencies.workers            = &workers;
  du_dependencies.f1c_client_handler = f1c_gw.get();
  du_dependencies.f1u_gw             = du_f1u_conn.get();
  du_dependencies.timer_ctrl         = time_ctrl.get();
  du_dependencies.mac_p              = du_pcaps.mac.get();
  du_dependencies.rlc_p              = du_pcaps.rlc.get();
  du_dependencies.e2_client_handler  = e2_gw.get();
  du_dependencies.metrics_notifier   = &metrics_notifier_forwarder;

  auto du_inst_and_cmds = o_du_app_unit->create_flexible_o_du_unit(du_dependencies);

  // Move app-units metrics to the application metrics.
  std::move(du_inst_and_cmds.metrics.begin(), du_inst_and_cmds.metrics.end(), std::back_inserter(app_metrics));

  // Only DU has metrics now.
  app_services::metrics_manager metrics_mngr(
      srslog::fetch_basic_logger("GNB"),
      workers.get_metrics_executor(),
      app_metrics,
      app_timers,
      std::chrono::milliseconds(du_cfg.metrics_cfg.metrics_service_cfg.app_usage_report_period));

  // Connect the forwarder to the metrics manager.
  metrics_notifier_forwarder.connect(metrics_mngr);

  srs_du::du& du_inst = *du_inst_and_cmds.unit;

  // Add the metrics STDOUT command.
  if (std::unique_ptr<app_services::cmdline_command> cmd = app_services::create_stdout_metrics_app_command(
          {{du_inst_and_cmds.commands.cmdline.metrics_subcommands}}, du_cfg.metrics_cfg.autostart_stdout_metrics)) {
    du_inst_and_cmds.commands.cmdline.commands.push_back(std::move(cmd));
  }

  // Register the commands.
  app_services::cmdline_command_dispatcher command_parser(
      *epoll_broker, workers.get_cmd_line_executor(), du_inst_and_cmds.commands.cmdline.commands);

  // Start processing.
  du_inst.get_operation_controller().start();

  std::unique_ptr<app_services::remote_server> remote_control_server =
      app_services::create_remote_server(du_cfg.remote_control_config, du_inst_and_cmds.commands.remote);

  metrics_mngr.start();
  {
    app_services::application_message_banners app_banner(
        app_name, du_cfg.log_cfg.filename == "stdout" ? std::string_view() : du_cfg.log_cfg.filename);

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

  // Stop DU activity.
  du_inst.get_operation_controller().stop();

  return 0;
}
