/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/du_high/du_high.h"
#include "../../../lib/du_high/du_high_executor_strategies.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/fapi_adaptor/mac/mac_fapi_adaptor_factory.h"
#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"
#include "srsgnb/phy/adapters/phy_error_adapter.h"
#include "srsgnb/phy/adapters/phy_rg_gateway_adapter.h"
#include "srsgnb/phy/adapters/phy_rx_symbol_adapter.h"
#include "srsgnb/phy/adapters/phy_timing_adapter.h"
#include "srsgnb/phy/lower/lower_phy_factory.h"
#include "srsgnb/phy/support/support_factories.h"
#include "srsgnb/phy/upper/downlink_processor.h"
#include "srsgnb/phy/upper/upper_phy_factories.h"
#include "srsgnb/phy/upper/upper_phy_rg_gateway.h"
#include "srsgnb/phy/upper/upper_phy_timing_notifier.h"
#include "srsgnb/radio/radio_factory.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/test_utils.h"
#include <atomic>
#include <csignal>
#include <getopt.h>

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;
using namespace srs_du;

/// \file
/// \brief Example application of a distributed unit (DU) transmitting SIB1 over a radio interface.
///
/// This example runs a DU without an F1 connection to a CU. It integrates the DU high and DU low subcomponents and
/// connects them through the FAPI interface, the DU low is then connected to the lower PHY and a real-time radio
/// interface. The example transmits SIB1 messages.
///
/// The application supports different working profiles, run <tt> du_example -h </tt> for usage details.
/// \cond

namespace {

/// Realtime radio event printer.
class radio_notification_handler_printer : public radio_notification_handler
{
public:
  void on_radio_rt_event(const event_description& description) override
  {
    fmt::print("Radio Realtime Error Event: Type: {} Source: {}\n",
               description.type.to_string(),
               description.source.to_string());
  }
};

/// This implementation returns back to the F1 interface a fake F1 Setup Response message upon the receival of the F1
/// Setup Request message.
class fake_cucp_handler : public f1c_message_notifier
{
public:
  explicit fake_cucp_handler(f1c_message_handler* handler_ = nullptr) : handler(handler_) {}

  void attach_handler(f1c_message_handler* handler_) { handler = handler_; };

  void on_new_message(const f1c_msg& msg) override
  {
    if (msg.pdu.type() == asn1::f1ap::f1_ap_pdu_c::types::init_msg) {
      // Generate a fake F1 Setup response message and pass it back to the DU.
      f1c_msg response;

      response.pdu.set_successful_outcome();
      response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

      auto& setup_res = response.pdu.successful_outcome().value.f1_setup_resp();
      // Use the same transaction ID as in the request message.
      setup_res->transaction_id.value = msg.pdu.init_msg().value.f1_setup_request()->transaction_id.value;
      setup_res->gnb_cu_name_present  = true;
      setup_res->gnb_cu_name.value.from_string("srsCU");
      setup_res->gnb_cu_rrc_version.value.latest_rrc_version.from_number(2);

      handler->handle_message(response);
    }
  }

private:
  f1c_message_handler* handler = nullptr;
};

/// Dummy implementation of the mac_result_notifier.
struct phy_dummy : public mac_result_notifier {
  mac_cell_result_notifier& cell;

public:
  explicit phy_dummy(mac_cell_result_notifier& cell) : cell(cell) {}

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
};

/// Manages the workers of the app.
struct worker_manager {
  static const uint32_t task_worker_queue_size = 10000;

  void stop()
  {
    for (auto& w : dl_workers) {
      w.stop();
    }

    ctrl_worker.stop();
    rt_task_worker.stop();
    upper_worker.stop();
    radio_worker.stop();
  }

  task_worker                            ctrl_worker{"Crtl-DU_UL", task_worker_queue_size};
  task_worker                            dl_workers[1] = {{"DU-DL#0", task_worker_queue_size}};
  static_vector<task_worker_executor, 1> dl_execs{{dl_workers[0]}};
  static_vector<task_worker_executor, 1> ul_execs{{ctrl_worker}};
  pcell_ul_executor_mapper               ul_exec_mapper{&ul_execs[0]};
  cell_dl_executor_mapper                dl_exec_mapper{&dl_execs[0]};
  // Lower PHY RT task executor.
  task_worker          rt_task_worker{"phy_rt_thread", 1, false, os_thread_realtime_priority::MAX_PRIO};
  task_worker_executor rt_task_executor{{rt_task_worker}};
  // Upper phy task executor
  task_worker          upper_worker{"PHY worker", task_worker_queue_size};
  task_worker_executor upper_executor{upper_worker};
  // Radio task executor
  task_worker          radio_worker{"Radio worker", task_worker_queue_size};
  task_worker_executor radio_executor{radio_worker};
};

struct configuration_profile {
  std::string           name;
  std::string           description;
  std::function<void()> function;
};

} // namespace

/// Program parameters.
static pci_t                                     pci                        = 1;
static const unsigned                            sector_id                  = 0;
static subcarrier_spacing                        scs                        = subcarrier_spacing::kHz15;
static subcarrier_spacing                        scs_common                 = subcarrier_spacing::kHz15;
static const double                              tx_gain                    = 60.0;
static const unsigned                            max_processing_delay_slots = 4;
static const unsigned                            ul_to_dl_slot_offset       = 1;
static const cyclic_prefix                       cp                         = cyclic_prefix::NORMAL;
static double                                    dl_center_freq             = 2680.1e6;
static unsigned                                  dl_arfcn                   = 536020;
static const double                              rx_freq                    = 3.5e9;
static const double                              rx_gain                    = 60.0;
static const unsigned                            nof_ports                  = 1;
static const unsigned                            nof_sectors                = 1;
static std::string                               driver_name                = "zmq";
static std::vector<std::string>                  tx_channel_args;
static std::vector<std::string>                  rx_channel_args;
static double                                    sampling_rate_hz = 61.44e6;
static unsigned                                  bw_rb            = 106;
static unsigned                                  offset_to_pointA = 40;
static unsigned                                  K_ssb            = 6;
static unsigned                                  coreset0_index   = 6;
static radio_configuration::clock_sources        clock_src        = {};
static std::string                               log_level        = "info";
static radio_configuration::over_the_wire_format otw_format       = radio_configuration::over_the_wire_format::DEFAULT;
static std::string                               device_arguments;
static std::atomic<bool>                         is_running = {true};

/// Defines a set of configuration profiles.
static const std::vector<configuration_profile> profiles = {
    {"zmq_20MHz_n7",
     "Single 20MHz FDD in band n7 using ZMQ.",
     []() {
       driver_name      = "zmq";
       device_arguments = "";
       scs              = subcarrier_spacing::kHz15;
       scs_common       = subcarrier_spacing::kHz15;
       sampling_rate_hz = 61.44e6;
       dl_arfcn         = 536020;
       K_ssb            = 6;
       offset_to_pointA = 40;
       bw_rb            = 106;
       pci              = 69;
       coreset0_index   = 9;
       otw_format       = radio_configuration::over_the_wire_format::DEFAULT;
       dl_center_freq   = 2680.1e6;
       // Prepare ZMQ addresses using TCP. Transmitter ports start at 5000+channel_id while receivers connect to
       // 6000+channel_id.
       unsigned port_base   = 5000;
       unsigned port_offset = 1000;
       for (unsigned channel_id = 0; channel_id != nof_ports * nof_sectors; ++channel_id) {
         tx_channel_args.emplace_back("tcp://*:" + std::to_string(port_base + channel_id));
         rx_channel_args.emplace_back("tcp://localhost:" + std::to_string(port_base + channel_id + port_offset));
       }
     }},
    {"uhd_20MHz_n7",
     "Single 20MHz FDD in band n7 using ZMQ.",
     []() {
       driver_name      = "uhd";
       device_arguments = "type=b200";
       scs              = subcarrier_spacing::kHz15;
       scs_common       = subcarrier_spacing::kHz15;
       sampling_rate_hz = 23.04e6;
       dl_arfcn         = 536020;
       K_ssb            = 6;
       offset_to_pointA = 40;
       bw_rb            = 106;
       pci              = 1;
       otw_format       = radio_configuration::over_the_wire_format::SC12;
       dl_center_freq   = 2680.1e6;
       clock_src.clock  = radio_configuration::clock_sources::source::INTERNAL;
       clock_src.sync   = radio_configuration::clock_sources::source::INTERNAL;
     }},
    {"zmq_20MHz_n41", "Single 20MHz TDD in band n41 using ZMQ.", []() {
       driver_name      = "zmq";
       device_arguments = "";
       scs              = subcarrier_spacing::kHz15;
       scs_common       = subcarrier_spacing::kHz15;
       sampling_rate_hz = 61.44e6;
       bw_rb            = 106;
       dl_arfcn         = 520000;
       coreset0_index   = 8;
       K_ssb            = 7;
       offset_to_pointA = 69;
       otw_format       = radio_configuration::over_the_wire_format::DEFAULT;
       dl_center_freq   = 2600.e6;
       // Prepare ZMQ addresses using TCP. Transmitter ports start at 5000+channel_id while receivers connect to
       // 6000+channel_id.
       unsigned port_base   = 5000;
       unsigned port_offset = 1000;
       for (unsigned channel_id = 0; channel_id != nof_ports * nof_sectors; ++channel_id) {
         tx_channel_args.emplace_back("tcp://*:" + std::to_string(port_base + channel_id));
         rx_channel_args.emplace_back("tcp://localhost:" + std::to_string(port_base + channel_id + port_offset));
       }
     }}};

static std::unique_ptr<resource_grid_pool> build_ul_resource_grid_pool()
{
  unsigned                                    nof_slots = max_processing_delay_slots * 2;
  std::vector<std::unique_ptr<resource_grid>> grids;
  grids.reserve(nof_sectors * nof_slots);

  for (unsigned sector_idx = 0; sector_idx != nof_sectors; ++sector_idx) {
    for (unsigned slot_id = 0; slot_id != nof_slots; ++slot_id) {
      std::unique_ptr<resource_grid> grid = create_resource_grid(1, get_nsymb_per_slot(cp), bw_rb * NRE);
      if (!grid) {
        srslog::fetch_basic_logger("TEST").error("Failed to create UL resource grid");
        return nullptr;
      }
      grids.emplace_back(std::move(grid));
    }
  }

  // Create UL resource grid pool.
  return create_resource_grid_pool(nof_sectors, nof_slots, std::move(grids));
}

static lower_phy_configuration create_lower_phy_config(baseband_gateway&             bb_gateway,
                                                       lower_phy_rx_symbol_notifier& rx_symbol_notifier,
                                                       lower_phy_timing_notifier&    timing_notifier,
                                                       lower_phy_error_notifier&     error_notifier,
                                                       resource_grid_pool&           ul_resource_grid_pool)
{
  // Derived parameters.
  unsigned dft_size_15kHz = static_cast<unsigned>(sampling_rate_hz / 15e3);
  float    tx_scale       = 1.0F / std::sqrt(NRE * bw_rb);
  double   rx_to_tx_delay = static_cast<double>(ul_to_dl_slot_offset * 1e-3) / pow2(to_numerology_value(scs));

  lower_phy_configuration phy_config;
  phy_config.dft_size_15kHz             = dft_size_15kHz;
  phy_config.scs                        = scs;
  phy_config.max_processing_delay_slots = max_processing_delay_slots;
  phy_config.ul_to_dl_slot_offset       = ul_to_dl_slot_offset;
  phy_config.rx_to_tx_delay             = rx_to_tx_delay;
  phy_config.tx_scale                   = tx_scale;
  phy_config.cp                         = cp;
  phy_config.bb_gateway                 = &bb_gateway;
  phy_config.rx_symbol_notifier         = &rx_symbol_notifier;
  phy_config.timing_notifier            = &timing_notifier;
  phy_config.error_notifier             = &error_notifier;

  for (unsigned sector_idx = 0; sector_idx != nof_sectors; ++sector_idx) {
    lower_phy_sector_description sector_config;
    sector_config.bandwidth_rb = bw_rb;
    sector_config.dl_freq_hz   = dl_center_freq;
    sector_config.ul_freq_hz   = rx_freq;
    for (unsigned port_id = 0; port_id < nof_ports; ++port_id) {
      lower_phy_sector_port_mapping port_mapping;
      port_mapping.stream_id  = sector_id;
      port_mapping.channel_id = port_id;
      sector_config.port_mapping.push_back(port_mapping);
    }
    phy_config.sectors.push_back(sector_config);
    phy_config.nof_channels_per_stream.push_back(nof_ports);
  }
  phy_config.log_level = log_level;

  return phy_config;
}

static std::unique_ptr<lower_phy> build_lower_phy(baseband_gateway&             bb_gateway,
                                                  lower_phy_rx_symbol_notifier& rx_symbol_notifier,
                                                  lower_phy_timing_notifier&    timing_notifier,
                                                  lower_phy_error_notifier&     error_notifier,
                                                  resource_grid_pool&           ul_resource_grid_pool)
{
  // Create DFT factory. It tries to create a FFTW based factory. If FFTW library is not available, it creates a FFTX
  // based factory.
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw();
  if (dft_factory == nullptr) {
    srslog::fetch_basic_logger("TEST").error("Failed to create FFTW processor factory. Trying with FFTX");
    dft_factory = create_dft_processor_factory_fftx();
    if (dft_factory == nullptr) {
      srslog::fetch_basic_logger("TEST").error("Failed to create FFTX processor factory");
      return nullptr;
    }
  }

  // Create OFDM modulator factory.
  ofdm_factory_generic_configuration ofdm_common_config;
  ofdm_common_config.dft_factory                            = dft_factory;
  std::shared_ptr<ofdm_modulator_factory> modulator_factory = create_ofdm_modulator_factory_generic(ofdm_common_config);
  if (!modulator_factory) {
    srslog::fetch_basic_logger("TEST").error("Failed to create OFDM modulator factory");
    return nullptr;
  }

  // Create OFDM demodulator factory.
  std::shared_ptr<ofdm_demodulator_factory> demodulator_factory =
      create_ofdm_demodulator_factory_generic(ofdm_common_config);
  if (!demodulator_factory) {
    srslog::fetch_basic_logger("TEST").error("Failed to create OFDM demodulator factory");

    return nullptr;
  }

#if 0
  // Create OFDM PRACH demodulator factory.
  std::shared_ptr<ofdm_prach_demodulator_factory> prach_demodulator_factory =
      create_ofdm_prach_demodulator_factory_sw(dft_factory, dft_size_15kHz, nof_prb_ul_grid);
  if (!demodulator_factory) {
    srslog::fetch_basic_logger("TEST").error("Failed to create OFDM PRACH demodulator factory");

    return nullptr;
  }

  // Create PRACH processor factory.
  std::shared_ptr<prach_processor_factory> prach_factory =
      create_prach_processor_factory_sw(prach_demodulator_factory, dft_size_15kHz, max_nof_concurrent_requests);
  if (!demodulator_factory) {
    srslog::fetch_basic_logger("TEST").error("Failed to create PRACH processor factory");
    return nullptr;
  }
#else
  std::shared_ptr<prach_processor_factory> prach_factory = nullptr;
#endif

  // Create Lower PHY factory.
  std::shared_ptr<lower_phy_factory> lphy_factory =
      create_lower_phy_factory_sw(modulator_factory, demodulator_factory, prach_factory);
  if (!lphy_factory) {
    srslog::fetch_basic_logger("TEST").error("Failed to create lower PHY factory");

    return nullptr;
  }

  lower_phy_configuration config =
      create_lower_phy_config(bb_gateway, rx_symbol_notifier, timing_notifier, error_notifier, ul_resource_grid_pool);

  return lphy_factory->create(config);
}

static radio_configuration::radio create_radio_configuration()
{
  radio_configuration::radio radio_config;
  radio_config.clock            = clock_src;
  radio_config.sampling_rate_hz = sampling_rate_hz;
  radio_config.otw_format       = otw_format;
  radio_config.args             = device_arguments;
  radio_config.log_level        = log_level;

  for (unsigned sector_idx = 0; sector_idx != nof_sectors; ++sector_idx) {
    // For each channel in the streams...
    radio_configuration::stream tx_stream_config;
    radio_configuration::stream rx_stream_config;
    for (unsigned port_id = 0; port_id != nof_ports; ++port_id) {
      // Create channel configuration and append it to the previous ones.
      radio_configuration::channel tx_ch_config;
      tx_ch_config.freq.center_frequency_hz = dl_center_freq;
      tx_ch_config.gain_dB                  = tx_gain;
      if (!tx_channel_args.empty()) {
        tx_ch_config.args = tx_channel_args[sector_idx * nof_ports + port_id];
      }
      tx_stream_config.channels.emplace_back(tx_ch_config);

      radio_configuration::channel rx_ch_config;
      rx_ch_config.freq.center_frequency_hz = rx_freq;
      rx_ch_config.gain_dB                  = rx_gain;
      if (!rx_channel_args.empty()) {
        rx_ch_config.args = rx_channel_args[sector_id * nof_ports + port_id];
      }
      rx_stream_config.channels.emplace_back(rx_ch_config);
    }
    radio_config.tx_streams.emplace_back(tx_stream_config);
    radio_config.rx_streams.emplace_back(rx_stream_config);
  }
  return radio_config;
}

static std::unique_ptr<radio_session>
create_radio(const std::string& radio_driver, task_executor& executor, radio_notification_handler& radio_handler)
{
  std::unique_ptr<radio_factory> factory = create_radio_factory(radio_driver);
  if (!factory) {
    srslog::fetch_basic_logger("TEST").error("Failed to create radio factory");

    return nullptr;
  }

  // Create radio configuration. Assume 1 sector per stream.
  radio_configuration::radio radio_config = create_radio_configuration();

  return factory->create(radio_config, executor, radio_handler);
}

static std::unique_ptr<phy_fapi_adaptor> build_phy_fapi_adaptor(downlink_processor_pool& dl_processor_pool,
                                                                resource_grid_pool&      dl_rg_pool)
{
  std::unique_ptr<phy_fapi_adaptor_factory> phy_factory =
      create_phy_fapi_adaptor_factory(dl_processor_pool, dl_rg_pool);

  phy_fapi_adaptor_factory_config phy_fapi_config;
  phy_fapi_config.sector_id  = sector_id;
  phy_fapi_config.scs_common = scs_common;

  return phy_factory->create(phy_fapi_config);
}

static std::unique_ptr<mac_fapi_adaptor> build_mac_fapi_adaptor(slot_message_gateway& gateway)
{
  std::unique_ptr<mac_fapi_adaptor_factory> mac_factory = create_mac_fapi_adaptor_factory();
  mac_fapi_adaptor_factory_config           mac_fapi_config(sector_id, scs, gateway);

  return mac_factory->create(mac_fapi_config);
}

static std::unique_ptr<upper_phy> build_upper(upper_phy_rg_gateway& gateway, task_executor& executor)
{
  // Upper.
  std::unique_ptr<upper_phy_factory> up_phy_factory = create_upper_phy_factory();

  upper_phy_config upper_config;
  upper_config.sector_id         = 1;
  upper_config.nof_ports         = 1;
  upper_config.nof_slots_dl_rg   = 40;
  upper_config.nof_dl_processors = 10;
  upper_config.dl_bw_rb          = bw_rb;
  upper_config.gateway           = &gateway;
  upper_config.dl_executor       = &executor;

  return up_phy_factory->create(upper_config);
}

static void signal_handler(int sig)
{
  fmt::print("Received signal {}\n", sig);
  is_running = false;
}

static void usage(std::string prog)
{
  fmt::print("Usage: {} [-P profile] [-D duration] [-v level] \n", prog);
  fmt::print("\t-P Profile. [Default {}]\n", profiles.front().name);
  for (const configuration_profile& profile : profiles) {
    fmt::print("\t\t {:<30}{}\n", profile.name, profile.description);
  }
  fmt::print("\t-v Logging level. [Default {}]\n", log_level);
  fmt::print("\t-h print this message.\n");
}

static int parse_args(int argc, char** argv)
{
  std::string profile_name;

  int opt = 0;
  while ((opt = getopt(argc, argv, "D:P:L:v:h")) != -1) {
    switch (opt) {
      case 'P':
        if (optarg != nullptr) {
          profile_name = std::string(optarg);
        }
        break;
      case 'v':
        log_level = std::string(optarg);
        break;
      case 'h':
      default:
        usage(argv[0]);
        return -1;
    }
  }

  // Search profile if set.
  if (!profile_name.empty()) {
    bool found = false;
    for (const auto& profile : profiles) {
      if (profile_name == profile.name) {
        profile.function();
        srslog::fetch_basic_logger("TEST").info("Loading profile: {}", profile.name);
        found = true;
        break;
      }
    }
    if (!found) {
      usage(argv[0]);
      srslog::fetch_basic_logger("TEST").error("Invalid profile: {}.", profile_name);
      return -1;
    }
  } else {
    const configuration_profile& default_profile = profiles.front();

    srslog::fetch_basic_logger("TEST").info("Loading default profile: {}", default_profile.name);
    default_profile.function();
  }

  return 0;
}

int main(int argc, char** argv)
{
  srslog::init();
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
  test_logger.set_level(srslog::basic_levels::info);

  // Parse arguments.
  int ret = parse_args(argc, argv);
  if (ret < 0) {
    return ret;
  }

  worker_manager workers;

  // Create radio.
  test_logger.info("Creating radio driver for {} ...", driver_name);
  radio_notification_handler_printer radio_error_printer;
  std::unique_ptr<radio_session>     radio = create_radio(driver_name, workers.radio_executor, radio_error_printer);
  if (!radio) {
    test_logger.error("Failed to create radio session");
    return -1;
  }

  test_logger.info("Radio session created successfully");

  // Create UL resource grid pool.
  std::unique_ptr<resource_grid_pool> ul_rg_pool = build_ul_resource_grid_pool();
  if (!ul_rg_pool) {
    test_logger.error("Failed to create UL Resource grid pool");
    return -1;
  }

  // Create Lower-Upper adapters.
  phy_error_adapter      error_adapter(log_level);
  phy_rx_symbol_adapter  rx_symbol_adapter;
  phy_rg_gateway_adapter rg_gateway_adapter;
  phy_timing_adapter     timing_adapter;

  // Create lower PHY.
  test_logger.info("Creating lower PHY object...");
  std::unique_ptr<lower_phy> lower =
      build_lower_phy(radio->get_baseband_gateway(), rx_symbol_adapter, timing_adapter, error_adapter, *ul_rg_pool);
  if (!lower) {
    test_logger.error("Failed to create Lower PHY");
    return -1;
  }
  test_logger.info("Lower PHY created successfully");

  test_logger.info("Creating upper PHY object...");
  std::unique_ptr<upper_phy> upper = build_upper(rg_gateway_adapter, workers.upper_executor);
  if (!upper) {
    test_logger.error("Failed to create Upper PHY");
    return -1;
  }
  test_logger.info("Upper PHY created successfully");

  // Connections between lower-upper PHYs.
  rx_symbol_adapter.connect(&upper->get_rx_symbol_handler());
  timing_adapter.connect(&upper->get_timing_handler());
  rg_gateway_adapter.connect(&lower->get_rg_handler());

  // Create FAPI adaptors.
  test_logger.info("Creating FAPI adaptors...");
  std::unique_ptr<phy_fapi_adaptor> phy_adaptor =
      build_phy_fapi_adaptor(upper->get_downlink_processor_pool(), upper->get_downlink_resource_grid_pool());
  if (!phy_adaptor) {
    test_logger.error("Failed to create PHY-FAPI adaptor");

    return -1;
  }

  std::unique_ptr<mac_fapi_adaptor> mac_adaptor = build_mac_fapi_adaptor(phy_adaptor->get_slot_message_gateway());
  if (!mac_adaptor) {
    test_logger.error("Failed to create MAC-FAPI adaptor");
    return -1;
  }
  test_logger.info("FAPI adaptors created successfully");

  phy_adaptor->set_fapi_slot_time_message_notifier(mac_adaptor->get_slot_time_notifier());
  upper->set_timing_notifier(phy_adaptor->get_upper_phy_timing_notifier());

  // Configuration for the cell.
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);

  struct du_cell_config_default_params default_config;
  default_config.pci               = pci;
  default_config.scs_common        = scs_common;
  default_config.nof_crbs          = bw_rb;
  default_config.dl_arfcn          = dl_arfcn;
  default_config.offset_to_point_a = offset_to_pointA;
  default_config.coreset0_index    = coreset0_index;

  fake_cucp_handler notifier;
  phy_dummy         phy(mac_adaptor->get_cell_result_notifier());

  du_high_configuration cfg = {};
  cfg.du_mng_executor       = &workers.ul_execs[0];
  cfg.dl_executors          = &workers.dl_exec_mapper;
  cfg.ul_executors          = &workers.ul_exec_mapper;
  cfg.f1c_notifier          = &notifier;
  cfg.phy_adapter           = &phy;
  cfg.cells                 = {du_config_helpers::make_default_du_cell_config(default_config)};

  du_cell_config& cell_cfg = cfg.cells.back();
  cell_cfg.ssb_cfg.k_ssb   = K_ssb;

  test_logger.info("Creating DU high object...");
  du_high du_obj(cfg);
  notifier.attach_handler(&du_obj.get_f1c_message_handler());
  test_logger.info("DU high created successfully");

  // Configure the DU slot handler.
  mac_adaptor->set_cell_slot_handler(du_obj.get_slot_handler(static_cast<du_cell_index_t>(0)));

  // Set signal handler.
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGHUP, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGKILL, signal_handler);

  // Start execution.
  test_logger.info("Starting DU high...");
  du_obj.start();
  test_logger.info("DU high start --> OK");

  // Give some time to the MAC to start.
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // Start processing.
  test_logger.info("Starting lower PHY...");
  lower->get_controller().start(workers.rt_task_executor);
  test_logger.info("Lower PHY start --> OK");

  while (is_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  test_logger.info("Stopping lower PHY...");
  lower->get_controller().stop();
  test_logger.info("Lower PHY successfully stopped");

  test_logger.info("Stopping workers...");
  workers.stop();
  test_logger.info("Workers successfully stopped");

  srslog::flush();

  return 0;
}

/// \endcond
