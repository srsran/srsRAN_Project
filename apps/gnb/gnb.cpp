/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/pcap/pcap.h"
#include "srsran/support/build_info/build_info.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/event_tracing.h"
#include "srsran/support/signal_handler.h"
#include "srsran/support/tsan_options.h"
#include "srsran/support/version/version.h"

#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_factory.h"
#include "srsran/cu_cp/cu_cp_types.h"

#include "srsran/cu_up/cu_up_factory.h"
#include "srsran/f1u/local_connector/f1u_local_connector.h"

#include "adapters/ngap_adapter.h"
#include "srsran/support/io/io_broker_factory.h"

#include "adapters/e1ap_adapter.h"
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

#include "gnb_du_factory.h"
#include "lib/pcap/dlt_pcap_impl.h"
#include "lib/pcap/mac_pcap_impl.h"
#include "srsran/phy/upper/upper_phy_timing_notifier.h"

#include "srsran/ru/ru_adapters.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/ru/ru_generic_factory.h"
#include "srsran/ru/ru_ofh_factory.h"

#include "apps/gnb/adapters/e2_gateway_remote_connector.h"
#include "srsran/e2/e2_du_metrics_manager.h"
#include "srsran/support/sysinfo.h"

#include <atomic>

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
// NGAP configuration.
static srsran::sctp_network_gateway_config ngap_nw_config;
static srsran::sctp_network_gateway_config e2_nw_config;
const int                                  MAX_CONFIG_FILES(10);

static void populate_cli11_generic_args(CLI::App& app)
{
  fmt::memory_buffer buffer;
  format_to(buffer, "srsRAN 5G gNB version {} ({})", get_version(), get_build_hash());
  app.set_version_flag("-v,--version", srsran::to_c_str(buffer));
  app.set_config("-c,", config_file, "Read config from file", false)->expected(1, MAX_CONFIG_FILES);
}

/// This function takes the populated appconfig and generates (sub)-component configurations.
static void compute_derived_args(const gnb_appconfig& gnb_params)
{
  /// Simply set the respective values in the appconfig.
  ngap_nw_config.connection_name = "AMF";
  ngap_nw_config.connect_address = gnb_params.amf_cfg.ip_addr;
  ngap_nw_config.connect_port    = gnb_params.amf_cfg.port;
  ngap_nw_config.bind_address    = gnb_params.amf_cfg.bind_addr;
  ngap_nw_config.ppid            = NGAP_PPID;
  if (gnb_params.amf_cfg.sctp_rto_initial >= 0) {
    ngap_nw_config.rto_initial = gnb_params.amf_cfg.sctp_rto_initial;
  }
  if (gnb_params.amf_cfg.sctp_rto_min >= 0) {
    ngap_nw_config.rto_min = gnb_params.amf_cfg.sctp_rto_min;
  }
  if (gnb_params.amf_cfg.sctp_rto_max >= 0) {
    ngap_nw_config.rto_max = gnb_params.amf_cfg.sctp_rto_max;
  }
  if (gnb_params.amf_cfg.sctp_init_max_attempts >= 0) {
    ngap_nw_config.init_max_attempts = gnb_params.amf_cfg.sctp_init_max_attempts;
  }
  if (gnb_params.amf_cfg.sctp_max_init_timeo >= 0) {
    ngap_nw_config.max_init_timeo = gnb_params.amf_cfg.sctp_max_init_timeo;
  }

  /// E2 interface - simply set the respective values in the appconfig.
  if (gnb_params.e2_cfg.enable_du_e2) {
    e2_nw_config.connection_name = "NearRT-RIC";
    e2_nw_config.connect_address = gnb_params.e2_cfg.ip_addr;
    e2_nw_config.connect_port    = gnb_params.e2_cfg.port;
    e2_nw_config.bind_address    = gnb_params.e2_cfg.bind_addr;
    e2_nw_config.ppid            = 0;
    if (gnb_params.e2_cfg.sctp_rto_initial >= 0) {
      e2_nw_config.rto_initial = gnb_params.e2_cfg.sctp_rto_initial;
    }
    if (gnb_params.e2_cfg.sctp_rto_min >= 0) {
      e2_nw_config.rto_min = gnb_params.e2_cfg.sctp_rto_min;
    }
    if (gnb_params.e2_cfg.sctp_rto_max >= 0) {
      e2_nw_config.rto_max = gnb_params.e2_cfg.sctp_rto_max;
    }
    if (gnb_params.e2_cfg.sctp_init_max_attempts >= 0) {
      e2_nw_config.init_max_attempts = gnb_params.e2_cfg.sctp_init_max_attempts;
    }
    if (gnb_params.e2_cfg.sctp_max_init_timeo >= 0) {
      e2_nw_config.max_init_timeo = gnb_params.e2_cfg.sctp_max_init_timeo;
    }
  }
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
  config.radio_exec                  = workers.radio_exec.get();
  config.statistics_printer_executor = workers.ru_printer_exec.get();
  config.timing_notifier             = &timing_notifier;
  config.symbol_notifier             = &symbol_notifier;

  for (unsigned i = 0, e = config.lower_phy_config.size(); i != e; ++i) {
    lower_phy_configuration& low_phy_cfg = config.lower_phy_config[i];
    low_phy_cfg.logger                   = &srslog::fetch_basic_logger("Low-PHY#" + std::to_string(i));
    low_phy_cfg.tx_task_executor         = workers.lower_phy_tx_exec[i].get();
    low_phy_cfg.rx_task_executor         = workers.lower_phy_rx_exec[i].get();
    low_phy_cfg.dl_task_executor         = workers.lower_phy_dl_exec[i].get();
    low_phy_cfg.ul_task_executor         = workers.lower_phy_ul_exec[i].get();
    low_phy_cfg.prach_async_executor     = workers.lower_prach_exec[i].get();

    low_phy_cfg.logger->set_level(srslog::str_to_basic_level(log_cfg.phy_level));
  }
}

/// Resolves the Open Fronthaul Radio Unit dependencies and adds them to the configuration.
static void configure_ru_ofh_executors_and_notifiers(ru_ofh_configuration&               config,
                                                     const log_appconfig&                log_cfg,
                                                     worker_manager&                     workers,
                                                     ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                     ru_timing_notifier&                 timing_notifier)
{
  srslog::basic_logger& ofh_logger = srslog::fetch_basic_logger("OFH", false);
  ofh_logger.set_level(srslog::str_to_basic_level(log_cfg.ofh_level));

  config.logger             = &ofh_logger;
  config.rt_timing_executor = workers.ru_timing_exec.get();
  config.timing_notifier    = &timing_notifier;
  config.rx_symbol_notifier = &symbol_notifier;

  // Configure sector.
  for (unsigned i = 0, e = config.sector_configs.size(); i != e; ++i) {
    ru_ofh_sector_configuration& sector_cfg = config.sector_configs[i];
    sector_cfg.receiver_executor            = workers.ru_rx_exec[i].get();
    sector_cfg.transmitter_executor         = workers.ru_tx_exec[i].get();
    sector_cfg.downlink_executors           = ([](span<std::unique_ptr<task_executor>> executor) {
      std::vector<task_executor*> out;
      for (auto& exec : executor) {
        out.push_back(exec.get());
      }
      return out;
    })(workers.ru_dl_exec[i]);
  }
}

/// Resolves the Radio Unit dependencies and adds them to the configuration.
static void configure_ru_executors_and_notifiers(ru_configuration&                   config,
                                                 const log_appconfig&                log_cfg,
                                                 worker_manager&                     workers,
                                                 ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                 ru_timing_notifier&                 timing_notifier)
{
  if (variant_holds_alternative<ru_ofh_configuration>(config.config)) {
    configure_ru_ofh_executors_and_notifiers(
        variant_get<ru_ofh_configuration>(config.config), log_cfg, workers, symbol_notifier, timing_notifier);

    return;
  }

  configure_ru_generic_executors_and_notifiers(
      variant_get<ru_generic_configuration>(config.config), log_cfg, workers, symbol_notifier, timing_notifier);
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

  gnb_appconfig gnb_cfg;
  // Configure CLI11 with the gNB application configuration schema.
  configure_cli11_with_gnb_appconfig_schema(app, gnb_cfg);

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  // Check the modified configuration.
  if (!validate_appconfig(gnb_cfg)) {
    report_error("Invalid configuration detected.\n");
  }

  // Setup size of byte buffer pool.
  init_byte_buffer_segment_pool(gnb_cfg.buffer_pool_config.nof_segments, gnb_cfg.buffer_pool_config.segment_size);

  // Compute derived parameters.
  compute_derived_args(gnb_cfg);

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
  auto& phy_logger = srslog::fetch_basic_logger("PHY", false);
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
    open_trace_file(gnb_cfg.log_cfg.tracing_filename);
  }

  // Log build info
  gnb_logger.info("Built in {} mode using {}", get_build_mode(), get_build_info());

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

  // Check some common causes of performance issues and
  // print a warning if required.
  check_cpu_governor(gnb_logger);
  check_drm_kms_polling(gnb_logger);

  // Set layer-specific pcap options.
  std::unique_ptr<dlt_pcap> ngap_p = std::make_unique<dlt_pcap_impl>(PCAP_NGAP_DLT, "NGAP");
  if (gnb_cfg.pcap_cfg.ngap.enabled) {
    ngap_p->open(gnb_cfg.pcap_cfg.ngap.filename.c_str());
  }
  std::unique_ptr<dlt_pcap> e1ap_p = std::make_unique<dlt_pcap_impl>(PCAP_E1AP_DLT, "E1AP");
  if (gnb_cfg.pcap_cfg.e1ap.enabled) {
    e1ap_p->open(gnb_cfg.pcap_cfg.e1ap.filename.c_str());
  }
  std::unique_ptr<dlt_pcap> f1ap_p = std::make_unique<dlt_pcap_impl>(PCAP_F1AP_DLT, "F1AP");
  if (gnb_cfg.pcap_cfg.f1ap.enabled) {
    f1ap_p->open(gnb_cfg.pcap_cfg.f1ap.filename.c_str());
  }
  std::unique_ptr<dlt_pcap> e2ap_p = std::make_unique<dlt_pcap_impl>(PCAP_E2AP_DLT, "E2AP");
  if (gnb_cfg.pcap_cfg.e2ap.enabled) {
    e2ap_p->open(gnb_cfg.pcap_cfg.e2ap.filename.c_str());
  }

  std::unique_ptr<mac_pcap> mac_p = std::make_unique<mac_pcap_impl>();
  if (gnb_cfg.pcap_cfg.mac.enabled) {
    mac_p->open(gnb_cfg.pcap_cfg.mac.filename.c_str());
  }

  worker_manager workers{gnb_cfg};

  f1c_gateway_local_connector f1c_gw{*f1ap_p};
  e1ap_local_adapter          e1ap_cp_to_up_adapter("CU-CP-E1", *e1ap_p), e1ap_up_to_cp_adapter("CU-UP-E1", *e1ap_p);

  // Create manager of timers for DU, CU-CP and CU-UP, which will be driven by the PHY slot ticks.
  timer_manager app_timers{256};

  // Create F1-U connector
  std::unique_ptr<f1u_local_connector> f1u_conn = std::make_unique<f1u_local_connector>();

  // Create IO broker.
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll);

  // Create console helper object for commands and metrics printing.
  gnb_console_helper console(*epoll_broker);
  console.on_app_starting();

  std::unique_ptr<metrics_hub> hub = std::make_unique<metrics_hub>(*workers.metrics_hub_exec.get());
  std::vector<std::unique_ptr<e2_du_metrics_manager>> e2_du_metric_managers;

  // Create NGAP adapter.
  std::unique_ptr<srsran::srs_cu_cp::ngap_network_adapter> ngap_adapter =
      std::make_unique<srsran::srs_cu_cp::ngap_network_adapter>(*epoll_broker, *ngap_p);

  // Create SCTP network adapter.
  std::unique_ptr<sctp_network_gateway> sctp_gateway = {};
  if (not gnb_cfg.amf_cfg.no_core) {
    gnb_logger.info("Connecting to AMF ({})..", ngap_nw_config.connect_address, ngap_nw_config.connect_port);
    sctp_gateway = create_sctp_network_gateway({ngap_nw_config, *ngap_adapter, *ngap_adapter});

    // Connect NGAP adapter to SCTP network gateway.
    ngap_adapter->connect_gateway(sctp_gateway.get(), sctp_gateway.get());
    gnb_logger.info("AMF connection established");
  } else {
    gnb_logger.info("Bypassing AMF connection");
  }

  e2_gateway_remote_connector e2_gw{*epoll_broker, e2_nw_config, *e2ap_p};

  // Create CU-UP config.
  srsran::srs_cu_up::cu_up_configuration cu_up_cfg;
  cu_up_cfg.cu_up_executor       = workers.cu_up_exec.get();
  cu_up_cfg.cu_up_e2_exec        = workers.cu_up_e2_exec.get();
  cu_up_cfg.gtpu_pdu_executor    = workers.gtpu_pdu_exec.get();
  cu_up_cfg.e1ap_notifier        = &e1ap_up_to_cp_adapter;
  cu_up_cfg.f1u_gateway          = f1u_conn->get_f1u_cu_up_gateway();
  cu_up_cfg.epoll_broker         = epoll_broker.get();
  cu_up_cfg.timers               = &app_timers;
  cu_up_cfg.net_cfg.n3_bind_addr = gnb_cfg.amf_cfg.bind_addr; // TODO: rename variable to core addr
  cu_up_cfg.net_cfg.f1u_bind_addr =
      gnb_cfg.amf_cfg.bind_addr; // FIXME: check if this can be removed for co-located case

  // create and start DUT
  std::unique_ptr<srsran::srs_cu_up::cu_up_interface> cu_up_obj = create_cu_up(cu_up_cfg);

  // Create CU-CP config.
  srs_cu_cp::cu_cp_configuration cu_cp_cfg = generate_cu_cp_config(gnb_cfg);
  cu_cp_cfg.cu_cp_executor                 = workers.cu_cp_exec.get();
  cu_cp_cfg.cu_cp_e2_exec                  = workers.cu_cp_e2_exec.get();
  cu_cp_cfg.e1ap_notifier                  = &e1ap_cp_to_up_adapter;
  cu_cp_cfg.ngap_notifier                  = ngap_adapter.get();

  // create CU-CP.
  std::unique_ptr<srsran::srs_cu_cp::cu_cp_interface> cu_cp_obj = create_cu_cp(cu_cp_cfg);
  cu_cp_obj->handle_new_cu_up_connection(); // trigger CU-UP addition

  // Connect NGAP adpter to CU-CP to pass NGAP messages.
  ngap_adapter->connect_ngap(&cu_cp_obj->get_ngap_message_handler(), &cu_cp_obj->get_ngap_event_handler());

  // attach E1AP adapters to CU-UP and CU-CP
  e1ap_up_to_cp_adapter.attach_handler(&cu_cp_obj->get_e1ap_message_handler(srsran::srs_cu_cp::uint_to_cu_up_index(0)));
  e1ap_cp_to_up_adapter.attach_handler(&cu_up_obj->get_e1ap_message_handler());

  // Connect F1-C to CU-CP.
  f1c_gw.attach_cu_cp(cu_cp_obj->get_connected_dus());

  // start CU-CP
  gnb_logger.info("Starting CU-CP...");
  cu_cp_obj->start();
  gnb_logger.info("CU-CP started successfully");

  // Radio Unit instantiation block.
  ru_configuration ru_cfg = generate_ru_config(gnb_cfg);

  upper_ru_ul_adapter     ru_ul_adapt(gnb_cfg.cells_cfg.size());
  upper_ru_timing_adapter ru_timing_adapt(gnb_cfg.cells_cfg.size());

  configure_ru_executors_and_notifiers(ru_cfg, gnb_cfg.log_cfg, workers, ru_ul_adapt, ru_timing_adapt);

  std::unique_ptr<radio_unit> ru_object;
  if (variant_holds_alternative<ru_ofh_configuration>(ru_cfg.config)) {
    ru_object = create_ofh_ru(variant_get<ru_ofh_configuration>(ru_cfg.config));
  } else {
    ru_object = create_generic_ru(variant_get<ru_generic_configuration>(ru_cfg.config));
  }
  report_error_if_not(ru_object, "Unable to create Radio Unit.");
  gnb_logger.info("Radio Unit created successfully");

  upper_ru_dl_rg_adapter      ru_dl_rg_adapt;
  upper_ru_ul_request_adapter ru_ul_request_adapt;
  ru_dl_rg_adapt.connect(ru_object->get_downlink_plane_handler());
  ru_ul_request_adapt.connect(ru_object->get_uplink_plane_handler());

  // Instantiate one DU per cell.
  std::vector<std::unique_ptr<du>> du_inst = make_gnb_dus(gnb_cfg,
                                                          workers,
                                                          ru_dl_rg_adapt,
                                                          ru_ul_request_adapt,
                                                          f1c_gw,
                                                          *f1u_conn->get_f1u_du_gateway(),
                                                          app_timers,
                                                          *mac_p,
                                                          console,
                                                          e2_gw,
                                                          e2_du_metric_managers,
                                                          *hub);

  for (unsigned sector_id = 0, sector_end = du_inst.size(); sector_id != sector_end; ++sector_id) {
    auto& du = du_inst[sector_id];

    // Make connections between DU and RU.
    ru_ul_adapt.map_handler(sector_id, du->get_rx_symbol_handler());
    ru_timing_adapt.map_handler(sector_id, du->get_timing_handler());

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

  ngap_p->close();
  e1ap_p->close();
  f1ap_p->close();
  e2ap_p->close();
  mac_p->close();

  gnb_logger.info("Stopping Radio Unit...");
  ru_object->get_controller().stop();
  gnb_logger.info("Radio Unit notify_stop successfully");

  // Stop DU activity.
  for (auto& du : du_inst) {
    du->stop();
  }

  if (not gnb_cfg.amf_cfg.no_core) {
    gnb_logger.info("Closing network connections...");
    ngap_adapter->disconnect_gateway();
    gnb_logger.info("Network connections closed successfully");
  }

  if (gnb_cfg.e2_cfg.enable_du_e2) {
    e2_gw.close();
  }

  gnb_logger.info("Stopping executors...");
  workers.stop();
  gnb_logger.info("Executors notify_stop successfully");

  srslog::flush();

  return 0;
}

/// \endcond
