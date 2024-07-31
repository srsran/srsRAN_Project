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

#include "srsran/support/build_info/build_info.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/event_tracing.h"
#include "srsran/support/signal_handling.h"
#include "srsran/support/version/version.h"

#include "srsran/f1u/local_connector/f1u_local_connector.h"

#include "srsran/ngap/gateways/n2_connection_client_factory.h"
#include "srsran/support/io/io_broker_factory.h"

#include "srsran/e1ap/gateways/e1_local_connector_factory.h"
#include "srsran/f1ap/gateways/f1c_local_connector_factory.h"
#include "srsran/gtpu/ngu_gateway.h"
#include "srsran/support/backtrace.h"
#include "srsran/support/config_parsers.h"

#include "gnb_appconfig.h"
#include "gnb_appconfig_cli11_schema.h"
#include "gnb_appconfig_translators.h"
#include "gnb_appconfig_validators.h"

#include "apps/services/worker_manager.h"

#include "apps/services/application_tracer.h"
#include "apps/services/metrics_hub.h"
#include "apps/services/metrics_log_helper.h"
#include "apps/services/rlc_metrics_plotter_json.h"
#include "apps/services/stdin_command_dispatcher.h"

#include "apps/units/flexible_du/split_dynamic/dynamic_du_factory.h"

#include "apps/gnb/adapters/e2_gateway_remote_connector.h"
#include "apps/services/e2_metric_connector_manager.h"

// Include ThreadSanitizer (TSAN) options if thread sanitization is enabled.
// This include is not unused - it helps prevent false alarms from the thread sanitizer.
#include "gnb_appconfig_yaml_writer.h"

#include "srsran/support/tsan_options.h"

#include "apps/units/cu_cp/cu_cp_config_translators.h"
#include "apps/units/cu_cp/cu_cp_logger_registrator.h"
#include "apps/units/cu_cp/cu_cp_unit_config_cli11_schema.h"
#include "apps/units/cu_cp/cu_cp_unit_config_validator.h"
#include "apps/units/cu_up/cu_up_logger_registrator.h"
#include "apps/units/cu_up/cu_up_unit_config_cli11_schema.h"
#include "apps/units/cu_up/cu_up_unit_config_validator.h"

#include <atomic>

#include "apps/services/application_message_banners.h"
#include "apps/services/buffer_pool/buffer_pool_manager.h"
#include "apps/services/core_isolation_manager.h"
#include "apps/services/metrics_plotter_json.h"
#include "apps/services/metrics_plotter_stdout.h"
#include "apps/units/cu_cp/cu_cp_builder.h"
#include "apps/units/cu_cp/cu_cp_unit_config_yaml_writer.h"
#include "apps/units/cu_cp/pcap_factory.h"
#include "apps/units/cu_up/cu_up_builder.h"
#include "apps/units/cu_up/cu_up_unit_config_yaml_writer.h"
#include "apps/units/cu_up/pcap_factory.h"
#include "apps/units/flexible_du/du_high/pcap_factory.h"
#include "apps/units/flexible_du/split_dynamic/dynamic_du_unit_cli11_schema.h"
#include "apps/units/flexible_du/split_dynamic/dynamic_du_unit_config_validator.h"
#include "apps/units/flexible_du/split_dynamic/dynamic_du_unit_config_yaml_writer.h"
#include "apps/units/flexible_du/split_dynamic/dynamic_du_unit_logger_registrator.h"
#include "srsran/support/cli11_utils.h"

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

static void register_app_logs(const logger_appconfig&         log_cfg,
                              const cu_cp_unit_logger_config& cu_cp_loggers,
                              const cu_up_unit_logger_config& cu_up_loggers,
                              const dynamic_du_unit_config&   du_loggers)
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
  register_cu_cp_loggers(cu_cp_loggers);
  register_cu_up_loggers(cu_up_loggers);
  register_dynamic_du_loggers(du_loggers);
}

static void autoderive_slicing_args(dynamic_du_unit_config& du_unit_cfg, cu_cp_unit_config& cu_cp_config)
{
  std::vector<s_nssai_t> du_slices;
  for (const auto& cell_cfg : du_unit_cfg.du_high_cfg.config.cells_cfg) {
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

  cu_cp_unit_config cu_cp_config;
  cu_cp_config.pcap_cfg.set_default_filename("/tmp/gnb");
  configure_cli11_with_cu_cp_unit_config_schema(app, cu_cp_config);

  cu_up_unit_config cu_up_config;
  cu_up_config.pcap_cfg.set_default_filename("/tmp/gnb");
  configure_cli11_with_cu_up_unit_config_schema(app, cu_up_config);

  dynamic_du_unit_config du_unit_cfg;
  du_unit_cfg.du_high_cfg.config.pcaps.set_default_filename("/tmp/gnb");
  configure_cli11_with_dynamic_du_unit_config_schema(app, du_unit_cfg);

  // Set the callback for the app calling all the autoderivation functions.
  app.callback([&app, &gnb_cfg, &du_unit_cfg, &cu_cp_config]() {
    autoderive_gnb_parameters_after_parsing(app, gnb_cfg);
    autoderive_slicing_args(du_unit_cfg, cu_cp_config);
    autoderive_dynamic_du_parameters_after_parsing(app, du_unit_cfg);

    // Create the PLMN and TAC list from the cells.
    std::vector<std::string> plmns;
    std::vector<unsigned>    tacs;
    for (const auto& cell : du_unit_cfg.du_high_cfg.config.cells_cfg) {
      plmns.push_back(cell.cell.plmn);
      tacs.push_back(cell.cell.tac);
    }

    // If test mode is enabled, we auto-enable "no_core" option
    if (du_unit_cfg.du_high_cfg.config.test_mode_cfg.test_ue.rnti != rnti_t::INVALID_RNTI) {
      cu_cp_config.amf_cfg.no_core = true;
    }

    autoderive_cu_cp_parameters_after_parsing(app, cu_cp_config, std::move(plmns), std::move(tacs));
  });

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  // Check the modified configuration.
  if (!validate_appconfig(gnb_cfg) || !validate_cu_cp_unit_config(cu_cp_config) ||
      !validate_cu_up_unit_config(cu_up_config) ||
      !validate_dynamic_du_unit_config(du_unit_cfg,
                                       (gnb_cfg.expert_execution_cfg.affinities.isolated_cpus)
                                           ? gnb_cfg.expert_execution_cfg.affinities.isolated_cpus.value()
                                           : os_sched_affinity_bitmask::available_cpus()) ||
      !validate_plmn_and_tacs(du_unit_cfg.du_high_cfg.config, cu_cp_config)) {
    report_error("Invalid configuration detected.\n");
  }

  // Set up logging.
  initialize_log(gnb_cfg.log_cfg.filename);
  register_app_logs(gnb_cfg.log_cfg, cu_cp_config.loggers, cu_up_config.loggers, du_unit_cfg);

  // Log input configuration.
  srslog::basic_logger& config_logger = srslog::fetch_basic_logger("CONFIG");
  if (config_logger.debug.enabled()) {
    YAML::Node node;
    fill_gnb_appconfig_in_yaml_schema(node, gnb_cfg);
    fill_cu_up_config_in_yaml_schema(node, cu_up_config);
    fill_cu_cp_config_in_yaml_schema(node, cu_cp_config);
    fill_dynamic_du_unit_config_in_yaml_schema(node, du_unit_cfg);
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

  worker_manager workers{du_unit_cfg,
                         gnb_cfg.expert_execution_cfg,
                         cu_cp_config.pcap_cfg,
                         cu_up_config.pcap_cfg,
                         cu_up_config.gtpu_queue_size};

  // Set layer-specific pcap options.
  const auto& low_prio_cpu_mask = gnb_cfg.expert_execution_cfg.affinities.low_priority_cpu_cfg.mask;

  // Create IO broker.
  io_broker_config           io_broker_cfg(low_prio_cpu_mask);
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll, io_broker_cfg);

  // Create layer specific PCAPs.
  // In the gNB app, there is no point in instantiating two pcaps for each node of E1 and F1.
  // We disable one accordingly.
  cu_up_config.pcap_cfg.disable_e1_pcaps();
  du_unit_cfg.du_high_cfg.config.pcaps.disable_f1_pcaps();
  srsran::modules::cu_cp::cu_cp_dlt_pcaps cu_cp_dlt_pcaps =
      modules::cu_cp::create_dlt_pcap(cu_cp_config.pcap_cfg, *workers.get_executor_getter());
  srsran::modules::cu_up::cu_up_dlt_pcaps cu_up_dlt_pcaps =
      modules::cu_up::create_dlt_pcaps(cu_up_config.pcap_cfg, *workers.get_executor_getter());
  srsran::modules::flexible_du::du_pcaps du_pcaps =
      modules::flexible_du::create_pcaps(du_unit_cfg.du_high_cfg.config.pcaps, workers);

  std::unique_ptr<f1c_local_connector> f1c_gw =
      create_f1c_local_connector(f1c_local_connector_config{*cu_cp_dlt_pcaps.f1ap});
  std::unique_ptr<e1_local_connector> e1_gw =
      create_e1_local_connector(e1_local_connector_config{*cu_cp_dlt_pcaps.e1ap});

  // Create manager of timers for DU, CU-CP and CU-UP, which will be driven by the PHY slot ticks.
  timer_manager                  app_timers{256};
  timer_manager*                 cu_timers = &app_timers;
  std::unique_ptr<timer_manager> dummy_timers;
  if (du_unit_cfg.du_high_cfg.config.test_mode_cfg.test_ue.rnti != rnti_t::INVALID_RNTI) {
    // In case test mode is enabled, we pass dummy timers to the upper layers.
    dummy_timers = std::make_unique<timer_manager>(256);
    cu_timers    = dummy_timers.get();
  }

  // Create F1-U connector
  std::unique_ptr<f1u_local_connector> f1u_conn = std::make_unique<f1u_local_connector>();

  // Set up the JSON log channel used by metrics.
  srslog::sink& json_sink =
      srslog::fetch_udp_sink(gnb_cfg.metrics_cfg.addr, gnb_cfg.metrics_cfg.port, srslog::create_json_formatter());
  srslog::log_channel& json_channel = srslog::fetch_log_channel("JSON_channel", json_sink, {});
  json_channel.set_enabled(gnb_cfg.metrics_cfg.enable_json_metrics);

  // Set up RLC JSON log channel used by metrics.
  srslog::log_channel& rlc_json_channel = srslog::fetch_log_channel("JSON_RLC_channel", json_sink, {});
  rlc_json_channel.set_enabled(du_unit_cfg.du_high_cfg.config.metrics.rlc.json_enabled);
  rlc_metrics_plotter_json rlc_json_plotter(rlc_json_channel);

  std::unique_ptr<metrics_hub> hub = std::make_unique<metrics_hub>(*workers.metrics_hub_exec);
  e2_metric_connector_manager  e2_metric_connectors(du_unit_cfg.du_high_cfg.config.cells_cfg.size());

  // Create N2 Gateway.
  std::unique_ptr<srs_cu_cp::n2_connection_client> n2_client = srs_cu_cp::create_n2_connection_client(
      generate_n2_client_config(cu_cp_config.amf_cfg, *cu_cp_dlt_pcaps.ngap, *epoll_broker));

  // E2AP configuration.
  srsran::sctp_network_connector_config e2_du_nw_config = generate_e2ap_nw_config(gnb_cfg, E2_DU_PPID);

  // Create E2AP GW remote connector.
  e2_gateway_remote_connector e2_gw{*epoll_broker, e2_du_nw_config, *du_pcaps.e2ap};

  // Create CU-CP config.
  cu_cp_build_dependencies cu_cp_dependencies;
  cu_cp_dependencies.cu_cp_executor = workers.cu_cp_exec;
  cu_cp_dependencies.cu_cp_e2_exec  = workers.cu_cp_e2_exec;
  cu_cp_dependencies.n2_client      = n2_client.get();
  cu_cp_dependencies.timers         = cu_timers;

  // create CU-CP.
  auto cu_cp_obj_and_cmds = build_cu_cp(cu_cp_config, cu_cp_dependencies);

  srs_cu_cp::cu_cp& cu_cp_obj = *cu_cp_obj_and_cmds.unit;

  // Create metrics log helper.
  metrics_log_helper metrics_logger(srslog::fetch_basic_logger("METRICS"));

  // Connect E1AP to CU-CP.
  e1_gw->attach_cu_cp(cu_cp_obj.get_e1_handler());

  // start CU-CP
  gnb_logger.info("Starting CU-CP...");
  cu_cp_obj.start();
  gnb_logger.info("CU-CP started successfully");

  if (not cu_cp_obj.get_ng_handler().amf_is_connected()) {
    report_error("CU-CP failed to connect to AMF");
  }

  // Connect F1-C to CU-CP and start listening for new F1-C connection requests.
  f1c_gw->attach_cu_cp(cu_cp_obj.get_f1c_handler());

  // Create and start CU-UP
  std::unique_ptr<srs_cu_up::cu_up_interface> cu_up_obj = build_cu_up(cu_up_config,
                                                                      workers,
                                                                      *e1_gw,
                                                                      *f1u_conn->get_f1u_cu_up_gateway(),
                                                                      *cu_up_dlt_pcaps.n3,
                                                                      *cu_timers,
                                                                      *epoll_broker);
  cu_up_obj->start();

  // Instantiate one DU.
  metrics_plotter_stdout metrics_stdout(gnb_cfg.metrics_cfg.autostart_stdout_metrics);
  metrics_plotter_json   metrics_json(json_channel);
  auto                   du_inst_and_cmds = create_du(du_unit_cfg,
                                    workers,
                                    *f1c_gw,
                                    *f1u_conn->get_f1u_du_gateway(),
                                    app_timers,
                                    *du_pcaps.mac,
                                    *du_pcaps.rlc,
                                    metrics_stdout,
                                    metrics_json,
                                    metrics_logger,
                                    e2_gw,
                                    e2_metric_connectors,
                                    rlc_json_plotter,
                                    *hub);

  du& du_inst = *du_inst_and_cmds.unit;

  std::vector<std::unique_ptr<app_services::application_command>> commands;
  for (auto& cmd : cu_cp_obj_and_cmds.commands) {
    commands.push_back(std::move(cmd));
  }
  for (auto& cmd : du_inst_and_cmds.commands) {
    commands.push_back(std::move(cmd));
  }

  app_services::stdin_command_dispatcher command_parser(*epoll_broker, commands);

  // Start processing.
  du_inst.start();

  {
    app_services::application_message_banners app_banner(app_name);

    while (is_app_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }

  // Stop DU activity.
  du_inst.stop();

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
