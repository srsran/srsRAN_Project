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

#include "srsran/cu_up/cu_up.h"
#include "srsran/cu_up/cu_up_factory.h"
#include "srsran/f1ap/gateways/f1c_network_server_factory.h"
#include "srsran/f1u/cu_up/split_connector/f1u_split_connector_factory.h"
#include "srsran/gateways/udp_network_gateway.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_demux_factory.h"
#include "srsran/gtpu/ngu_gateway.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/backtrace.h"
#include "srsran/support/build_info/build_info.h"
#include "srsran/support/config_parsers.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/event_tracing.h"
#include "srsran/support/io/io_broker.h"
#include "srsran/support/io/io_broker_factory.h"
#include "srsran/support/io/io_timer_source.h"
#include "srsran/support/signal_handling.h"
#include "srsran/support/sysinfo.h"
#include "srsran/support/timers.h"
#include "srsran/support/version/version.h"

#include "apps/cu/cu_appconfig_cli11_schema.h"
#include "apps/cu/cu_worker_manager.h"
#include "apps/services/metrics_log_helper.h"
#include "apps/units/cu_cp/cu_cp_builder.h"
#include "apps/units/cu_cp/cu_cp_config_translators.h"
#include "apps/units/cu_cp/cu_cp_logger_registrator.h"
#include "apps/units/cu_cp/cu_cp_unit_config.h"
#include "apps/units/cu_cp/cu_cp_unit_config_cli11_schema.h"
#include "apps/units/cu_cp/cu_cp_unit_config_validator.h"
#include "apps/units/cu_cp/cu_cp_unit_logger_config.h"
#include "apps/units/cu_cp/pcap_factory.h"
#include "apps/units/cu_up/cu_up_builder.h"
#include "apps/units/cu_up/cu_up_logger_registrator.h"
#include "apps/units/cu_up/cu_up_unit_config.h"
#include "apps/units/cu_up/cu_up_unit_config_cli11_schema.h"
#include "apps/units/cu_up/cu_up_unit_config_translators.h"
#include "apps/units/cu_up/cu_up_unit_config_validator.h"
#include "apps/units/cu_up/cu_up_wrapper.h"
#include "apps/units/cu_up/pcap_factory.h"

// TODO remove apps/gnb/*.h
#include "apps/gnb/adapters/e2_gateway_remote_connector.h"
#include "apps/gnb/gnb_appconfig_translators.h"
#include "srsran/e1ap/gateways/e1_local_connector_factory.h"
#include "srsran/ngap/gateways/n2_connection_client_factory.h"

#include "apps/services/application_message_banners.h"
#include "apps/services/application_tracer.h"
#include "apps/services/buffer_pool/buffer_pool_manager.h"
#include "apps/services/stdin_command_dispatcher.h"
#include "apps/units/cu_cp/cu_cp_unit_config_yaml_writer.h"
#include "apps/units/cu_up/cu_up_unit_config_yaml_writer.h"
#include "cu_appconfig.h"
#include "cu_appconfig_validator.h"
#include "cu_appconfig_yaml_writer.h"

#include <atomic>
#include <thread>

using namespace srsran;

/// \file
/// \brief Application of a Central Unit (CU) with combined CU control-plane (CU-CP) and CU user-plane (CU-UP).
///
/// This application runs a CU without the E1 connection between the CU-CP and CU-UP going over a real SCTP
/// connection. However, its does expose the F1, N2 and N3 interface to the DU, AMF and UPF over the standard
/// UDP/SCTP ports.
///
/// The app serves as an example for an all-integrated CU.

static std::string config_file;

/// Flag that indicates if the application is running or being shutdown.
static std::atomic<bool> is_app_running = {true};
/// Maximum number of configuration files allowed to be concatenated in the command line.
static constexpr unsigned MAX_CONFIG_FILES = 10;

static void populate_cli11_generic_args(CLI::App& app)
{
  fmt::memory_buffer buffer;
  format_to(buffer, "srsRAN 5G CU version {} ({})", get_version(), get_build_hash());
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
                              const cu_up_unit_logger_config& cu_up_loggers)
{
  // Set log-level of app and all non-layer specific components to app level.
  for (const auto& id : {"ALL", "SCTP-GW", "IO-EPOLL", "UDP-GW", "PCAP"}) {
    auto& logger = srslog::fetch_basic_logger(id, false);
    logger.set_level(log_cfg.lib_level);
    logger.set_hex_dump_max_size(log_cfg.hex_max_size);
  }

  auto& app_logger = srslog::fetch_basic_logger("CU", false);
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

  // Register units logs.
  register_cu_cp_loggers(cu_cp_loggers);
  register_cu_up_loggers(cu_up_loggers);
}

// Temporary helper to create CU-UP.
// TODO remove
std::unique_ptr<srs_cu_up::cu_up_interface> app_build_cu_up(const cu_up_unit_config&         unit_cfg,
                                                            cu_worker_manager&               workers,
                                                            const std::string&               f1u_bind_addr,
                                                            srs_cu_up::e1_connection_client& e1_conn_client,
                                                            f1u_cu_up_gateway&               f1u_gateway,
                                                            dlt_pcap&                        gtpu_pcap,
                                                            timer_manager&                   timers,
                                                            io_broker&                       io_brk);

int main(int argc, char** argv)
{
  // Set the application error handler.
  set_error_handler(app_error_report_handler);

  static constexpr std::string_view app_name = "CU";
  app_services::application_message_banners::announce_app_and_version(app_name);

  // Set interrupt and cleanup signal handlers.
  register_interrupt_signal_handler(interrupt_signal_handler);
  register_cleanup_signal_handler(cleanup_signal_handler);

  // Enable backtrace.
  enable_backtrace();

  // Setup and configure config parsing.
  CLI::App app("srsCU application");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  // Fill the generic application arguments to parse.
  populate_cli11_generic_args(app);

  // Configure CLI11 with the CU application configuration schema.
  cu_appconfig cu_cfg;
  configure_cli11_with_cu_appconfig_schema(app, cu_cfg);

  cu_cp_unit_config cu_cp_config;
  cu_cp_config.pcap_cfg.set_default_filename("/tmp/cu");
  configure_cli11_with_cu_cp_unit_config_schema(app, cu_cp_config);

  cu_up_unit_config cu_up_config;
  cu_up_config.pcap_cfg.set_default_filename("/tmp/cu");
  configure_cli11_with_cu_up_unit_config_schema(app, cu_up_config);

  // Set the callback for the app calling all the autoderivation functions.
  app.callback([&app, &cu_cp_config]() {
    // Create the PLMN and TAC list from the cells.
    std::vector<std::string> plmns;
    std::vector<unsigned>    tacs;
    autoderive_cu_cp_parameters_after_parsing(app, cu_cp_config, std::move(plmns), std::move(tacs));
  });

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  // Check the modified configuration.
  if (!validate_cu_appconfig(cu_cfg) || !validate_cu_cp_unit_config(cu_cp_config) ||
      !validate_cu_up_unit_config(cu_up_config)) {
    report_error("Invalid configuration detected.\n");
  }

  // Set up logging.
  initialize_log(cu_cfg.log_cfg.filename);
  register_app_logs(cu_cfg.log_cfg, cu_cp_config.loggers, cu_up_config.loggers);

  // Log input configuration.
  srslog::basic_logger& config_logger = srslog::fetch_basic_logger("CONFIG");
  if (config_logger.debug.enabled()) {
    YAML::Node node;
    fill_cu_appconfig_in_yaml_schema(node, cu_cfg);
    fill_cu_up_config_in_yaml_schema(node, cu_up_config);
    fill_cu_cp_config_in_yaml_schema(node, cu_cp_config);
    config_logger.debug("Input configuration (all values): \n{}", YAML::Dump(node));
  } else {
    config_logger.info("Input configuration (only non-default values): \n{}", app.config_to_str(false, false));
  }

  srslog::basic_logger&            cu_logger = srslog::fetch_basic_logger("CU");
  app_services::application_tracer app_tracer;
  if (not cu_cfg.log_cfg.tracing_filename.empty()) {
    app_tracer.enable_tracer(cu_cfg.log_cfg.tracing_filename, cu_logger);
  }

  // configure cgroups
  // TODO

  // Setup size of byte buffer pool.
  app_services::buffer_pool_manager buffer_pool_service(cu_cfg.buffer_pool_config);

  // Log CPU architecture.
  // TODO

  // Check and log included CPU features and check support by current CPU
  if (cpu_supports_included_features()) {
    cu_logger.debug("Required CPU features: {}", get_cpu_feature_info());
  } else {
    // Quit here until we complete selection of the best matching implementation for the current CPU at runtime.
    cu_logger.error("The CPU does not support the required CPU features that were configured during compile time: {}",
                    get_cpu_feature_info());
    report_error("The CPU does not support the required CPU features that were configured during compile time: {}\n",
                 get_cpu_feature_info());
  }

  // Check some common causes of performance issues and print a warning if required.
  check_cpu_governor(cu_logger);
  check_drm_kms_polling(cu_logger);

  // Create worker manager.
  cu_worker_manager workers{cu_cfg, cu_cp_config.pcap_cfg, cu_up_config.pcap_cfg, cu_up_config.gtpu_queue_size};

  // Create layer specific PCAPs.
  srsran::modules::cu_cp::cu_cp_dlt_pcaps cu_cp_dlt_pcaps =
      modules::cu_cp::create_dlt_pcap(cu_cp_config.pcap_cfg, *workers.get_executor_getter());
  srsran::modules::cu_up::cu_up_dlt_pcaps cu_up_dlt_pcaps =
      modules::cu_up::create_dlt_pcaps(cu_up_config.pcap_cfg, *workers.get_executor_getter());

  // Create IO broker.
  const auto&                low_prio_cpu_mask = cu_cfg.expert_execution_cfg.affinities.low_priority_cpu_cfg.mask;
  io_broker_config           io_broker_cfg(low_prio_cpu_mask);
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll, io_broker_cfg);

  // Create F1-C GW (TODO cleanup port and PPID args with factory)
  sctp_network_gateway_config f1c_sctp_cfg = {};
  f1c_sctp_cfg.if_name                     = "F1-C";
  f1c_sctp_cfg.bind_address                = cu_cfg.f1ap_cfg.bind_addr;
  f1c_sctp_cfg.bind_port                   = F1AP_PORT;
  f1c_sctp_cfg.ppid                        = F1AP_PPID;
  f1c_cu_sctp_gateway_config f1c_server_cfg({f1c_sctp_cfg, *epoll_broker, *cu_cp_dlt_pcaps.f1ap});
  std::unique_ptr<srs_cu_cp::f1c_connection_server> cu_f1c_gw = srsran::create_f1c_gateway_server(f1c_server_cfg);

  // Create F1-U GW (TODO factory and cleanup).
  gtpu_demux_creation_request cu_f1u_gtpu_msg   = {};
  cu_f1u_gtpu_msg.cfg.warn_on_drop              = true;
  cu_f1u_gtpu_msg.gtpu_pcap                     = cu_up_dlt_pcaps.f1u.get();
  std::unique_ptr<gtpu_demux> cu_f1u_gtpu_demux = create_gtpu_demux(cu_f1u_gtpu_msg);
  udp_network_gateway_config  cu_f1u_gw_config  = {};
  cu_f1u_gw_config.bind_address                 = cu_cfg.nru_cfg.bind_addr;
  cu_f1u_gw_config.bind_port                    = GTPU_PORT;
  cu_f1u_gw_config.reuse_addr                   = true;
  std::unique_ptr<srs_cu_up::ngu_gateway> cu_f1u_gw =
      srs_cu_up::create_udp_ngu_gateway(cu_f1u_gw_config, *epoll_broker, *workers.cu_up_io_ul_exec);
  std::unique_ptr<f1u_cu_up_udp_gateway> cu_f1u_conn =
      srs_cu_up::create_split_f1u_gw({cu_f1u_gw.get(), cu_f1u_gtpu_demux.get(), *cu_up_dlt_pcaps.f1u, GTPU_PORT});

  // Create E1AP local connector
  std::unique_ptr<e1_local_connector> e1_gw =
      create_e1_local_connector(e1_local_connector_config{*cu_up_dlt_pcaps.e1ap});

  // Create manager of timers for CU-CP and CU-UP, which will be
  // driven by the system timer slot ticks.
  timer_manager  app_timers{256};
  timer_manager* cu_timers = &app_timers;

  // Create time source that ticks the timers
  io_timer_source time_source{app_timers, *epoll_broker, std::chrono::milliseconds{1}};

  // Create N2 Client Gateway.
  std::unique_ptr<srs_cu_cp::n2_connection_client> n2_client = srs_cu_cp::create_n2_connection_client(
      generate_n2_client_config(cu_cp_config.amf_cfg, *cu_cp_dlt_pcaps.ngap, *epoll_broker));

  // Create CU-CP config.
  cu_cp_build_dependencies cu_cp_dependencies;
  cu_cp_dependencies.cu_cp_executor = workers.cu_cp_exec;
  cu_cp_dependencies.cu_cp_e2_exec  = workers.cu_cp_e2_exec;
  cu_cp_dependencies.n2_client      = n2_client.get();
  cu_cp_dependencies.timers         = cu_timers;

  // create CU-CP.
  auto              cu_cp_obj_and_cmds = build_cu_cp(cu_cp_config, cu_cp_dependencies);
  srs_cu_cp::cu_cp& cu_cp_obj          = *cu_cp_obj_and_cmds.unit;

  // Create console helper object for commands and metrics printing.
  app_services::stdin_command_dispatcher command_parser(*epoll_broker, cu_cp_obj_and_cmds.commands);

  // Create metrics log helper.
  metrics_log_helper metrics_logger(srslog::fetch_basic_logger("METRICS"));

  // Connect E1AP to CU-CP.
  e1_gw->attach_cu_cp(cu_cp_obj.get_e1_handler());

  // start CU-CP
  cu_logger.info("Starting CU-CP...");
  cu_cp_obj.start();
  cu_logger.info("CU-CP started successfully");

  // Check connection to AMF
  if (not cu_cp_obj.get_ng_handler().amf_is_connected()) {
    report_error("CU-CP failed to connect to AMF");
  }

  // Connect F1-C to CU-CP and start listening for new F1-C connection requests.
  cu_f1c_gw->attach_cu_cp(cu_cp_obj.get_f1c_handler());

  // Create and start CU-UP
  std::unique_ptr<srs_cu_up::cu_up_interface> cu_up_obj = app_build_cu_up(cu_up_config,
                                                                          workers,
                                                                          cu_cfg.nru_cfg.bind_addr,
                                                                          *e1_gw,
                                                                          *cu_f1u_conn,
                                                                          *cu_up_dlt_pcaps.n3,
                                                                          *cu_timers,
                                                                          *epoll_broker);
  cu_up_obj->start();

  {
    app_services::application_message_banners app_banner(app_name);

    while (is_app_running) {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
  }

  // Stop CU-UP activity.
  cu_up_obj->stop();

  // Stop CU-CP activity.
  cu_cp_obj.stop();

  // Close PCAPs
  cu_logger.info("Closing PCAP files...");
  cu_cp_dlt_pcaps.close();
  cu_up_dlt_pcaps.close();
  cu_logger.info("PCAP files successfully closed.");

  // Stop workers
  cu_logger.info("Stopping executors...");
  workers.stop();
  cu_logger.info("Executors closed successfully.");

  srslog::flush();

  return 0;
}

// Temporary helper to create CU-UP.
// TODO remove
std::unique_ptr<srs_cu_up::cu_up_interface> app_build_cu_up(const cu_up_unit_config&         unit_cfg,
                                                            cu_worker_manager&               workers,
                                                            const std::string&               f1u_bind_address,
                                                            srs_cu_up::e1_connection_client& e1_conn_client,
                                                            f1u_cu_up_gateway&               f1u_gateway,
                                                            dlt_pcap&                        gtpu_pcap,
                                                            timer_manager&                   timers,
                                                            io_broker&                       io_brk)
{
  srs_cu_up::cu_up_configuration config = generate_cu_up_config(unit_cfg);
  config.ctrl_executor                  = workers.cu_up_ctrl_exec;
  config.cu_up_e2_exec                  = workers.cu_up_e2_exec;
  config.ue_exec_pool                   = workers.cu_up_exec_mapper.get();
  config.io_ul_executor                 = workers.cu_up_io_ul_exec; // Optionally select separate exec for UL IO
  config.e1ap.e1_conn_client            = &e1_conn_client;
  config.f1u_gateway                    = &f1u_gateway;
  config.gtpu_pcap                      = &gtpu_pcap;
  config.timers                         = &timers;
  config.qos                            = generate_cu_up_qos_config(unit_cfg);

  config.net_cfg.f1u_bind_addr = f1u_bind_address; // TODO remove this parameter and make sure that the CU-UP gets the
                                                   // bind address directly from the gateway.

  // Create NG-U gateway.
  std::unique_ptr<srs_cu_up::ngu_gateway> ngu_gw;
  if (not unit_cfg.upf_cfg.no_core) {
    udp_network_gateway_config ngu_gw_config = {};
    ngu_gw_config.bind_address               = config.net_cfg.n3_bind_addr;
    ngu_gw_config.bind_port                  = config.net_cfg.n3_bind_port;
    ngu_gw_config.bind_interface             = config.net_cfg.n3_bind_interface;
    ngu_gw_config.rx_max_mmsg                = config.net_cfg.n3_rx_max_mmsg;
    ngu_gw = srs_cu_up::create_udp_ngu_gateway(ngu_gw_config, io_brk, *workers.cu_up_io_ul_exec);
  } else {
    ngu_gw = srs_cu_up::create_no_core_ngu_gateway();
  }
  config.ngu_gw = ngu_gw.get();

  return std::make_unique<cu_up_wrapper>(std::move(ngu_gw), create_cu_up(config));
}
