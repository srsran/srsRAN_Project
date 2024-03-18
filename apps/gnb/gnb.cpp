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
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/support/build_info/build_info.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/event_tracing.h"
#include "srsran/support/signal_handler.h"
#include "srsran/support/tsan_options.h"
#include "srsran/support/version/version.h"

#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_factory.h"

#include "srsran/cu_up/cu_up_factory.h"
#include "srsran/f1u/local_connector/f1u_local_connector.h"

#include "adapters/ngap_adapter.h"
#include "srsran/support/io/io_broker_factory.h"

#include "adapters/e1ap_gateway_local_connector.h"
#include "adapters/f1c_gateway_local_connector.h"
#include "srsran/support/backtrace.h"
#include "srsran/support/config_parsers.h"

#include "gnb_appconfig.h"
#include "gnb_appconfig_cli11_schema.h"
#include "gnb_appconfig_translators.h"
#include "gnb_appconfig_validators.h"

#include "gnb_worker_manager.h"

#include "helpers/gnb_console_helper.h"
#include "helpers/metrics_hub.h"
#include "helpers/rlc_metrics_plotter_json.h"

#include "gnb_du_factory.h"
#include "srsran/phy/upper/upper_phy_timing_notifier.h"

#include "srsran/ru/ru_adapters.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/ru/ru_dummy_factory.h"
#include "srsran/ru/ru_generic_factory.h"
#include "srsran/ru/ru_ofh_factory.h"

#include "apps/gnb/adapters/e2_gateway_remote_connector.h"
#include "gnb_e2_metric_connector_manager.h"
#include "srsran/support/sysinfo.h"

#include <atomic>

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
///
/// \cond

static std::string config_file;

static std::atomic<bool> is_running = {true};
const int                MAX_CONFIG_FILES(10);

static void populate_cli11_generic_args(CLI::App& app)
{
  fmt::memory_buffer buffer;
  format_to(buffer, "srsRAN 5G gNB version {} ({})", get_version(), get_build_hash());
  app.set_version_flag("-v,--version", srsran::to_c_str(buffer));
  app.set_config("-c,", config_file, "Read config from file", false)->expected(1, MAX_CONFIG_FILES);
}

static void local_signal_handler()
{
  is_running = false;
}

/// Resolves the generic Radio Unit dependencies and adds them to the configuration.
static void configure_ru_generic_executors_and_notifiers(ru_generic_configuration&           config,
                                                         const log_appconfig&                log_cfg,
                                                         worker_manager&                     workers,
                                                         ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                         ru_timing_notifier&                 timing_notifier)
{
  srslog::basic_logger& rf_logger = srslog::fetch_basic_logger("RF", false);
  rf_logger.set_level(srslog::str_to_basic_level(log_cfg.radio_level));

  config.rf_logger                   = &rf_logger;
  config.radio_exec                  = workers.radio_exec;
  config.statistics_printer_executor = workers.ru_printer_exec;
  config.timing_notifier             = &timing_notifier;
  config.symbol_notifier             = &symbol_notifier;

  for (unsigned i = 0, e = config.lower_phy_config.size(); i != e; ++i) {
    lower_phy_configuration& low_phy_cfg = config.lower_phy_config[i];
    low_phy_cfg.logger                   = &srslog::fetch_basic_logger("Low-PHY#" + std::to_string(i));
    low_phy_cfg.tx_task_executor         = workers.lower_phy_tx_exec[i];
    low_phy_cfg.rx_task_executor         = workers.lower_phy_rx_exec[i];
    low_phy_cfg.dl_task_executor         = workers.lower_phy_dl_exec[i];
    low_phy_cfg.ul_task_executor         = workers.lower_phy_ul_exec[i];
    low_phy_cfg.prach_async_executor     = workers.lower_prach_exec[i];

    low_phy_cfg.logger->set_level(srslog::str_to_basic_level(log_cfg.phy_level));
  }
}

/// Resolves the Open Fronthaul Radio Unit dependencies and adds them to the configuration.
static void configure_ru_ofh_executors_and_notifiers(ru_ofh_configuration&               config,
                                                     ru_ofh_dependencies&                dependencies,
                                                     const log_appconfig&                log_cfg,
                                                     worker_manager&                     workers,
                                                     ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                     ru_timing_notifier&                 timing_notifier,
                                                     ru_error_notifier&                  error_notifier)
{
  srslog::basic_logger& ofh_logger = srslog::fetch_basic_logger("OFH", false);
  ofh_logger.set_level(srslog::str_to_basic_level(log_cfg.ofh_level));

  dependencies.logger             = &ofh_logger;
  dependencies.rt_timing_executor = workers.ru_timing_exec;
  dependencies.timing_notifier    = &timing_notifier;
  dependencies.rx_symbol_notifier = &symbol_notifier;
  dependencies.error_notifier     = &error_notifier;

  // Configure sector.
  for (unsigned i = 0, e = config.sector_configs.size(); i != e; ++i) {
    dependencies.sector_dependencies.emplace_back();
    ru_ofh_sector_dependencies& sector_deps = dependencies.sector_dependencies.back();
    sector_deps.logger                      = dependencies.logger;
    sector_deps.receiver_executor           = workers.ru_rx_exec[i];
    sector_deps.transmitter_executor        = workers.ru_tx_exec[i];
    sector_deps.downlink_executor           = workers.ru_dl_exec[i];
  }
}

/// Resolves the Dummy Radio Unit dependencies and adds them to the configuration.
static void configure_ru_dummy_executors_and_notifiers(ru_dummy_configuration&             config,
                                                       ru_dummy_dependencies&              dependencies,
                                                       const log_appconfig&                log_cfg,
                                                       worker_manager&                     workers,
                                                       ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                       ru_timing_notifier&                 timing_notifier)
{
  srslog::basic_logger& ru_logger = srslog::fetch_basic_logger("RU", true);
  ru_logger.set_level(srslog::str_to_basic_level(log_cfg.radio_level));

  dependencies.logger          = &ru_logger;
  dependencies.executor        = workers.radio_exec;
  dependencies.timing_notifier = &timing_notifier;
  dependencies.symbol_notifier = &symbol_notifier;
}

int main(int argc, char** argv)
{
  // Set signal handler.
  register_signal_handler(local_signal_handler);

  // Enable backtrace.
  enable_backtrace();

  // Setup and configure config parsing.
  CLI::App app("srsGNB application");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  // Fill the generic application arguments to parse.
  populate_cli11_generic_args(app);

  gnb_parsed_appconfig gnb_parsed_cfg;
  // Configure CLI11 with the gNB application configuration schema.
  configure_cli11_with_gnb_appconfig_schema(app, gnb_parsed_cfg);

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  gnb_appconfig& gnb_cfg = gnb_parsed_cfg.config;

  // Derive the parameters that were set to be derived automatically.
  derive_auto_params(gnb_cfg);

  // Check the modified configuration.
  if (!validate_appconfig(gnb_cfg)) {
    report_error("Invalid configuration detected.\n");
  }

  // Set up logging.
  srslog::sink* log_sink = (gnb_cfg.log_cfg.filename == "stdout") ? srslog::create_stdout_sink()
                                                                  : srslog::create_file_sink(gnb_cfg.log_cfg.filename);
  if (log_sink == nullptr) {
    report_error("Could not create application main log sink.\n");
  }
  srslog::set_default_sink(*log_sink);
  srslog::init();

  // Set log-level of app and all non-layer specific components to app level.
  srslog::basic_logger& gnb_logger = srslog::fetch_basic_logger("GNB");
  for (const auto& id : {"GNB", "ALL", "SCTP-GW", "IO-EPOLL", "UDP-GW", "PCAP"}) {
    auto& logger = srslog::fetch_basic_logger(id, false);
    logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.lib_level));
    logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  // Set component-specific logging options.
  for (const auto& id : {"DU", "DU-MNG", "UE-MNG"}) {
    auto& du_logger = srslog::fetch_basic_logger(id, false);
    du_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.du_level));
    du_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  for (const auto& id : {"CU-CP", "CU-UEMNG", "CU-CP-E1"}) {
    auto& cu_cp_logger = srslog::fetch_basic_logger(id, false);
    cu_cp_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.cu_level));
    cu_cp_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  for (const auto& id : {"CU-UP", "CU-UP-E1"}) {
    auto& cu_up_logger = srslog::fetch_basic_logger(id, false);
    cu_up_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.cu_level));
    cu_up_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  // Set layer-specific logging options.
  auto& phy_logger = srslog::fetch_basic_logger("PHY", true);
  phy_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.phy_level));
  phy_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  for (const auto& id : {"MAC", "SCHED"}) {
    auto& mac_logger = srslog::fetch_basic_logger(id, true);
    mac_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.mac_level));
    mac_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  auto& rlc_logger = srslog::fetch_basic_logger("RLC", false);
  rlc_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.rlc_level));
  rlc_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& du_f1ap_logger = srslog::fetch_basic_logger("DU-F1", false);
  auto& cu_f1ap_logger = srslog::fetch_basic_logger("CU-CP-F1", false);
  du_f1ap_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.f1ap_level));
  du_f1ap_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  cu_f1ap_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.f1ap_level));
  cu_f1ap_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  for (const auto& id : {"CU-F1-U", "DU-F1-U"}) {
    auto& f1u_logger = srslog::fetch_basic_logger(id, false);
    f1u_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.f1u_level));
    f1u_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);
  }

  auto& sec_logger = srslog::fetch_basic_logger("SEC", false);
  sec_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.sec_level));
  sec_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& pdcp_logger = srslog::fetch_basic_logger("PDCP", false);
  pdcp_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.pdcp_level));
  pdcp_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& rrc_logger = srslog::fetch_basic_logger("RRC", false);
  rrc_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.rrc_level));
  rrc_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& ngap_logger = srslog::fetch_basic_logger("NGAP", false);
  ngap_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.ngap_level));
  ngap_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& sdap_logger = srslog::fetch_basic_logger("SDAP", false);
  sdap_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.sdap_level));
  sdap_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& gtpu_logger = srslog::fetch_basic_logger("GTPU", false);
  gtpu_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.gtpu_level));
  gtpu_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  auto& fapi_logger = srslog::fetch_basic_logger("FAPI", true);
  fapi_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.fapi_level));

  auto& e2ap_logger = srslog::fetch_basic_logger("E2AP", false);
  e2ap_logger.set_level(srslog::str_to_basic_level(gnb_cfg.log_cfg.e2ap_level));
  e2ap_logger.set_hex_dump_max_size(gnb_cfg.log_cfg.hex_max_size);

  if (not gnb_cfg.log_cfg.tracing_filename.empty()) {
    gnb_logger.info("Opening event tracer...");
    open_trace_file(gnb_cfg.log_cfg.tracing_filename);
    gnb_logger.info("Event tracer opened successfully");
  }

  if (gnb_cfg.expert_execution_cfg.affinities.isolated_cpus) {
    if (!configure_cgroups(*gnb_cfg.expert_execution_cfg.affinities.isolated_cpus)) {
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

  // Setup size of byte buffer pool.
  init_byte_buffer_segment_pool(gnb_cfg.buffer_pool_config.nof_segments, gnb_cfg.buffer_pool_config.segment_size);

  // Log build info
  gnb_logger.info("Built in {} mode using {}", get_build_mode(), get_build_info());

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

  worker_manager workers{gnb_cfg};

  // Set layer-specific pcap options.
  const auto& low_prio_cpu_mask = gnb_cfg.expert_execution_cfg.affinities.low_priority_cpu_cfg.mask;

  std::unique_ptr<dlt_pcap> ngap_p = gnb_cfg.pcap_cfg.ngap.enabled ? create_ngap_pcap(gnb_cfg.pcap_cfg.ngap.filename,
                                                                                      workers.get_executor("pcap_exec"))
                                                                   : create_null_dlt_pcap();
  std::unique_ptr<dlt_pcap> e1ap_p = gnb_cfg.pcap_cfg.e1ap.enabled ? create_e1ap_pcap(gnb_cfg.pcap_cfg.e1ap.filename,
                                                                                      workers.get_executor("pcap_exec"))
                                                                   : create_null_dlt_pcap();
  std::unique_ptr<dlt_pcap> f1ap_p = gnb_cfg.pcap_cfg.f1ap.enabled ? create_f1ap_pcap(gnb_cfg.pcap_cfg.f1ap.filename,
                                                                                      workers.get_executor("pcap_exec"))
                                                                   : create_null_dlt_pcap();
  std::unique_ptr<dlt_pcap> e2ap_p = gnb_cfg.pcap_cfg.e2ap.enabled ? create_e2ap_pcap(gnb_cfg.pcap_cfg.e2ap.filename,
                                                                                      workers.get_executor("pcap_exec"))
                                                                   : create_null_dlt_pcap();
  std::unique_ptr<dlt_pcap> gtpu_p =
      gnb_cfg.pcap_cfg.gtpu.enabled
          ? create_gtpu_pcap(gnb_cfg.pcap_cfg.gtpu.filename, workers.get_executor("gtpu_pcap_exec"))
          : create_null_dlt_pcap();
  if (gnb_cfg.pcap_cfg.mac.type != "dlt" and gnb_cfg.pcap_cfg.mac.type != "udp") {
    report_error("Invalid type for MAC PCAP. type={}\n", gnb_cfg.pcap_cfg.mac.type);
  }
  std::unique_ptr<mac_pcap> mac_p =
      gnb_cfg.pcap_cfg.mac.enabled
          ? create_mac_pcap(gnb_cfg.pcap_cfg.mac.filename,
                            gnb_cfg.pcap_cfg.mac.type == "dlt" ? mac_pcap_type::dlt : mac_pcap_type::udp,
                            workers.get_executor("mac_pcap_exec"))
          : create_null_mac_pcap();
  if (gnb_cfg.pcap_cfg.rlc.rb_type != "all" and gnb_cfg.pcap_cfg.rlc.rb_type != "srb" and
      gnb_cfg.pcap_cfg.rlc.rb_type != "drb") {
    report_error("Invalid rb_type for RLC PCAP. rb_type={}\n", gnb_cfg.pcap_cfg.rlc.rb_type);
  }
  std::unique_ptr<rlc_pcap> rlc_p = gnb_cfg.pcap_cfg.rlc.enabled
                                        ? create_rlc_pcap(gnb_cfg.pcap_cfg.rlc.filename,
                                                          workers.get_executor("rlc_pcap_exec"),
                                                          gnb_cfg.pcap_cfg.rlc.rb_type != "drb",
                                                          gnb_cfg.pcap_cfg.rlc.rb_type != "srb")
                                        : create_null_rlc_pcap();

  f1c_gateway_local_connector  f1c_gw{*f1ap_p};
  e1ap_gateway_local_connector e1ap_gw{*e1ap_p};

  // Create manager of timers for DU, CU-CP and CU-UP, which will be driven by the PHY slot ticks.
  timer_manager                  app_timers{256};
  timer_manager*                 cu_timers = &app_timers;
  std::unique_ptr<timer_manager> dummy_timers;
  if (gnb_cfg.test_mode_cfg.test_ue.rnti != rnti_t::INVALID_RNTI) {
    // In case test mode is enabled, we pass dummy timers to the upper layers.
    dummy_timers = std::make_unique<timer_manager>(256);
    cu_timers    = dummy_timers.get();
  }

  // Create F1-U connector
  std::unique_ptr<f1u_local_connector> f1u_conn = std::make_unique<f1u_local_connector>();

  // Create IO broker.
  io_broker_config           io_broker_cfg(low_prio_cpu_mask);
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll, io_broker_cfg);

  // Set up the JSON log channel used by metrics.
  srslog::sink& json_sink =
      srslog::fetch_udp_sink(gnb_cfg.metrics_cfg.addr, gnb_cfg.metrics_cfg.port, srslog::create_json_formatter());
  srslog::log_channel& json_channel = srslog::fetch_log_channel("JSON_channel", json_sink, {});
  json_channel.set_enabled(gnb_cfg.metrics_cfg.enable_json_metrics);

  // Set up RLC JSON log channel used by metrics.
  srslog::log_channel& rlc_json_channel = srslog::fetch_log_channel("JSON_RLC_channel", json_sink, {});
  rlc_json_channel.set_enabled(gnb_cfg.metrics_cfg.rlc.json_enabled);
  rlc_metrics_plotter_json rlc_json_plotter(rlc_json_channel);

  // Create console helper object for commands and metrics printing.
  gnb_console_helper console(*epoll_broker, json_channel, gnb_cfg.metrics_cfg.autostart_stdout_metrics);
  console.on_app_starting();

  std::unique_ptr<metrics_hub> hub = std::make_unique<metrics_hub>(*workers.metrics_hub_exec);
  e2_metric_connector_manager  e2_metric_connectors{gnb_cfg};

  // Create NGAP Gateway.
  std::unique_ptr<srs_cu_cp::ngap_gateway_connector> ngap_adapter;
  {
    using no_core_mode_t = srs_cu_cp::ngap_gateway_params::no_core;
    using network_mode_t = srs_cu_cp::ngap_gateway_params::network;
    using ngap_mode_t    = variant<no_core_mode_t, network_mode_t>;

    ngap_adapter = srs_cu_cp::create_ngap_gateway(srs_cu_cp::ngap_gateway_params{
        *ngap_p,
        gnb_cfg.amf_cfg.no_core ? ngap_mode_t{no_core_mode_t{}}
                                : ngap_mode_t{network_mode_t{*epoll_broker, generate_ngap_nw_config(gnb_cfg)}}});
  }

  // E2AP configuration.
  srsran::sctp_network_gateway_config e2_du_nw_config = generate_e2ap_nw_config(gnb_cfg, E2_DU_PPID);

  // Create E2AP GW remote connector.
  e2_gateway_remote_connector e2_gw{*epoll_broker, e2_du_nw_config, *e2ap_p};

  // Create CU-CP config.
  srs_cu_cp::cu_cp_configuration cu_cp_cfg = generate_cu_cp_config(gnb_cfg);
  cu_cp_cfg.cu_cp_executor                 = workers.cu_cp_exec;
  cu_cp_cfg.cu_cp_e2_exec                  = workers.cu_cp_e2_exec;
  cu_cp_cfg.ngap_notifier                  = ngap_adapter.get();
  cu_cp_cfg.timers                         = cu_timers;

  // create CU-CP.
  std::unique_ptr<srsran::srs_cu_cp::cu_cp> cu_cp_obj = create_cu_cp(cu_cp_cfg);

  // Connect NGAP adpter to CU-CP to pass NGAP messages.
  ngap_adapter->connect_cu_cp(cu_cp_obj->get_ng_handler().get_ngap_message_handler(),
                              cu_cp_obj->get_ng_handler().get_ngap_event_handler());

  // Connect E1AP to CU-CP.
  e1ap_gw.attach_cu_cp(cu_cp_obj->get_e1_handler());

  // Connect F1-C to CU-CP.
  f1c_gw.attach_cu_cp(cu_cp_obj->get_f1c_handler());

  // start CU-CP
  gnb_logger.info("Starting CU-CP...");
  cu_cp_obj->start();
  gnb_logger.info("CU-CP started successfully");

  if (not cu_cp_obj->get_ng_handler().amf_is_connected()) {
    report_error("CU-CP failed to connect to AMF");
  }

  // Create CU-UP config.
  srsran::srs_cu_up::cu_up_configuration cu_up_cfg = generate_cu_up_config(gnb_cfg);
  cu_up_cfg.ctrl_executor                          = workers.cu_up_ctrl_exec;
  cu_up_cfg.cu_up_e2_exec                          = workers.cu_up_e2_exec;
  cu_up_cfg.ue_exec_pool                           = workers.cu_up_exec_mapper.get();
  cu_up_cfg.io_ul_executor        = workers.cu_up_io_ul_exec; // Optionally select separate exec for UL IO
  cu_up_cfg.e1ap.e1ap_conn_client = &e1ap_gw;
  cu_up_cfg.f1u_gateway           = f1u_conn->get_f1u_cu_up_gateway();
  cu_up_cfg.epoll_broker          = epoll_broker.get();
  cu_up_cfg.gtpu_pcap             = gtpu_p.get();
  cu_up_cfg.timers                = cu_timers;
  cu_up_cfg.qos                   = generate_cu_up_qos_config(gnb_cfg);

  // create and start CU-UP
  std::unique_ptr<srsran::srs_cu_up::cu_up_interface> cu_up_obj = create_cu_up(cu_up_cfg);
  cu_up_obj->start();

  std::vector<du_cell_config> du_cells = generate_du_cell_config(gnb_cfg);

  // Radio Unit instantiation block.
  ru_configuration ru_cfg = generate_ru_config(gnb_cfg, du_cells);

  upper_ru_ul_adapter     ru_ul_adapt(gnb_cfg.cells_cfg.size());
  upper_ru_timing_adapter ru_timing_adapt(gnb_cfg.cells_cfg.size());
  upper_ru_error_adapter  ru_error_adapt(gnb_cfg.cells_cfg.size());

  std::unique_ptr<radio_unit> ru_object;
  if (variant_holds_alternative<ru_ofh_configuration>(ru_cfg.config)) {
    ru_ofh_dependencies ru_dependencies;
    configure_ru_ofh_executors_and_notifiers(variant_get<ru_ofh_configuration>(ru_cfg.config),
                                             ru_dependencies,
                                             gnb_cfg.log_cfg,
                                             workers,
                                             ru_ul_adapt,
                                             ru_timing_adapt,
                                             ru_error_adapt);

    ru_object = create_ofh_ru(variant_get<ru_ofh_configuration>(ru_cfg.config), std::move(ru_dependencies));
  } else if (variant_holds_alternative<ru_generic_configuration>(ru_cfg.config)) {
    configure_ru_generic_executors_and_notifiers(
        variant_get<ru_generic_configuration>(ru_cfg.config), gnb_cfg.log_cfg, workers, ru_ul_adapt, ru_timing_adapt);

    ru_object = create_generic_ru(variant_get<ru_generic_configuration>(ru_cfg.config));

    // Set the generic RU controller for the GNB console.
    console.set_ru_controller(ru_object->get_controller());
  } else {
    ru_dummy_dependencies ru_dependencies;
    configure_ru_dummy_executors_and_notifiers(variant_get<ru_dummy_configuration>(ru_cfg.config),
                                               ru_dependencies,
                                               gnb_cfg.log_cfg,
                                               workers,
                                               ru_ul_adapt,
                                               ru_timing_adapt);

    ru_object = create_dummy_ru(variant_get<ru_dummy_configuration>(ru_cfg.config), ru_dependencies);
  }
  report_error_if_not(ru_object, "Unable to create Radio Unit.");
  gnb_logger.info("Radio Unit created successfully");

  upper_ru_dl_rg_adapter      ru_dl_rg_adapt;
  upper_ru_ul_request_adapter ru_ul_request_adapt;
  ru_dl_rg_adapt.connect(ru_object->get_downlink_plane_handler());
  ru_ul_request_adapt.connect(ru_object->get_uplink_plane_handler());

  // Instantiate one DU per cell.
  std::vector<std::unique_ptr<du>> du_inst = make_gnb_dus(gnb_cfg,
                                                          du_cells,
                                                          workers,
                                                          ru_dl_rg_adapt,
                                                          ru_ul_request_adapt,
                                                          f1c_gw,
                                                          *f1u_conn->get_f1u_du_gateway(),
                                                          app_timers,
                                                          *mac_p,
                                                          *rlc_p,
                                                          console,
                                                          e2_gw,
                                                          e2_metric_connectors,
                                                          rlc_json_plotter,
                                                          *hub);

  for (unsigned sector_id = 0, sector_end = du_inst.size(); sector_id != sector_end; ++sector_id) {
    auto& du = du_inst[sector_id];

    // Make connections between DU and RU.
    ru_ul_adapt.map_handler(sector_id, du->get_rx_symbol_handler());
    ru_timing_adapt.map_handler(sector_id, du->get_timing_handler());
    ru_error_adapt.map_handler(sector_id, du->get_error_handler());

    // Start DU execution.
    du->start();
  }

  // Start processing.
  gnb_logger.info("Starting Radio Unit...");
  ru_object->get_controller().start();
  gnb_logger.info("Radio Unit started successfully");

  console.on_app_running();

  while (is_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  console.on_app_stopping();

  gnb_logger.info("Stopping Radio Unit...");
  ru_object->get_controller().stop();
  gnb_logger.info("Radio Unit notify_stop successfully");

  // Stop DU activity.
  for (auto& du : du_inst) {
    du->stop();
  }

  // Stop CU-UP activity.
  cu_up_obj->stop();

  // Stop CU-CP activity.
  cu_cp_obj->stop();

  gnb_logger.info("Closing network connections...");
  ngap_adapter->disconnect();
  gnb_logger.info("Network connections closed successfully");

  if (gnb_cfg.e2_cfg.enable_du_e2) {
    gnb_logger.info("Closing E2 network connections...");
    e2_gw.close();
    gnb_logger.info("E2 Network connections closed successfully");
  }

  gnb_logger.info("Closing PCAP files...");
  ngap_p->close();
  gtpu_p->close();
  e1ap_p->close();
  f1ap_p->close();
  e2ap_p->close();
  mac_p->close();
  rlc_p->close();
  gnb_logger.info("PCAP files successfully closed.");

  gnb_logger.info("Stopping executors...");
  workers.stop();
  gnb_logger.info("Executors closed successfully.");

  srslog::flush();

  if (not gnb_cfg.log_cfg.tracing_filename.empty()) {
    gnb_logger.info("Closing event tracer...");
    close_trace_file();
    gnb_logger.info("Event tracer closed successfully");
  }

  if (gnb_cfg.expert_execution_cfg.affinities.isolated_cpus) {
    cleanup_cgroups();
  }

  return 0;
}

/// \endcond
