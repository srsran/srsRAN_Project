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

#include "apps/helpers/e2/e2_metric_connector_manager.h"
#include "apps/helpers/metrics/metrics_helpers.h"
#include "apps/services/app_resource_usage/app_resource_usage.h"
#include "apps/services/application_message_banners.h"
#include "apps/services/application_tracer.h"
#include "apps/services/buffer_pool/buffer_pool_manager.h"
#include "apps/services/cmdline/cmdline_command_dispatcher.h"
#include "apps/services/core_isolation_manager.h"
#include "apps/services/metrics/metrics_manager.h"
#include "apps/services/metrics/metrics_notifier_proxy.h"
#include "apps/services/periodic_metrics_report_controller.h"
#include "apps/services/remote_control/remote_server.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/flexible_o_du_application_unit.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_pcap_factory.h"
#include "apps/units/o_cu_cp/o_cu_cp_application_unit.h"
#include "apps/units/o_cu_cp/o_cu_cp_unit_config.h"
#include "apps/units/o_cu_cp/pcap_factory.h"
#include "apps/units/o_cu_up/o_cu_up_application_unit.h"
#include "apps/units/o_cu_up/o_cu_up_unit_config.h"
#include "apps/units/o_cu_up/pcap_factory.h"
#include "gnb_appconfig.h"
#include "gnb_appconfig_cli11_schema.h"
#include "gnb_appconfig_translators.h"
#include "gnb_appconfig_validators.h"
#include "gnb_appconfig_yaml_writer.h"
#include "srsran/cu_cp/cu_cp_operation_controller.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/e1ap/gateways/e1_local_connector_factory.h"
#include "srsran/e2/e2ap_config_translators.h"
#include "srsran/e2/gateways/e2_network_client_factory.h"
#include "srsran/f1ap/gateways/f1c_local_connector_factory.h"
#include "srsran/f1u/local_connector/f1u_local_connector.h"
#include "srsran/ngap/gateways/n2_connection_client_factory.h"
#include "srsran/support/backtrace.h"
#include "srsran/support/config_parsers.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/signal_handling.h"
#include "srsran/support/signal_observer.h"
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
/// \brief Application of a co-located gNB with combined distributed unit (DU) and centralized unit (CU).
///
/// This application runs a gNB without the the F1 connection between CU and DU and without the E1 connection
/// between the CU-CP and CU-UP going over a real SCTP connection. However, its does expose the N2 and N3 interface
/// to the AMF and UPF over the standard SCTP ports.
/// The app serves as an example for a all-integrated, small-cell-style gNB.

static std::string config_file;

/// Flag that indicates if the application is running or being shutdown.
static std::atomic<bool> is_app_running = {true};
/// Maximum number of configuration files allowed to be concatenated in the command line.
static constexpr unsigned MAX_CONFIG_FILES = 10;

static void populate_cli11_generic_args(CLI::App& app)
{
  fmt::memory_buffer buffer;
  format_to(std::back_inserter(buffer), "srsRAN 5G gNB version {} ({})", get_version(), get_build_hash());
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
  srslog::flush();
}

/// Function to call when an error is reported by the application.
static void app_error_report_handler()
{
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

static void register_app_logs(const gnb_appconfig&            gnb_cfg,
                              o_cu_cp_application_unit&       cu_cp_app_unit,
                              o_cu_up_application_unit&       cu_up_app_unit,
                              flexible_o_du_application_unit& du_app_unit)
{
  const logger_appconfig& log_cfg = gnb_cfg.log_cfg;
  // Set log-level of app and all non-layer specific components to app level.
  for (const auto& id : {"ALL", "SCTP-GW", "IO-EPOLL", "UDP-GW", "PCAP"}) {
    auto& logger = srslog::fetch_basic_logger(id, false);
    logger.set_level(log_cfg.lib_level);
    logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& app_logger = srslog::fetch_basic_logger("GNB", false);
  app_logger.set_level(srslog::basic_levels::info);
  app_services::application_message_banners::log_build_info(app_logger);
  app_logger.set_level(log_cfg.config_level);
  app_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& config_logger = srslog::fetch_basic_logger("CONFIG", false);
  config_logger.set_level(log_cfg.config_level);
  config_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& e2ap_logger = srslog::fetch_basic_logger("E2AP", false);
  e2ap_logger.set_level(log_cfg.e2ap_level);
  e2ap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  // Metrics log channels.
  const app_helpers::metrics_config& metrics_cfg = gnb_cfg.metrics_cfg.common_metrics_cfg;
  app_helpers::initialize_metrics_log_channels(metrics_cfg, log_cfg.hex_max_size);

  // Register units logs.
  cu_cp_app_unit.on_loggers_registration();
  cu_up_app_unit.on_loggers_registration();
  du_app_unit.on_loggers_registration();
}

static void autoderive_slicing_args(du_high_unit_config& du_hi_cfg, cu_cp_unit_config& cu_cp_config)
{
  std::vector<s_nssai_t> du_slices;
  for (const auto& cell_cfg : du_hi_cfg.cells_cfg) {
    for (const auto& slice : cell_cfg.cell.slice_cfg) {
      s_nssai_t nssai{slice_service_type{slice.sst}, slice_differentiator::create(slice.sd).value()};
      if (du_slices.end() == std::find(du_slices.begin(), du_slices.end(), nssai)) {
        du_slices.push_back(nssai);
      }
    }
  }
  // NOTE: A CU-CP can serve more slices than slices configured in the DU cells.
  // [Implementation-defined] Ensure that all slices served by DU cells are part of CU-CP served slices.
  for (const auto& slice : du_slices) {
    if (cu_cp_config.slice_cfg.end() ==
        std::find(cu_cp_config.slice_cfg.begin(), cu_cp_config.slice_cfg.end(), slice)) {
      cu_cp_config.slice_cfg.push_back(slice);
    }
  }
}

static void autoderive_cu_up_parameters_after_parsing(cu_up_unit_config& cu_up_cfg, const cu_cp_unit_config& cu_cp_cfg)
{
  // If no UPF is configured, we set the UPF configuration from the CU-CP AMF configuration.
  if (cu_up_cfg.ngu_cfg.ngu_socket_cfg.empty()) {
    cu_up_unit_ngu_socket_config sock_cfg;
    sock_cfg.bind_addr = cu_cp_cfg.amf_config.amf.bind_addr;
    cu_up_cfg.ngu_cfg.ngu_socket_cfg.push_back(sock_cfg);
  }
}

int main(int argc, char** argv)
{
  // Set the application error handler.
  set_error_handler(app_error_report_handler);

  static constexpr std::string_view app_name = "gNB";
  app_services::application_message_banners::announce_app_and_version(app_name);

  // Set interrupt and cleanup signal handlers.
  register_interrupt_signal_handler(interrupt_signal_handler);
  register_cleanup_signal_handler(cleanup_signal_handler);

  // Enable backtrace.
  enable_backtrace();

  // Setup and configure config parsing.
  CLI::App app("srsGNB application");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  // Fill the generic application arguments to parse.
  populate_cli11_generic_args(app);

  gnb_appconfig gnb_cfg;
  // Configure CLI11 with the gNB application configuration schema.
  configure_cli11_with_gnb_appconfig_schema(app, gnb_cfg);

  auto o_cu_cp_app_unit = create_o_cu_cp_application_unit("gnb");
  o_cu_cp_app_unit->on_parsing_configuration_registration(app);

  auto o_cu_up_app_unit = create_o_cu_up_application_unit("gnb");
  o_cu_up_app_unit->on_parsing_configuration_registration(app);

  auto o_du_app_unit = create_flexible_o_du_application_unit("gnb");
  o_du_app_unit->on_parsing_configuration_registration(app);

  // Set the callback for the app calling all the autoderivation functions.
  app.callback([&app, &gnb_cfg, &o_du_app_unit, &o_cu_cp_app_unit, &o_cu_up_app_unit]() {
    autoderive_gnb_parameters_after_parsing(app, gnb_cfg);
    autoderive_slicing_args(o_du_app_unit->get_o_du_high_unit_config().du_high_cfg.config,
                            o_cu_cp_app_unit->get_o_cu_cp_unit_config().cucp_cfg);
    o_du_app_unit->on_configuration_parameters_autoderivation(app);

    // If test mode is enabled, we auto-enable "no_core" option and generate a amf config with no core.
    if (o_du_app_unit->get_o_du_high_unit_config().du_high_cfg.config.is_testmode_enabled()) {
      o_cu_cp_app_unit->get_o_cu_cp_unit_config().cucp_cfg.amf_config.no_core           = true;
      o_cu_cp_app_unit->get_o_cu_cp_unit_config().cucp_cfg.amf_config.amf.supported_tas = {
          {7, {{"00101", {cu_cp_unit_plmn_item::tai_slice_t{1}}}}}};
    }

    o_cu_cp_app_unit->on_configuration_parameters_autoderivation(app);
    o_cu_up_app_unit->on_configuration_parameters_autoderivation(app);
    autoderive_cu_up_parameters_after_parsing(o_cu_up_app_unit->get_o_cu_up_unit_config().cu_up_cfg,
                                              o_cu_cp_app_unit->get_o_cu_cp_unit_config().cucp_cfg);
  });

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  auto available_cpu_mask = (gnb_cfg.expert_execution_cfg.affinities.isolated_cpus)
                                ? gnb_cfg.expert_execution_cfg.affinities.isolated_cpus.value()
                                : os_sched_affinity_bitmask::available_cpus();
  // Check the modified configuration.
  if (!validate_appconfig(gnb_cfg) || !o_cu_cp_app_unit->on_configuration_validation(available_cpu_mask) ||
      !o_cu_up_app_unit->on_configuration_validation(available_cpu_mask) ||
      !o_du_app_unit->on_configuration_validation(available_cpu_mask) ||
      !validate_plmn_and_tacs(o_du_app_unit->get_o_du_high_unit_config().du_high_cfg.config,
                              o_cu_cp_app_unit->get_o_cu_cp_unit_config().cucp_cfg)) {
    report_error("Invalid configuration detected.\n");
  }

  // Set up logging.
  initialize_log(gnb_cfg.log_cfg.filename);
  register_app_logs(gnb_cfg, *o_cu_cp_app_unit, *o_cu_up_app_unit, *o_du_app_unit);

  // Log input configuration.
  srslog::basic_logger& config_logger = srslog::fetch_basic_logger("CONFIG");
  if (config_logger.debug.enabled()) {
    YAML::Node node;
    fill_gnb_appconfig_in_yaml_schema(node, gnb_cfg);
    o_cu_cp_app_unit->dump_config(node);
    o_cu_up_app_unit->dump_config(node);
    o_du_app_unit->dump_config(node);
    config_logger.debug("Input configuration (all values): \n{}", YAML::Dump(node));
  } else {
    config_logger.info("Input configuration (only non-default values): \n{}", app.config_to_str(false, false));
  }

  srslog::basic_logger&            gnb_logger = srslog::fetch_basic_logger("GNB");
  app_services::application_tracer app_tracer;
  if (not gnb_cfg.log_cfg.tracing_filename.empty()) {
    app_tracer.enable_tracer(gnb_cfg.log_cfg.tracing_filename, gnb_logger);
  }

  app_services::core_isolation_manager core_isolation_mngr;
  if (gnb_cfg.expert_execution_cfg.affinities.isolated_cpus) {
    if (!core_isolation_mngr.isolate_cores(*gnb_cfg.expert_execution_cfg.affinities.isolated_cpus)) {
      report_error("Failed to isolate specified CPUs");
    }
  }

#ifdef DPDK_FOUND
  std::unique_ptr<dpdk::dpdk_eal> eal;
  if (gnb_cfg.hal_config) {
    // Prepend the application name in argv[0] as it is expected by EAL.
    eal = dpdk::create_dpdk_eal(std::string(argv[0]) + " " + gnb_cfg.hal_config->eal_args,
                                srslog::fetch_basic_logger("EAL", false));
  }
#endif

  // Buffer pool service.
  app_services::buffer_pool_manager buffer_pool_service(gnb_cfg.buffer_pool_config);

  // Log CPU architecture.
  cpu_architecture_info::get().print_cpu_info(gnb_logger);

  // Check and log included CPU features and check support by current CPU
  if (cpu_supports_included_features()) {
    gnb_logger.debug("Required CPU features: {}", get_cpu_feature_info());
  } else {
    // Quit here until we complete selection of the best matching implementation for the current CPU at runtime.
    gnb_logger.error("The CPU does not support the required CPU features that were configured during compile time: {}",
                     get_cpu_feature_info());
    report_error("The CPU does not support the required CPU features that were configured during compile time: {}\n",
                 get_cpu_feature_info());
  }

  // Check some common causes of performance issues and print a warning if required.
  check_cpu_governor(gnb_logger);
  check_drm_kms_polling(gnb_logger);

  // Setup application timers.
  timer_manager app_timers{1024};

  // Instantiate worker manager.
  worker_manager_config worker_manager_cfg;
  o_cu_cp_app_unit->fill_worker_manager_config(worker_manager_cfg);
  o_cu_up_app_unit->fill_worker_manager_config(worker_manager_cfg);
  o_du_app_unit->fill_worker_manager_config(worker_manager_cfg);
  fill_gnb_worker_manager_config(worker_manager_cfg, gnb_cfg);
  worker_manager_cfg.app_timers = &app_timers;

  worker_manager workers{worker_manager_cfg};

  // Set layer-specific pcap options.
  const auto& low_prio_cpu_mask = gnb_cfg.expert_execution_cfg.affinities.low_priority_cpu_cfg.mask;

  // Create IO broker.
  io_broker_config           io_broker_cfg(low_prio_cpu_mask);
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll, io_broker_cfg);

  // Create layer specific PCAPs.
  // In the gNB app, there is no point in instantiating two pcaps for each node of E1 and F1.
  // We disable one accordingly.
  o_cu_up_app_unit->get_o_cu_up_unit_config().cu_up_cfg.pcap_cfg.disable_e1_pcaps();
  o_du_app_unit->get_o_du_high_unit_config().du_high_cfg.config.pcaps.disable_f1_pcaps();
  o_cu_cp_dlt_pcaps cu_cp_dlt_pcaps = create_o_cu_cp_dlt_pcap(
      o_cu_cp_app_unit->get_o_cu_cp_unit_config(), *workers.get_executor_getter(), cleanup_signal_dispatcher);
  o_cu_up_dlt_pcaps cu_up_dlt_pcaps = create_o_cu_up_dlt_pcaps(
      o_cu_up_app_unit->get_o_cu_up_unit_config(), *workers.get_executor_getter(), cleanup_signal_dispatcher);
  flexible_o_du_pcaps du_pcaps =
      create_o_du_pcaps(o_du_app_unit->get_o_du_high_unit_config(), workers, cleanup_signal_dispatcher);

  std::unique_ptr<f1c_local_connector> f1c_gw =
      create_f1c_local_connector(f1c_local_connector_config{*cu_cp_dlt_pcaps.f1ap});
  std::unique_ptr<e1_local_connector> e1_gw =
      create_e1_local_connector(e1_local_connector_config{*cu_cp_dlt_pcaps.e1ap});

  // Create manager of timers for DU, CU-CP and CU-UP, which will be driven by the PHY slot ticks.
  timer_manager*                 cu_timers = &app_timers;
  std::unique_ptr<timer_manager> dummy_timers;
  if (o_du_app_unit->get_o_du_high_unit_config().du_high_cfg.config.is_testmode_enabled()) {
    // In case test mode is enabled, we pass dummy timers to the upper layers.
    dummy_timers = std::make_unique<timer_manager>(256);
    cu_timers    = dummy_timers.get();
  }

  // Create F1-U connector
  std::unique_ptr<f1u_local_connector> f1u_conn = std::make_unique<f1u_local_connector>();

  app_services::metrics_notifier_proxy_impl metrics_notifier_forwarder;

  // Create app-level resource usage service and metrics.
  auto app_resource_usage_service = app_services::build_app_resource_usage_service(
      metrics_notifier_forwarder, gnb_cfg.metrics_cfg.common_metrics_cfg);

  // Create service for periodically polling app resource usage.
  auto app_metrics_timer = app_timers.create_unique_timer(*workers.non_rt_low_prio_exec);
  std::vector<app_services::metrics_producer*> metric_producers;
  for (auto& metric : app_resource_usage_service.metrics) {
    for (auto& producer : metric.producers)
      metric_producers.push_back(producer.get());
  }
  app_services::periodic_metrics_report_controller periodic_metrics_controller(
      metric_producers,
      std::move(app_metrics_timer),
      std::chrono::milliseconds(gnb_cfg.metrics_cfg.rusage_report_period));

  std::vector<app_services::metrics_config> metrics_configs = std::move(app_resource_usage_service.metrics);

  // Instantiate E2AP client gateways.
  std::unique_ptr<e2_connection_client> e2_gw_du = create_e2_gateway_client(
      generate_e2_client_gateway_config(o_du_app_unit->get_o_du_high_unit_config().e2_cfg.base_cfg,
                                        *epoll_broker,
                                        *workers.non_rt_hi_prio_exec,
                                        *du_pcaps.e2ap,
                                        E2_DU_PPID));
  std::unique_ptr<e2_connection_client> e2_gw_cu_cp = create_e2_gateway_client(
      generate_e2_client_gateway_config(o_cu_cp_app_unit->get_o_cu_cp_unit_config().e2_cfg.base_config,
                                        *epoll_broker,
                                        *workers.non_rt_hi_prio_exec,
                                        *cu_cp_dlt_pcaps.e2ap,
                                        E2_CP_PPID));
  std::unique_ptr<e2_connection_client> e2_gw_cu_up = create_e2_gateway_client(
      generate_e2_client_gateway_config(o_cu_up_app_unit->get_o_cu_up_unit_config().e2_cfg.base_config,
                                        *epoll_broker,
                                        *workers.non_rt_hi_prio_exec,
                                        *cu_up_dlt_pcaps.e2ap,
                                        E2_UP_PPID));

  // Create O-CU-CP dependencies.
  o_cu_cp_unit_dependencies o_cucp_deps;
  o_cucp_deps.cu_cp_executor       = workers.cu_cp_exec;
  o_cucp_deps.cu_cp_n2_rx_executor = workers.non_rt_hi_prio_exec;
  o_cucp_deps.cu_cp_e2_exec        = workers.cu_e2_exec;
  o_cucp_deps.timers               = cu_timers;
  o_cucp_deps.ngap_pcap            = cu_cp_dlt_pcaps.ngap.get();
  o_cucp_deps.broker               = epoll_broker.get();
  o_cucp_deps.metrics_notifier     = &metrics_notifier_forwarder;
  o_cucp_deps.e2_gw                = e2_gw_cu_cp.get();

  // create O-CU-CP.
  auto                o_cucp_unit = o_cu_cp_app_unit->create_o_cu_cp(o_cucp_deps);
  srs_cu_cp::o_cu_cp& o_cucp_obj  = *o_cucp_unit.unit;
  for (auto& metric : o_cucp_unit.metrics) {
    metrics_configs.push_back(std::move(metric));
  }

  // Create CU-UP
  o_cu_up_unit_dependencies o_cuup_unit_deps;
  o_cuup_unit_deps.workers          = &workers;
  o_cuup_unit_deps.cu_up_e2_exec    = workers.cu_e2_exec;
  o_cuup_unit_deps.e1ap_conn_client = e1_gw.get();
  o_cuup_unit_deps.f1u_gateway      = f1u_conn->get_f1u_cu_up_gateway();
  o_cuup_unit_deps.gtpu_pcap        = cu_up_dlt_pcaps.n3.get();
  o_cuup_unit_deps.timers           = cu_timers;
  o_cuup_unit_deps.io_brk           = epoll_broker.get();
  o_cuup_unit_deps.e2_gw            = e2_gw_cu_up.get();
  o_cuup_unit_deps.metrics_notifier = &metrics_notifier_forwarder;

  auto o_cuup_obj = o_cu_up_app_unit->create_o_cu_up_unit(o_cuup_unit_deps);
  for (auto& metric : o_cuup_obj.metrics) {
    metrics_configs.push_back(std::move(metric));
  }
  // Instantiate DU.
  o_du_unit_dependencies odu_dependencies;
  odu_dependencies.workers            = &workers;
  odu_dependencies.f1c_client_handler = f1c_gw.get();
  odu_dependencies.f1u_gw             = f1u_conn->get_f1u_du_gateway();
  odu_dependencies.timer_mng          = &app_timers;
  odu_dependencies.mac_p              = du_pcaps.mac.get();
  odu_dependencies.rlc_p              = du_pcaps.rlc.get();
  odu_dependencies.e2_client_handler  = e2_gw_du.get();
  odu_dependencies.metrics_notifier   = &metrics_notifier_forwarder;

  auto du_inst_and_cmds = o_du_app_unit->create_flexible_o_du_unit(odu_dependencies);

  srs_du::du& du_inst = *du_inst_and_cmds.unit;

  for (auto& metric : du_inst_and_cmds.metrics) {
    metrics_configs.push_back(std::move(metric));
  }

  app_services::metrics_manager metrics_mngr(
      srslog::fetch_basic_logger("GNB"), *workers.metrics_hub_exec, metrics_configs);
  // Connect the forwarder to the metrics manager.
  metrics_notifier_forwarder.connect(metrics_mngr);

  std::vector<std::unique_ptr<app_services::cmdline_command>> commands;
  for (auto& cmd : o_cucp_unit.commands.cmdline) {
    commands.push_back(std::move(cmd));
  }
  for (auto& cmd : du_inst_and_cmds.commands.cmdline) {
    commands.push_back(std::move(cmd));
  }

  app_services::cmdline_command_dispatcher command_parser(*epoll_broker, *workers.non_rt_low_prio_exec, commands);

  // Connect E1AP to O-CU-CP.
  e1_gw->attach_cu_cp(o_cucp_obj.get_cu_cp().get_e1_handler());

  // Start O-CU-CP
  gnb_logger.info("Starting CU-CP...");
  o_cucp_obj.get_operation_controller().start();
  gnb_logger.info("CU-CP started successfully");

  if (not o_cucp_obj.get_cu_cp().get_ng_handler().amfs_are_connected()) {
    report_error("CU-CP failed to connect to AMF");
  }

  // Connect F1-C to O-CU-CP and start listening for new F1-C connection requests.
  f1c_gw->attach_cu_cp(o_cucp_obj.get_cu_cp().get_f1c_handler());

  o_cuup_obj.unit->get_operation_controller().start();

  // Start processing.
  du_inst.get_operation_controller().start();
  periodic_metrics_controller.start();

  std::unique_ptr<app_services::remote_server> remote_control_server =
      app_services::create_remote_server(gnb_cfg.remote_control_config, du_inst_and_cmds.commands.remote);

  {
    app_services::application_message_banners app_banner(app_name);

    while (is_app_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }

  periodic_metrics_controller.stop();

  if (remote_control_server) {
    remote_control_server->stop();
  }

  // Stop DU activity.
  du_inst.get_operation_controller().stop();

  // Stop O-CU-UP activity.
  o_cuup_obj.unit->get_operation_controller().stop();

  // Stop O-CU-CP activity.
  o_cucp_obj.get_operation_controller().stop();

  gnb_logger.info("Closing PCAP files...");
  cu_cp_dlt_pcaps.reset();
  cu_up_dlt_pcaps.reset();
  du_pcaps.reset();
  gnb_logger.info("PCAP files successfully closed.");

  gnb_logger.info("Stopping executors...");
  workers.stop();
  gnb_logger.info("Executors closed successfully.");

  srslog::flush();

  return 0;
}
