/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "CLI/CLI11.hpp"

#include "srsgnb/cu_cp/cu_cp_configuration.h"
#include "srsgnb/cu_cp/cu_cp_factory.h"
#include "srsgnb/cu_cp/cu_cp_types.h"

#include "adapters/ngap_adapter.h"
#include "srsgnb/support/io_broker/io_broker_factory.h"

#include "adapters/f1_adapter.h"
#include "srsgnb/support/config_json.h"

#include "gnb_appconfig.h"
#include "gnb_appconfig_translators.h"
#include "gnb_appconfig_validators.h"

#include "fapi_factory.h"
#include "lib/du_high/du_high.h"
#include "lib/du_high/du_high_executor_strategies.h"
#include "phy_factory.h"
#include "radio_notifier_sample.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"
#include "srsgnb/phy/adapters/phy_error_adapter.h"
#include "srsgnb/phy/adapters/phy_rg_gateway_adapter.h"
#include "srsgnb/phy/adapters/phy_rx_symbol_adapter.h"
#include "srsgnb/phy/adapters/phy_rx_symbol_request_adapter.h"
#include "srsgnb/phy/adapters/phy_timing_adapter.h"
#include "srsgnb/phy/lower/lower_phy_factory.h"
#include "srsgnb/phy/upper/upper_phy_timing_notifier.h"
#include "srsgnb/radio/radio_factory.h"
#include <atomic>
#include <csignal>

using namespace srsgnb;

/// \file
/// \brief Application of a co-located gNB with combined distributed unit (DU) and centralized unit (CU).
///
/// This application runs a gNB without the the F1 connection between CU and DU and without the E1 connection
/// between the CU-CP and CU-UP going over a real SCTP connection. However, its does expose the N2 and N3 interface
/// to the AMF and UPF over the standard SCTP ports.
/// The app serves as an example for a all-integrated, small-cell-style gNB.
///
/// \cond

/// From TS38.104 Section 5.3.2 Table 5.3.2-1. Default 20MHz FR1.
static const std::array<uint16_t, NOF_NUMEROLOGIES> nof_prb_ul_grid = {106, 51, 24, 0, 0};
static bool                                         printconfig     = false;
static std::string                                  config_file;

static srslog::basic_logger& gnb_logger = srslog::fetch_basic_logger("GNB");
static std::atomic<bool>     is_running = {true};
// NGAP configuration.
static srsgnb::network_gateway_config ngap_nw_config;
const std::string                     srsgnb_version = "0.1";

static void populate_cli11_gnb_args(CLI::App& app, srsgnb::gnb_appconfig& gnb_params)
{
  app.set_version_flag("-v,--version", srsgnb_version);
  app.set_config("-c,", config_file, "Read config from file", false);
  app.add_flag("--printconfig", printconfig, "Print configuration and exit")->configurable(false);
  app.add_flag("-d", gnb_params.debug_logger, "Enable debug logger");
}

static void populate_cli11_cu_args(CLI::App& app, srsgnb::gnb_appconfig& gnb_params)
{
  app.add_option("--amf_addr", gnb_params.cu_cfg.amf_cfg.amf_addr, "AMF IP address")->check(CLI::ValidIPV4)->required();
  app.add_option("--amf_port", gnb_params.cu_cfg.amf_cfg.amf_port, "AMF port")->capture_default_str();
  app.add_option(
         "--amf_bind_addr", gnb_params.cu_cfg.amf_cfg.amf_bind_addr, "Local IP IP address to bind for AMF connection")
      ->check(CLI::ValidIPV4);
}

/// This function takes the populated appconfig and generates (sub)-component configurations.
static void compute_derived_args(const gnb_appconfig& gnb_params)
{
  /// Simply set the respective values in the appconfig.
  ngap_nw_config.connect_address = gnb_params.cu_cfg.amf_cfg.amf_addr;
  ngap_nw_config.connect_port    = gnb_params.cu_cfg.amf_cfg.amf_port;
  ngap_nw_config.bind_address    = gnb_params.cu_cfg.amf_cfg.amf_bind_addr;
}

namespace {

/// Dummy implementation of the mac_result_notifier.
class phy_dummy : public mac_result_notifier
{
  mac_cell_result_notifier& cell;

public:
  explicit phy_dummy(mac_cell_result_notifier& cell) : cell(cell) {}

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
};

/// Manages the workers of the app.
struct worker_manager {
  static const uint32_t task_worker_queue_size = 128;

  void stop()
  {
    cu_ctrl_worker.stop();
    dl_workers.stop();
    ul_workers.stop();
    ctrl_worker.stop();
    rt_task_worker.stop();
    upper_dl_worker.stop();
    upper_ul_worker.stop();
    lower_prach_worker.stop();
    radio_worker.stop();
  }

  // CU worker and executers.
  task_worker                            cu_ctrl_worker{"CU Ctrl", task_worker_queue_size};
  static_vector<task_worker_executor, 1> cu_exec{{cu_ctrl_worker}};
  // DU workers and executers.
  task_worker              ctrl_worker{"Crtl-DU_UL", task_worker_queue_size};
  task_worker              dl_workers{"DU-DL#0", task_worker_queue_size};
  task_worker              ul_workers{"DU-UL#0", task_worker_queue_size};
  task_worker_executor     ctrl_exec{ctrl_worker};
  task_worker_executor     dl_execs{dl_workers};
  task_worker_executor     ul_execs{ul_workers};
  pcell_ul_executor_mapper ul_exec_mapper{&ul_execs};
  cell_dl_executor_mapper  dl_exec_mapper{&dl_execs};
  // Lower PHY RT task executor.
  task_worker          rt_task_worker{"phy_rt_thread", 1, false, os_thread_realtime_priority::MAX_PRIO};
  task_worker_executor rt_task_executor{{rt_task_worker}};
  // PRACH lower PHY executor
  task_worker          lower_prach_worker{"Lower PHY PRACH worker", task_worker_queue_size};
  task_worker_executor lower_prach_executor{lower_prach_worker};
  // Upper phy task executor
  task_worker          upper_dl_worker{"PHY DL worker", task_worker_queue_size};
  task_worker_executor upper_dl_executor{upper_dl_worker};
  task_worker          upper_ul_worker{"PHY UL worker", task_worker_queue_size};
  task_worker_executor upper_ul_executor{upper_ul_worker};
  // Radio task executor
  task_worker          radio_worker{"Radio worker", task_worker_queue_size};
  task_worker_executor radio_executor{radio_worker};
};

} // namespace

static lower_phy_configuration create_lower_phy_configuration(baseband_gateway*             bb_gateway,
                                                              lower_phy_rx_symbol_notifier* rx_symbol_notifier,
                                                              lower_phy_timing_notifier*    timing_notifier,
                                                              lower_phy_error_notifier*     error_notifier,
                                                              task_executor&                prach_executor,
                                                              const gnb_appconfig&          app_cfg)
{
  lower_phy_configuration phy_config = generate_ru_config(app_cfg);

  phy_config.bb_gateway           = bb_gateway;
  phy_config.error_notifier       = error_notifier;
  phy_config.rx_symbol_notifier   = rx_symbol_notifier;
  phy_config.timing_notifier      = timing_notifier;
  phy_config.prach_async_executor = &prach_executor;

  return phy_config;
}

static void signal_handler(int sig)
{
  fmt::print("Received signal {}\n", sig);
  is_running = false;
}

static fapi::prach_config generate_prach_config_tlv(const gnb_appconfig& app_config)
{
  const std::vector<du_cell_config>& cell_cfg = generate_du_cell_config(app_config);
  const du_cell_config&              cell     = cell_cfg.front();

  fapi::prach_config config     = {};
  config.prach_res_config_index = 0;
  config.prach_sequence_length  = fapi::prach_sequence_length_type::long_sequence;
  config.prach_scs              = prach_subcarrier_spacing::values::kHz1_25;
  config.prach_ul_bwp_pusch_scs = cell.scs_common;
  config.restricted_set         = restricted_set_config::UNRESTRICTED;
  config.num_prach_fd_occasions = cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_fdm;
  config.prach_config_index =
      cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index;
  config.prach_format           = fapi::prach_format_type::zero;
  config.num_prach_td_occasions = 1;
  config.num_preambles          = 1;
  config.start_preamble_index   = 0;

  // Add FD occasion info.
  config.fd_occasions.emplace_back();
  fapi::prach_fd_occasion_config& fd_occasion = config.fd_occasions.back();
  fd_occasion.prach_root_sequence_index = cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().prach_root_seq_index;
  fd_occasion.prach_freq_offset =
      cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_frequency_start;
  fd_occasion.prach_zero_corr_conf =
      cell.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.zero_correlation_zone_config;

  return config;
}

static fapi::carrier_config generate_carrier_config_tlv()
{
  fapi::carrier_config config = {};

  // NOTE; for now we only need to fill the nof_prb_ul_grid.
  // :TODO: extract this to app config.
  config.ul_grid_size = nof_prb_ul_grid;

  return config;
}

static std::unique_ptr<radio_session>
build_radio(task_executor& executor, radio_notification_handler& radio_handler, const gnb_appconfig& config)
{
  std::unique_ptr<radio_factory> factory = create_radio_factory(config.rf_driver_cfg.device_driver);
  report_fatal_error_if_not(factory, "Invalid radio factory.");

  // Create radio configuration. Assume 1 sector per stream.
  radio_configuration::radio radio_config = generate_radio_config(config);
  return factory->create(radio_config, executor, radio_handler);
}

int main(int argc, char** argv)
{
  // Setup and configure config parsing.
  CLI::App app("srsGNB application");
  app.config_formatter(create_json_config_parser());

  gnb_appconfig gnb_cfg;
  // This variable is filled by the command line parser.
  populate_cli11_gnb_args(app, gnb_cfg);
  auto cu_app = app.add_subcommand("cu", "CU parameters");
  populate_cli11_cu_args(*cu_app, gnb_cfg);

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  // Check the modified configuration.
  if (!validate_appconfig(gnb_cfg)) {
    srsgnb_terminate("Invalid configuration detected");
  }

  // Compute derived parameters.
  compute_derived_args(gnb_cfg);

  srslog::init();
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);
  gnb_logger.set_level(srslog::basic_levels::info);

  worker_manager workers;

  f1c_local_adapter f1c_cu_to_du_adapter("f1-cu2du"), f1c_du_to_cu_adapter("f1-du2cu");

  // Create IO broker.
  std::unique_ptr<io_broker> epoll_broker = create_io_broker(io_broker_type::epoll);

  // Create NGAP adapter, NGC and connect them with one another.
  std::unique_ptr<srsgnb::srs_cu_cp::ngap_network_adapter> ngap_adapter =
      std::make_unique<srsgnb::srs_cu_cp::ngap_network_adapter>(*epoll_broker, ngap_nw_config);

  // Create CU-CP config.
  srs_cu_cp::cu_cp_configuration cu_cfg = generate_cu_cp_config(gnb_cfg);
  cu_cfg.cu_executor                    = &workers.cu_exec.front();
  cu_cfg.f1c_notifier                   = &f1c_cu_to_du_adapter;
  cu_cfg.ngc_notifier                   = ngap_adapter.get();

  // create and start CU.
  std::unique_ptr<srsgnb::srs_cu_cp::cu_cp_interface> cu_cp_obj = create_cu_cp(std::move(cu_cfg));
  cu_cp_obj->on_new_connection(); // trigger DU addition

  // Connect NGAP adpter to CU-CP to pass NGC messages.
  ngap_adapter->connect_ngc(&cu_cp_obj->get_ngc_message_handler(), &cu_cp_obj->get_ngc_event_handler());

  // Create radio.
  radio_notification_handler_printer radio_event_printer;
  auto                               radio = build_radio(workers.radio_executor, radio_event_printer, gnb_cfg);
  report_fatal_error_if_not(radio, "Unable to create radio session.");
  gnb_logger.info("Radio driver '{}' created successfully", gnb_cfg.rf_driver_cfg.device_driver);

  // Create lower and upper PHY adapters.
  phy_error_adapter             phy_err_printer("info");
  phy_rx_symbol_adapter         phy_rx_adapter;
  phy_rg_gateway_adapter        rg_gateway_adapter;
  phy_timing_adapter            phy_time_adapter;
  phy_rx_symbol_request_adapter phy_rx_symbol_req_adapter;

  // Create lower PHY.
  lower_phy_configuration   lower_phy_config = create_lower_phy_configuration(&radio->get_baseband_gateway(),
                                                                            &phy_rx_adapter,
                                                                            &phy_time_adapter,
                                                                            &phy_err_printer,
                                                                            workers.lower_prach_executor,
                                                                            gnb_cfg);
  static constexpr unsigned max_nof_prach_concurrent_requests = 11;
  auto                      lower = create_lower_phy(lower_phy_config, max_nof_prach_concurrent_requests);
  report_fatal_error_if_not(lower, "Unable to create lower PHY.");
  gnb_logger.info("Lower PHY created successfully");

  auto upper = create_upper_phy(
      gnb_cfg, &rg_gateway_adapter, &workers.upper_dl_executor, &workers.upper_ul_executor, &phy_rx_symbol_req_adapter);
  report_fatal_error_if_not(upper, "Unable to create upper PHY.");
  gnb_logger.info("Upper PHY created successfully");

  // Make connections between upper and lower PHYs.
  phy_rx_adapter.connect(&upper->get_rx_symbol_handler());
  phy_time_adapter.connect(&upper->get_timing_handler());
  rg_gateway_adapter.connect(&lower->get_rg_handler());
  phy_rx_symbol_req_adapter.connect(&lower->get_request_handler());

  // Create FAPI adaptors.
  srsgnb_assert(gnb_cfg.cells_cfg.size() == 1, "Error, currently supporting one cell");
  const rf_cell& cell = gnb_cfg.cells_cfg.front().rf;

  const unsigned sector_id   = 0;
  auto           phy_adaptor = build_phy_fapi_adaptor(sector_id,
                                            cell.scs_common,
                                            cell.scs_common,
                                            upper->get_downlink_processor_pool(),
                                            upper->get_downlink_resource_grid_pool(),
                                            upper->get_uplink_request_processor(),
                                            upper->get_uplink_resource_grid_pool(),
                                            upper->get_uplink_slot_pdu_repository(),
                                            generate_prach_config_tlv(gnb_cfg),
                                            generate_carrier_config_tlv());
  report_fatal_error_if_not(phy_adaptor, "Unable to create PHY adaptor.");
  upper->set_results_notifier(phy_adaptor->get_rx_results_notifier());
  auto mac_adaptor = build_mac_fapi_adaptor(0, cell.scs_common, phy_adaptor->get_slot_message_gateway());
  report_fatal_error_if_not(mac_adaptor, "Unable to create MAC adaptor.");
  phy_adaptor->set_slot_time_message_notifier(mac_adaptor->get_slot_time_notifier());
  phy_adaptor->set_slot_data_message_notifier(mac_adaptor->get_slot_data_notifier());
  upper->set_timing_notifier(phy_adaptor->get_timing_notifier());
  gnb_logger.info("FAPI adaptors created successfully");

  // Cell configuration.
  phy_dummy phy(mac_adaptor->get_cell_result_notifier());

  srs_du::du_high_configuration du_hi_cfg = {};
  du_hi_cfg.du_mng_executor               = &workers.ctrl_exec;
  du_hi_cfg.ul_executors                  = &workers.ul_exec_mapper;
  du_hi_cfg.dl_executors                  = &workers.dl_exec_mapper;
  du_hi_cfg.f1c_notifier                  = &f1c_du_to_cu_adapter;
  du_hi_cfg.phy_adapter                   = &phy;
  du_hi_cfg.cells                         = generate_du_cell_config(gnb_cfg);

  srs_du::du_high du_obj(du_hi_cfg);
  gnb_logger.info("DU-High created successfully");

  // Set signal handler.
  ::signal(SIGINT, signal_handler);
  ::signal(SIGTERM, signal_handler);
  ::signal(SIGHUP, signal_handler);
  ::signal(SIGQUIT, signal_handler);
  ::signal(SIGKILL, signal_handler);

  // attach F1C adapter to DU and CU-CP
  f1c_du_to_cu_adapter.attach_handler(&cu_cp_obj->get_f1c_message_handler(srsgnb::srs_cu_cp::int_to_du_index(0)));
  f1c_cu_to_du_adapter.attach_handler(&du_obj.get_f1c_message_handler());

  // Start execution.
  gnb_logger.info("Starting DU-High...");
  du_obj.start();
  gnb_logger.info("DU-High started successfully");

  // Give some time to the MAC to start.
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // Configure the DU slot handler.
  du_cell_index_t cell_id = to_du_cell_index(0);
  mac_adaptor->set_cell_slot_handler(du_obj.get_slot_handler(cell_id));
  mac_adaptor->set_cell_rach_handler(du_obj.get_rach_handler(cell_id));
  mac_adaptor->set_cell_pdu_handler(du_obj.get_pdu_handler(cell_id));
  mac_adaptor->set_cell_crc_handler(du_obj.get_control_information_handler(cell_id));

  // Start processing.
  gnb_logger.info("Starting lower PHY...");
  lower->get_controller().start(workers.rt_task_executor);
  gnb_logger.info("Lower PHY started successfully");

  while (is_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  gnb_logger.info("Stopping lower PHY...");
  lower->get_controller().stop();
  gnb_logger.info("Lower PHY stopped successfully");

  gnb_logger.info("Stopping executors...");
  workers.stop();
  gnb_logger.info("Executors stopped successfully");

  srslog::flush();

  return 0;
}

/// \endcond
