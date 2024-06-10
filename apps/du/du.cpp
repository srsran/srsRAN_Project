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

#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/pcap/mac_pcap.h"
#include "srsran/support/build_info/build_info.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/event_tracing.h"
#include "srsran/support/signal_handler.h"
#include "srsran/support/version/version.h"

#include "srsran/f1u/du/split_connector/f1u_split_connector.h"
#include "srsran/gtpu/gtpu_demux_factory.h"

#include "srsran/support/io/io_broker_factory.h"

#include "adapters/f1_gateways.h"
#include "srsran/support/backtrace.h"
#include "srsran/support/config_parsers.h"

#include "du_appconfig.h"
#include "du_appconfig_cli11_schema.h"
#include "du_appconfig_translators.h"
#include "du_appconfig_validators.h"

#include "apps/services/worker_manager.h"

#include "apps/services/console_helper.h"
#include "apps/services/metrics_log_helper.h"
#include "apps/services/rlc_metrics_plotter_json.h"

#include "apps/units/flexible_du/split_dynamic/dynamic_du_factory.h"

#include "apps/gnb/adapters/e2_gateway_remote_connector.h"
#include "apps/services/e2_metric_connector_manager.h"
#include "srsran/support/sysinfo.h"

#include <atomic>

#include "../units/flexible_du/du_high/pcap_factory.h"
#include "apps/units/flexible_du/split_dynamic/dynamic_du_unit_cli11_schema.h"
#include "apps/units/flexible_du/split_dynamic/dynamic_du_unit_config_validator.h"
#include "apps/units/flexible_du/split_dynamic/dynamic_du_unit_logger_registrator.h"

#ifdef DPDK_FOUND
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#endif

using namespace srsran;

/// \file
/// \brief Application of a distributed unit (DU) that is split from the CU-CP and CU-UP via the F1 interface.
///
/// \cond

static std::string config_file;

static std::atomic<bool> is_running = {true};
const int                MAX_CONFIG_FILES(10);

static void populate_cli11_generic_args(CLI::App& app)
{
  fmt::memory_buffer buffer;
  format_to(buffer, "srsRAN 5G DU version {} ({})", get_version(), get_build_hash());
  app.set_version_flag("-v,--version", srsran::to_c_str(buffer));
  app.set_config("-c,", config_file, "Read config from file", false)->expected(1, MAX_CONFIG_FILES);
}

static void local_signal_handler()
{
  is_running = false;
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

static void register_app_logs(const log_appconfig& log_cfg, const dynamic_du_unit_config& du_loggers)
{
  // Set log-level of app and all non-layer specific components to app level.
  for (const auto& id : {"GNB", "ALL", "SCTP-GW", "IO-EPOLL", "UDP-GW", "PCAP"}) {
    auto& logger = srslog::fetch_basic_logger(id, false);
    logger.set_level(srslog::str_to_basic_level(log_cfg.lib_level));
    logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& config_logger = srslog::fetch_basic_logger("CONFIG", false);
  config_logger.set_level(srslog::str_to_basic_level(log_cfg.config_level));
  config_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& metrics_logger = srslog::fetch_basic_logger("METRICS", false);
  metrics_logger.set_level(srslog::str_to_basic_level(log_cfg.metrics_level));
  metrics_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  auto& e2ap_logger = srslog::fetch_basic_logger("E2AP", false);
  e2ap_logger.set_level(srslog::str_to_basic_level(log_cfg.e2ap_level));
  e2ap_logger.set_hex_dump_max_size(log_cfg.hex_max_size);

  // Register units logs.
  register_dynamic_du_loggers(du_loggers);
}

// TODO: Remove.
class null_command_handler : public srs_cu_cp::cu_cp_command_handler, public srs_cu_cp::cu_cp_mobility_command_handler
{
public:
  srs_cu_cp::cu_cp_mobility_command_handler& get_mobility_command_handler() override { return *this; }
  void trigger_handover(pci_t source_pci, rnti_t rnti, pci_t target_pci) override {}
};

int main(int argc, char** argv)
{
  // Set signal handler.
  register_signal_handler(local_signal_handler);

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

  dynamic_du_unit_config du_unit_cfg;
  configure_cli11_with_dynamic_du_unit_config_schema(app, du_unit_cfg);

  // Set the callback for the app calling all the autoderivation functions.
  app.callback([&app, &du_cfg, &du_unit_cfg]() {
    autoderive_du_parameters_after_parsing(app, du_cfg);
    autoderive_dynamic_du_parameters_after_parsing(app, du_unit_cfg);
  });

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  // Check the modified configuration.
  if (!validate_appconfig(du_cfg) ||
      !validate_dynamic_du_unit_config(du_unit_cfg,
                                       (du_cfg.expert_execution_cfg.affinities.isolated_cpus)
                                           ? du_cfg.expert_execution_cfg.affinities.isolated_cpus.value()
                                           : os_sched_affinity_bitmask::available_cpus())) {
    report_error("Invalid configuration detected.\n");
  }

  // Set up logging.
  initialize_log(du_cfg.log_cfg.filename);
  register_app_logs(du_cfg.log_cfg, du_unit_cfg);

  // Log input configuration.
  srslog::basic_logger& config_logger = srslog::fetch_basic_logger("CONFIG");
  if (config_logger.debug.enabled()) {
    config_logger.debug("Input configuration (all values): \n{}", app.config_to_str(true, false));
  } else {
    config_logger.info("Input configuration (only non-default values): \n{}", app.config_to_str(false, false));
  }

  srslog::basic_logger& du_logger = srslog::fetch_basic_logger("DU");
  if (not du_cfg.log_cfg.tracing_filename.empty()) {
    du_logger.info("Opening event tracer...");
    open_trace_file(du_cfg.log_cfg.tracing_filename);
    du_logger.info("Event tracer opened successfully");
  }

  if (du_cfg.expert_execution_cfg.affinities.isolated_cpus) {
    if (!configure_cgroups(*du_cfg.expert_execution_cfg.affinities.isolated_cpus)) {
      report_error("Failed to isolate specified CPUs");
    }
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
  init_byte_buffer_segment_pool(du_cfg.buffer_pool_config.nof_segments, du_cfg.buffer_pool_config.segment_size);

  // Log build info
  du_logger.info("Built in {} mode using {}", get_build_mode(), get_build_info());

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

  worker_manager workers{du_unit_cfg, du_cfg.expert_execution_cfg, du_cfg.pcap_cfg, du_cfg.f1u_cfg.pdu_queue_size};

  // Set layer-specific pcap options.
  const auto& low_prio_cpu_mask = du_cfg.expert_execution_cfg.affinities.low_priority_cpu_cfg.mask;

  // Create IO broker.
  io_broker_config           io_broker_cfg(low_prio_cpu_mask);
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll, io_broker_cfg);

  std::unique_ptr<dlt_pcap> f1ap_p =
      modules::flexible_du::create_dlt_pcap(du_unit_cfg.du_high_cfg.config.pcaps, workers);
  std::unique_ptr<mac_pcap> mac_p =
      modules::flexible_du::create_mac_pcap(du_unit_cfg.du_high_cfg.config.pcaps, workers);
  std::unique_ptr<rlc_pcap> rlc_p =
      modules::flexible_du::create_rlc_pcap(du_unit_cfg.du_high_cfg.config.pcaps, workers);
  // TODO: Remove GTPU pcap
  std::unique_ptr<dlt_pcap> gtpu_p =
      modules::flexible_du::create_dlt_pcap(du_unit_cfg.du_high_cfg.config.pcaps, workers);
  std::unique_ptr<dlt_pcap> e2ap_p =
      du_cfg.pcap_cfg.e2ap.enabled ? create_e2ap_pcap(du_cfg.pcap_cfg.e2ap.filename, workers.get_executor("pcap_exec"))
                                   : create_null_dlt_pcap();

  // Instantiate F1-C client gateway.
  std::unique_ptr<srs_du::f1c_connection_client> f1c_gw =
      create_f1c_client_gateway(du_cfg.f1c_cfg.cu_cp_address, du_cfg.f1c_cfg.bind_address, *epoll_broker, *f1ap_p);

  // Create manager of timers for DU, which will be driven by the PHY slot ticks.
  timer_manager app_timers{256};

  // Create F1-U connector
  // TODO: Simplify this and use factory.
  gtpu_demux_creation_request du_f1u_gtpu_msg       = {};
  du_f1u_gtpu_msg.cfg.warn_on_drop                  = true;
  du_f1u_gtpu_msg.gtpu_pcap                         = gtpu_p.get();
  std::unique_ptr<gtpu_demux> du_f1u_gtpu_demux     = create_gtpu_demux(du_f1u_gtpu_msg);
  udp_network_gateway_config  du_f1u_gw_config      = {};
  du_f1u_gw_config.bind_address                     = du_cfg.f1u_cfg.bind_address;
  du_f1u_gw_config.bind_port                        = GTPU_PORT;
  du_f1u_gw_config.reuse_addr                       = true;
  std::unique_ptr<srs_cu_up::ngu_gateway> du_f1u_gw = srs_cu_up::create_udp_ngu_gateway(
      du_f1u_gw_config,
      *epoll_broker,
      workers.get_du_high_executor_mapper(0).ue_mapper().mac_ul_pdu_executor(to_du_ue_index(0)));
  std::unique_ptr<srs_du::f1u_split_connector> du_f1u_conn =
      std::make_unique<srs_du::f1u_split_connector>(du_f1u_gw.get(), du_f1u_gtpu_demux.get(), *gtpu_p);

  // Set up the JSON log channel used by metrics.
  srslog::sink& json_sink =
      srslog::fetch_udp_sink(du_cfg.metrics_cfg.addr, du_cfg.metrics_cfg.port, srslog::create_json_formatter());
  srslog::log_channel& json_channel = srslog::fetch_log_channel("JSON_channel", json_sink, {});
  json_channel.set_enabled(du_cfg.metrics_cfg.enable_json_metrics);

  // Set up RLC JSON log channel used by metrics.
  srslog::log_channel& rlc_json_channel = srslog::fetch_log_channel("JSON_RLC_channel", json_sink, {});
  rlc_json_channel.set_enabled(du_unit_cfg.du_high_cfg.config.metrics.rlc.json_enabled);
  rlc_metrics_plotter_json rlc_json_plotter(rlc_json_channel);

  std::unique_ptr<metrics_hub> hub = std::make_unique<metrics_hub>(*workers.metrics_hub_exec);
  e2_metric_connector_manager  e2_metric_connectors(du_unit_cfg.du_high_cfg.config.cells_cfg.size());

  // E2AP configuration.
  srsran::sctp_network_connector_config e2_du_nw_config = generate_e2ap_nw_config(du_cfg, E2_DU_PPID);

  // Create E2AP GW remote connector.
  e2_gateway_remote_connector e2_gw{*epoll_broker, e2_du_nw_config, *e2ap_p};

  // Create console helper object for commands and metrics printing.
  // TODO: Remove cu_cp handler dependency.
  null_command_handler cmd_handler;
  console_helper       console(*epoll_broker, json_channel, cmd_handler, du_cfg.metrics_cfg.autostart_stdout_metrics);
  console.on_app_starting();

  // Create metrics log helper.
  metrics_log_helper metrics_logger(srslog::fetch_basic_logger("METRICS"));

  // Instantiate one DU.
  std::unique_ptr<du> du_inst = create_du(du_unit_cfg,
                                          workers,
                                          *f1c_gw,
                                          *du_f1u_conn,
                                          app_timers,
                                          *mac_p,
                                          *rlc_p,
                                          console,
                                          metrics_logger,
                                          e2_gw,
                                          e2_metric_connectors,
                                          rlc_json_plotter,
                                          *hub);

  // Move all the DLT PCAPs to a container.
  std::vector<std::unique_ptr<dlt_pcap>> dlt_pcaps;
  dlt_pcaps.push_back(std::move(f1ap_p));
  dlt_pcaps.push_back(std::move(e2ap_p));

  // Start processing.
  du_inst->start();
  console.on_app_running();

  while (is_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  console.on_app_stopping();

  // Stop DU activity.
  du_inst->stop();

  if (du_cfg.e2_cfg.enable_du_e2) {
    du_logger.info("Closing E2 network connections...");
    e2_gw.close();
    du_logger.info("E2 Network connections closed successfully");
  }

  du_logger.info("Closing PCAP files...");
  mac_p->close();
  rlc_p->close();
  for (auto& pcap : dlt_pcaps) {
    pcap->close();
  }
  du_logger.info("PCAP files successfully closed.");

  du_logger.info("Stopping executors...");
  workers.stop();
  du_logger.info("Executors closed successfully.");

  srslog::flush();

  if (not du_cfg.log_cfg.tracing_filename.empty()) {
    du_logger.info("Closing event tracer...");
    close_trace_file();
    du_logger.info("Event tracer closed successfully");
  }

  if (du_cfg.expert_execution_cfg.affinities.isolated_cpus) {
    cleanup_cgroups();
  }

  return 0;
}
