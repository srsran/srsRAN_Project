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

#include "srsran/support/backtrace.h"
#include "srsran/support/config_parsers.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/dynlink_manager.h"
#include "srsran/support/event_tracing.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/signal_handling.h"
#include "srsran/support/versioning/build_info.h"
#include "srsran/support/versioning/version.h"

#include "srsran/du/du_power_controller.h"
#include "srsran/e1ap/gateways/e1_local_connector_factory.h"
#include "srsran/f1ap/gateways/f1c_local_connector_factory.h"
#include "srsran/f1u/local_connector/f1u_local_connector.h"
#include "srsran/gtpu/ngu_gateway.h"
#include "srsran/ngap/gateways/n2_connection_client_factory.h"

#include "gnb_appconfig.h"
#include "gnb_appconfig_cli11_schema.h"
#include "gnb_appconfig_translators.h"
#include "gnb_appconfig_validators.h"
#include "gnb_appconfig_yaml_writer.h"

#include "apps/services/application_message_banners.h"
#include "apps/services/application_tracer.h"
#include "apps/services/buffer_pool/buffer_pool_manager.h"
#include "apps/services/core_isolation_manager.h"
#include "apps/services/e2_metric_connector_manager.h"
#include "apps/services/metrics/metrics_manager.h"
#include "apps/services/metrics/metrics_notifier_proxy.h"
#include "apps/services/stdin_command_dispatcher.h"
#include "apps/services/worker_manager.h"

#include "apps/gnb/adapters/e2_gateway_remote_connector.h"

// Include ThreadSanitizer (TSAN) options if thread sanitization is enabled.
// This include is not unused - it helps prevent false alarms from the thread sanitizer.
#include "srsran/support/tsan_options.h"

#include "apps/units/cu_cp/cu_cp_application_unit.h"
#include "apps/units/cu_cp/cu_cp_config_translators.h"
#include "apps/units/cu_cp/cu_cp_unit_config.h"
#include "apps/units/cu_cp/pcap_factory.h"
#include "apps/units/cu_up/cu_up_application_unit.h"
#include "apps/units/cu_up/cu_up_unit_config.h"
#include "apps/units/cu_up/pcap_factory.h"
#include "apps/units/flexible_du/du_high/du_high_config.h"
#include "apps/units/flexible_du/du_high/pcap_factory.h"
#include "apps/units/flexible_du/flexible_du_application_unit.h"

#include <atomic>
#include <yaml-cpp/node/convert.h>

#ifdef DPDK_FOUND
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#endif

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
  format_to(buffer, "srsRAN 5G gNB version {} ({})", get_version(), get_build_hash());
  app.set_version_flag("-v,--version", srsran::to_c_str(buffer));
  app.set_config("-c,", config_file, "Read config from file", false)->expected(1, MAX_CONFIG_FILES);
}

/// Function to call when the application is interrupted.
static void interrupt_signal_handler()
{
  is_app_running = false;
}

/// Function to call when the application is going to be forcefully shutdown.
static void cleanup_signal_handler()
{
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

static void register_app_logs(const logger_appconfig&       log_cfg,
                              cu_cp_application_unit&       cu_cp_app_unit,
                              cu_up_application_unit&       cu_up_app_unit,
                              flexible_du_application_unit& du_app_unit)
{
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

  auto& metrics_logger = srslog::fetch_basic_logger("METRICS", false);
  metrics_logger.set_level(log_cfg.metrics_level);
  metrics_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& e2ap_logger = srslog::fetch_basic_logger("E2AP", false);
  e2ap_logger.set_level(log_cfg.e2ap_level);
  e2ap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

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
      if (du_slices.end() == std::find(du_slices.begin(), du_slices.end(), slice.s_nssai)) {
        du_slices.push_back(slice.s_nssai);
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
  if (cu_up_cfg.upf_cfg.bind_addr == "auto") {
    cu_up_cfg.upf_cfg.bind_addr = cu_cp_cfg.amf_config.amf.bind_addr;
  }
  cu_up_cfg.upf_cfg.no_core = cu_cp_cfg.amf_config.no_core;
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

  auto cu_cp_app_unit = create_cu_cp_application_unit("gnb");
  cu_cp_app_unit->on_parsing_configuration_registration(app);

  auto cu_up_app_unit = create_cu_up_application_unit("gnb");
  cu_up_app_unit->on_parsing_configuration_registration(app);

  auto du_app_unit = create_flexible_du_application_unit("gnb");
  du_app_unit->on_parsing_configuration_registration(app);

  // Set the callback for the app calling all the autoderivation functions.
  app.callback([&app, &gnb_cfg, &du_app_unit, &cu_cp_app_unit, &cu_up_app_unit]() {
    autoderive_gnb_parameters_after_parsing(app, gnb_cfg);
    autoderive_slicing_args(du_app_unit->get_du_high_unit_config(), cu_cp_app_unit->get_cu_cp_unit_config());
    du_app_unit->on_configuration_parameters_autoderivation(app);

    // If test mode is enabled, we auto-enable "no_core" option and generate a amf config with no core.
    if (du_app_unit->get_du_high_unit_config().is_testmode_enabled()) {
      cu_cp_app_unit->get_cu_cp_unit_config().amf_config.no_core           = true;
      cu_cp_app_unit->get_cu_cp_unit_config().amf_config.amf.supported_tas = {{7, {{"00101", {s_nssai_t{1}}}}}};
    }

    cu_cp_app_unit->on_configuration_parameters_autoderivation(app);
    autoderive_cu_up_parameters_after_parsing(cu_up_app_unit->get_cu_up_unit_config(),
                                              cu_cp_app_unit->get_cu_cp_unit_config());
  });

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  auto available_cpu_mask = (gnb_cfg.expert_execution_cfg.affinities.isolated_cpus)
                                ? gnb_cfg.expert_execution_cfg.affinities.isolated_cpus.value()
                                : os_sched_affinity_bitmask::available_cpus();
  // Check the modified configuration.
  if (!validate_appconfig(gnb_cfg) || !cu_cp_app_unit->on_configuration_validation(available_cpu_mask) ||
      !cu_up_app_unit->on_configuration_validation(available_cpu_mask) ||
      !du_app_unit->on_configuration_validation(available_cpu_mask) ||
      !validate_plmn_and_tacs(du_app_unit->get_du_high_unit_config(), cu_cp_app_unit->get_cu_cp_unit_config())) {
    report_error("Invalid configuration detected.\n");
  }

  // Set up logging.
  initialize_log(gnb_cfg.log_cfg.filename);
  register_app_logs(gnb_cfg.log_cfg, *cu_cp_app_unit, *cu_up_app_unit, *du_app_unit);

  // Log input configuration.
  srslog::basic_logger& config_logger = srslog::fetch_basic_logger("CONFIG");
  if (config_logger.debug.enabled()) {
    YAML::Node node;
    fill_gnb_appconfig_in_yaml_schema(node, gnb_cfg);
    cu_cp_app_unit->dump_config(node);
    cu_up_app_unit->dump_config(node);
    du_app_unit->dump_config(node);
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

  // Instantiate worker manager.
  worker_manager_config worker_manager_cfg;
  fill_gnb_worker_manager_config(worker_manager_cfg, gnb_cfg);
  cu_cp_app_unit->fill_worker_manager_config(worker_manager_cfg);
  cu_up_app_unit->fill_worker_manager_config(worker_manager_cfg);
  du_app_unit->fill_worker_manager_config(worker_manager_cfg);

  worker_manager workers{worker_manager_cfg};

  // Set layer-specific pcap options.
  const auto& low_prio_cpu_mask = gnb_cfg.expert_execution_cfg.affinities.low_priority_cpu_cfg.mask;

  // Create IO broker.
  io_broker_config           io_broker_cfg(low_prio_cpu_mask);
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll, io_broker_cfg);

  // Create layer specific PCAPs.
  // In the gNB app, there is no point in instantiating two pcaps for each node of E1 and F1.
  // We disable one accordingly.
  cu_up_app_unit->get_cu_up_unit_config().pcap_cfg.disable_e1_pcaps();
  du_app_unit->get_du_high_unit_config().pcaps.disable_f1_pcaps();
  cu_cp_dlt_pcaps cu_cp_dlt_pcaps =
      create_cu_cp_dlt_pcap(cu_cp_app_unit->get_cu_cp_unit_config().pcap_cfg, *workers.get_executor_getter());
  cu_up_dlt_pcaps cu_up_dlt_pcaps =
      create_cu_up_dlt_pcaps(cu_up_app_unit->get_cu_up_unit_config().pcap_cfg, *workers.get_executor_getter());
  flexible_du_pcaps du_pcaps = create_du_pcaps(du_app_unit->get_du_high_unit_config().pcaps, workers);

  std::unique_ptr<f1c_local_connector> f1c_gw =
      create_f1c_local_connector(f1c_local_connector_config{*cu_cp_dlt_pcaps.f1ap});
  std::unique_ptr<e1_local_connector> e1_gw =
      create_e1_local_connector(e1_local_connector_config{*cu_cp_dlt_pcaps.e1ap});

  // Create manager of timers for DU, CU-CP and CU-UP, which will be driven by the PHY slot ticks.
  timer_manager                  app_timers{256};
  timer_manager*                 cu_timers = &app_timers;
  std::unique_ptr<timer_manager> dummy_timers;
  if (du_app_unit->get_du_high_unit_config().is_testmode_enabled()) {
    // In case test mode is enabled, we pass dummy timers to the upper layers.
    dummy_timers = std::make_unique<timer_manager>(256);
    cu_timers    = dummy_timers.get();
  }

  // Create F1-U connector
  std::unique_ptr<f1u_local_connector> f1u_conn = std::make_unique<f1u_local_connector>();

  // Set up the JSON log channel used by metrics.
  srslog::sink& json_sink =
      srslog::fetch_udp_sink(gnb_cfg.metrics_cfg.addr, gnb_cfg.metrics_cfg.port, srslog::create_json_formatter());

  e2_metric_connector_manager e2_metric_connectors(du_app_unit->get_du_high_unit_config().cells_cfg.size());

  // Load CU-CP plugins if enabled
  std::optional<dynlink_manager> ng_handover_plugin =
      cu_cp_app_unit->get_cu_cp_unit_config().load_plugins
          ? dynlink_manager::create("libsrsran_plugin_ng_handover.so", gnb_logger)
          : std::nullopt;
  std::optional<dynlink_manager> mocn_plugin = cu_cp_app_unit->get_cu_cp_unit_config().load_plugins
                                                   ? dynlink_manager::create("libsrsran_plugin_mocn.so", gnb_logger)
                                                   : std::nullopt;
  if (cu_cp_app_unit->get_cu_cp_unit_config().load_plugins) {
    if (not ng_handover_plugin) {
      gnb_logger.error("Could not open NG Handover plugin");
      return -1;
    }
    expected<void*> ng_ho_func = ng_handover_plugin->load_symbol("start_ngap_preparation_procedure_func");
    if (not ng_ho_func) {
      gnb_logger.error("Could not open NG Handover function pointer");
      return -1;
    }
    cu_cp_app_unit->get_cu_cp_unit_config().start_ng_ho_func = ng_ho_func.value();

    if (not mocn_plugin) {
      gnb_logger.error("Could not open MOCN plugin");
      return -1;
    }
    expected<void*> connect_amfs = mocn_plugin->load_symbol("connect_amfs_func");
    if (not connect_amfs) {
      gnb_logger.error("Could not open MOCN function pointer");
      return -1;
    }
    cu_cp_app_unit->get_cu_cp_unit_config().connect_amfs_func_ptr = connect_amfs.value();
    expected<void*> disconnect_amfs                               = mocn_plugin->load_symbol("disconnect_amfs_func");
    if (not disconnect_amfs) {
      gnb_logger.error("Could not open MOCN function pointer");
      return -1;
    }
    cu_cp_app_unit->get_cu_cp_unit_config().disconnect_amfs_func_ptr = disconnect_amfs.value();
  }

  // Create CU-CP dependencies.
  cu_cp_build_dependencies cu_cp_dependencies;
  cu_cp_dependencies.cu_cp_executor = workers.cu_cp_exec;
  cu_cp_dependencies.cu_cp_e2_exec  = workers.cu_cp_e2_exec;
  cu_cp_dependencies.timers         = cu_timers;
  cu_cp_dependencies.ngap_pcap      = cu_cp_dlt_pcaps.ngap.get();
  cu_cp_dependencies.broker         = epoll_broker.get();

  // create CU-CP.
  auto cu_cp_obj_and_cmds = cu_cp_app_unit->create_cu_cp(cu_cp_dependencies);

  srs_cu_cp::cu_cp& cu_cp_obj = *cu_cp_obj_and_cmds.unit;

  // Create and start CU-UP
  cu_up_unit_dependencies cu_up_unit_deps;
  cu_up_unit_deps.workers          = &workers;
  cu_up_unit_deps.e1ap_conn_client = e1_gw.get();
  cu_up_unit_deps.f1u_gateway      = f1u_conn->get_f1u_cu_up_gateway();
  cu_up_unit_deps.gtpu_pcap        = cu_up_dlt_pcaps.n3.get();
  cu_up_unit_deps.timers           = cu_timers;
  cu_up_unit_deps.io_brk           = epoll_broker.get();

  std::unique_ptr<srs_cu_up::cu_up_interface> cu_up_obj = cu_up_app_unit->create_cu_up_unit(cu_up_unit_deps);

  // E2AP configuration.
  sctp_network_connector_config e2_du_nw_config = generate_e2ap_nw_config(gnb_cfg, E2_DU_PPID);

  // Create E2AP GW remote connector.
  e2_gateway_remote_connector e2_gw{*epoll_broker, e2_du_nw_config, *du_pcaps.e2ap};

  // Instantiate one DU.
  app_services::metrics_notifier_proxy_impl metrics_notifier_forwarder;
  du_unit_dependencies                      du_dependencies;
  du_dependencies.workers              = &workers;
  du_dependencies.f1c_client_handler   = f1c_gw.get();
  du_dependencies.f1u_gw               = f1u_conn->get_f1u_du_gateway();
  du_dependencies.timer_mng            = &app_timers;
  du_dependencies.mac_p                = du_pcaps.mac.get();
  du_dependencies.rlc_p                = du_pcaps.rlc.get();
  du_dependencies.e2_client_handler    = &e2_gw;
  du_dependencies.e2_metric_connectors = &e2_metric_connectors;
  du_dependencies.json_sink            = &json_sink;
  du_dependencies.metrics_notifier     = &metrics_notifier_forwarder;

  auto du_inst_and_cmds = du_app_unit->create_flexible_du_unit(du_dependencies);

  srs_du::du& du_inst = *du_inst_and_cmds.unit;

  // Only DU has metrics now. When CU returns metrics, create the vector of metrics as it is done for the commands.
  app_services::metrics_manager metrics_mngr(
      srslog::fetch_basic_logger("GNB"), *workers.metrics_hub_exec, du_inst_and_cmds.metrics);
  // Connect the forwarder to the metrics manager.
  metrics_notifier_forwarder.connect(metrics_mngr);

  std::vector<std::unique_ptr<app_services::application_command>> commands;
  for (auto& cmd : cu_cp_obj_and_cmds.commands) {
    commands.push_back(std::move(cmd));
  }
  for (auto& cmd : du_inst_and_cmds.commands) {
    commands.push_back(std::move(cmd));
  }

  app_services::stdin_command_dispatcher command_parser(*epoll_broker, commands);

  // Connect E1AP to CU-CP.
  e1_gw->attach_cu_cp(cu_cp_obj.get_e1_handler());

  // start CU-CP
  gnb_logger.info("Starting CU-CP...");
  cu_cp_obj.start();
  gnb_logger.info("CU-CP started successfully");

  if (not cu_cp_obj.get_ng_handler().amfs_are_connected()) {
    report_error("CU-CP failed to connect to AMF");
  }

  // Connect F1-C to CU-CP and start listening for new F1-C connection requests.
  f1c_gw->attach_cu_cp(cu_cp_obj.get_f1c_handler());

  cu_up_obj->start();

  // Start processing.
  du_inst.get_power_controller().start();

  {
    app_services::application_message_banners app_banner(app_name);

    while (is_app_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }

  // Stop DU activity.
  du_inst.get_power_controller().stop();

  // Stop CU-UP activity.
  cu_up_obj->stop();

  // Stop CU-CP activity.
  cu_cp_obj.stop();

  if (gnb_cfg.e2_cfg.enable_du_e2) {
    gnb_logger.info("Closing E2 network connections...");
    e2_gw.close();
    gnb_logger.info("E2 Network connections closed successfully");
  }

  gnb_logger.info("Closing PCAP files...");
  cu_cp_dlt_pcaps.close();
  cu_up_dlt_pcaps.close();
  du_pcaps.close();
  gnb_logger.info("PCAP files successfully closed.");

  gnb_logger.info("Stopping executors...");
  workers.stop();
  gnb_logger.info("Executors closed successfully.");

  srslog::flush();

  return 0;
}
