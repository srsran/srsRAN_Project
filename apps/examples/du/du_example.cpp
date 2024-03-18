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

#include "../../../lib/du_high/du_high_executor_strategies.h"
#include "fapi_factory.h"
#include "phy_factory.h"
#include "radio_notifier_sample.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/asn1/rrc_nr/dl_ccch_msg.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg_ies.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du_high/du_high_factory.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/fapi/logging_decorator_factories.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor_factory.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_generator.h"
#include "srsran/phy/upper/upper_phy_timing_notifier.h"
#include "srsran/ru/ru_adapters.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/ru/ru_generic_configuration.h"
#include "srsran/ru/ru_generic_factory.h"
#include "srsran/support/executors/task_worker.h"
#include <atomic>
#include <csignal>
#include <getopt.h>

using namespace srsran;
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

struct configuration_profile {
  std::string           name;
  std::string           description;
  std::function<void()> function;
};

} // namespace

/// Cell configuration parameters.
static const pci_t              pci            = 55;
static nr_band                  band           = nr_band::n3;
static const subcarrier_spacing scs            = subcarrier_spacing::kHz15;
static const cyclic_prefix      cp             = cyclic_prefix::NORMAL;
static unsigned                 dl_arfcn       = 536020;
static bs_channel_bandwidth_fr1 channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;

/// Radio configuration parameters.
static std::string                               driver_name = "zmq";
static std::string                               rx_address  = "tcp://localhost:6000";
static std::string                               tx_address  = "tcp://*:5000";
static unsigned                                  num_tx_ant  = 1;
static unsigned                                  num_rx_ant  = 1;
static std::string                               device_arguments;
static std::vector<std::string>                  tx_channel_args;
static std::vector<std::string>                  rx_channel_args;
static radio_configuration::over_the_wire_format otw_format = radio_configuration::over_the_wire_format::DEFAULT;
static radio_configuration::clock_sources        clock_src  = {};
static sampling_rate                             srate      = sampling_rate::from_MHz(61.44);
static int                                       time_alignmemt_calibration = 0;
static double                                    tx_gain                    = 60.0;
static double                                    rx_gain                    = 70.0;

/// SSB configuration parameters.
static unsigned       offset_to_pointA = 40;
static unsigned       K_ssb            = 6;
static const unsigned coreset0_index   = 6;

/// Logger configuration parameters.
static std::string           log_level        = "info";
static bool                  enable_fapi_logs = false;
static srslog::basic_logger& du_logger        = srslog::fetch_basic_logger("DU_APP");

/// PRACH configuration parameters.
static unsigned       prach_msg1_freq_offset    = 3;
static unsigned       prach_root_sequence_index = 1;
static const unsigned zero_correlation_zone     = 15;
static const unsigned prach_config_index        = 1;
static const unsigned num_prach_fd_occasions    = 1;

/// Amplitude control configuration parameters.
static float baseband_gain_dB       = -2.5F;
static bool  enable_clipping        = false;
static float full_scale_amplitude   = 1.0F;
static float amplitude_ceiling_dBFS = -0.1F;

static std::atomic<bool> is_running = {true};

/// Defines a set of configuration profiles.
static const std::vector<configuration_profile> profiles = {
    {"zmq_20MHz_n7",
     "Single 20MHz FDD in band n7 using ZMQ.",
     []() {
       driver_name                = "zmq";
       device_arguments           = "";
       srate                      = sampling_rate::from_MHz(61.44);
       time_alignmemt_calibration = -16;
       dl_arfcn                   = 536020;
       channel_bw_mhz             = bs_channel_bandwidth_fr1::MHz20;
       K_ssb                      = 6;
       offset_to_pointA           = 40;
       band                       = nr_band::n7;
       otw_format                 = radio_configuration::over_the_wire_format::DEFAULT;
       clock_src.clock            = radio_configuration::clock_sources::source::DEFAULT;
       clock_src.sync             = radio_configuration::clock_sources::source::DEFAULT;
       tx_channel_args.emplace_back(tx_address);
       rx_channel_args.emplace_back(rx_address);
     }},
    {"b200_20MHz_n7",
     "Single 20MHz FDD in band n7 using UHD and B200.",
     []() {
       driver_name      = "uhd";
       device_arguments = "type=b200";
       srate            = sampling_rate::from_MHz(23.04);
       dl_arfcn         = 536020;
       channel_bw_mhz   = bs_channel_bandwidth_fr1::MHz20;
       K_ssb            = 6;
       offset_to_pointA = 40;
       band             = nr_band::n7;
       otw_format       = radio_configuration::over_the_wire_format::SC12;
       clock_src.clock  = radio_configuration::clock_sources::source::INTERNAL;
       clock_src.sync   = radio_configuration::clock_sources::source::INTERNAL;
     }},
    {"x300_20MHz_n7",
     "Single 20MHz FDD in band n7 using UHD and X300.",
     []() {
       driver_name      = "uhd";
       device_arguments = "type=x300,send_frame_size=8000,recv_frame_size=8000";
       srate            = sampling_rate::from_MHz(184.32 / 8);
       dl_arfcn         = 536020;
       channel_bw_mhz   = bs_channel_bandwidth_fr1::MHz20;
       K_ssb            = 6;
       offset_to_pointA = 40;
       band             = nr_band::n7;
       otw_format       = radio_configuration::over_the_wire_format::SC16;
       clock_src.clock  = radio_configuration::clock_sources::source::EXTERNAL;
       clock_src.sync   = radio_configuration::clock_sources::source::EXTERNAL;
       tx_gain          = 5.0;
       rx_gain          = 5.0;
     }},
    {"n300_100MHz_n7",
     "Single 100MHz FDD in band n7 using UHD and N3x0.",
     []() {
       driver_name      = "uhd";
       device_arguments = "type=n3xx";
       srate            = sampling_rate::from_MHz(122.88);
       dl_arfcn         = 536020;
       channel_bw_mhz   = bs_channel_bandwidth_fr1::MHz100;
       K_ssb            = 6;
       offset_to_pointA = 40;
       band             = nr_band::n7;
       otw_format       = radio_configuration::over_the_wire_format::SC16;
       clock_src.clock  = radio_configuration::clock_sources::source::EXTERNAL;
       clock_src.sync   = radio_configuration::clock_sources::source::EXTERNAL;
     }},
    {"zmq_20MHz_n41", "Single 20MHz TDD in band n41 using ZMQ.", []() {
       driver_name      = "zmq";
       device_arguments = "";
       srate            = sampling_rate::from_MHz(61.44);
       channel_bw_mhz   = bs_channel_bandwidth_fr1::MHz20;
       dl_arfcn         = 520000;
       K_ssb            = 7;
       offset_to_pointA = 69;
       band             = nr_band::n41;
       otw_format       = radio_configuration::over_the_wire_format::DEFAULT;
       tx_channel_args.emplace_back(tx_address);
       rx_channel_args.emplace_back(rx_address);
     }}};

namespace {

/// This implementation returns back to the F1 interface a dummy F1 Setup Response message upon the receival of the F1
/// Setup Request message.
class dummy_cu_cp_handler : public f1c_connection_client
{
public:
  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier_) override
  {
    class dummy_du_tx_pdu_notifier : public f1ap_message_notifier
    {
    public:
      dummy_du_tx_pdu_notifier(dummy_cu_cp_handler& parent_) : parent(parent_) {}

      void on_new_message(const f1ap_message& msg) override
      {
        if (msg.pdu.type() != asn1::f1ap::f1ap_pdu_c::types::init_msg) {
          return;
        }

        f1ap_message response;
        if (msg.pdu.init_msg().value.type().value ==
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::init_ul_rrc_msg_transfer) {
          // Generate a dummy DL RRC Message transfer message and pass it back to the DU.
          response.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_DL_RRC_MSG_TRANSFER);

          auto& resp                      = response.pdu.init_msg().value.dl_rrc_msg_transfer();
          resp->gnb_du_ue_f1ap_id         = msg.pdu.init_msg().value.init_ul_rrc_msg_transfer()->gnb_du_ue_f1ap_id;
          resp->gnb_cu_ue_f1ap_id         = 0;
          resp->srb_id                    = srb_id_to_uint(srb_id_t::srb0);
          static constexpr uint8_t msg4[] = {
              0x20, 0x40, 0x03, 0x82, 0xe0, 0x05, 0x80, 0x08, 0x8b, 0xd7, 0x63, 0x80, 0x83, 0x0f, 0x00, 0x03, 0xe1,
              0x02, 0x04, 0x68, 0x3c, 0x08, 0x01, 0x05, 0x10, 0x48, 0x24, 0x06, 0x54, 0x00, 0x07, 0xc0, 0x00, 0x00,
              0x00, 0x00, 0x04, 0x1b, 0x84, 0x21, 0x00, 0x00, 0x44, 0x0b, 0x28, 0x00, 0x02, 0x41, 0x00, 0x00, 0x10,
              0x34, 0xd0, 0x35, 0x52, 0x4c, 0x40, 0x00, 0x10, 0x01, 0x02, 0x00, 0x02, 0x00, 0x68, 0x04, 0x00, 0x9d,
              0xb2, 0x58, 0xc0, 0xa2, 0x00, 0x72, 0x34, 0x56, 0x78, 0x90, 0x00, 0x00, 0x4b, 0x03, 0x84, 0x10, 0x78,
              0xbb, 0xf0, 0x30, 0x43, 0x80, 0x00, 0x00, 0x07, 0x12, 0x81, 0xc0, 0x00, 0x02, 0x05, 0xef, 0x40, 0x10,
              0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10, 0x0c, 0xa8, 0x18, 0x06, 0x20, 0x00};

          // Unpack the pre-canned Msg4, that contains the DL-CCCH RRC setup message.
          byte_buffer                 msg4_pdu = byte_buffer::create(span<const uint8_t>{msg4, sizeof(msg4)}).value();
          asn1::cbit_ref              r_bref{msg4_pdu};
          asn1::rrc_nr::dl_ccch_msg_s msg4_rrc;
          msg4_rrc.unpack(r_bref);

          // Copy DU-to-CU RRC container stored in the F1AP "INITIAL UL RRC MESSAGE TRANSFER" to masterCellGroup field
          // of the unpacked RRC Setup message.
          const auto&          src  = msg.pdu.init_msg().value.init_ul_rrc_msg_transfer()->du_to_cu_rrc_container;
          asn1::dyn_octstring& dest = msg4_rrc.msg.c1().rrc_setup().crit_exts.rrc_setup().master_cell_group;
          dest                      = src.copy();

          // Pack the updated RRC setup message.
          msg4_pdu.clear();
          asn1::bit_ref w_bref{msg4_pdu};
          msg4_rrc.pack(w_bref);

          // Store the packed RRC setup message in the RRC container field of the F1 DL RRC Message that is sent to the
          // DU.
          if (!resp->rrc_container.resize(msg4_pdu.length())) {
            du_logger.warning("Unable to resize RRC PDU to {} bytes", msg4_pdu.length());
            return;
          }
          std::copy(msg4_pdu.begin(), msg4_pdu.end(), resp->rrc_container.begin());
        } else if (msg.pdu.init_msg().value.type().value ==
                   asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request) {
          // Generate a dummy F1 Setup response message and pass it back to the DU.
          response.pdu.set_successful_outcome();
          response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

          auto& setup_res = response.pdu.successful_outcome().value.f1_setup_resp();
          // Use the same transaction ID as in the request message.
          setup_res->transaction_id      = msg.pdu.init_msg().value.f1_setup_request()->transaction_id;
          setup_res->gnb_cu_name_present = true;
          setup_res->gnb_cu_name.from_string("srsCU");
          setup_res->gnb_cu_rrc_version.latest_rrc_version.from_number(2);
        } else {
          srsran::byte_buffer buffer;
          asn1::bit_ref       bref(buffer);
          if (msg.pdu.pack(bref) != asn1::SRSASN_SUCCESS) {
            du_logger.info("The contents of the received Msg5 are invalid");
          } else {
            std::vector<uint8_t> v(buffer.begin(), buffer.end());
            du_logger.info(v.data(), v.size(), "Msg5 successfully received");
          }

          // Terminate the application once we reach this point.
          is_running = false;
          return;
        }

        // Send response to DU.
        parent.du_rx_pdu_notifier->on_new_message(response);
      }

    private:
      dummy_cu_cp_handler& parent;
    };

    du_rx_pdu_notifier = std::move(du_rx_pdu_notifier_);

    return std::make_unique<dummy_du_tx_pdu_notifier>(*this);
  }

private:
  std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier;
};

/// Dummy implementation of the mac_result_notifier.
class phy_dummy : public mac_result_notifier
{
  mac_cell_result_notifier& cell;

public:
  explicit phy_dummy(mac_cell_result_notifier& cell_) : cell(cell_) {}

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
};

class fapi_slot_last_message_dummy : public fapi::slot_last_message_notifier
{
public:
  void on_last_message(slot_point slot) override {}
};

/// Manages the workers of the app.
struct worker_manager {
  static const uint32_t task_worker_queue_size = 128;

  void stop()
  {
    cell_workers.stop();
    ue_workers.stop();
    ctrl_worker.stop();
    lower_dl_task_worker.stop();
    lower_ul_task_worker.stop();
    upper_dl_worker.stop();
    upper_ul_worker.stop();
    lower_prach_worker.stop();
    radio_worker.stop();
  }

  task_worker                  ctrl_worker{"Ctrl-GNB", task_worker_queue_size};
  task_worker                  cell_workers{"DU-CELL#0", task_worker_queue_size};
  task_worker                  ue_workers{"UE#0", task_worker_queue_size};
  task_worker_executor         ctrl_exec{ctrl_worker};
  task_worker_executor         cell_execs{cell_workers};
  task_worker_executor         ue_execs{ue_workers};
  du_high_executor_mapper_impl du_high_exec_mapper{
      std::make_unique<cell_executor_mapper>(std::initializer_list<task_executor*>{&cell_execs}),
      std::make_unique<pcell_ue_executor_mapper>(std::initializer_list<task_executor*>{&ue_execs}),
      ctrl_exec,
      ctrl_exec,
      ctrl_exec};
  // Downlink Lower PHY task executors.
  task_worker          lower_dl_task_worker{"low_dl", 2048, os_thread_realtime_priority::max()};
  task_worker_executor lower_tx_task_executor{{lower_dl_task_worker}};
  task_worker_executor lower_dl_task_executor{{lower_dl_task_worker}};
  // Uplink Lower PHY task executors.
  task_worker          lower_ul_task_worker{"low_ul", 1, os_thread_realtime_priority::max()};
  task_worker_executor lower_rx_task_executor{{lower_ul_task_worker}};
  task_worker_executor lower_ul_task_executor{{lower_ul_task_worker}};
  // PRACH lower PHY executor.
  task_worker          lower_prach_worker{"low_prach", task_worker_queue_size};
  task_worker_executor lower_prach_executor{lower_prach_worker};
  // Upper phy task executor.
  task_worker          upper_dl_worker{"PHY_DL", task_worker_queue_size};
  task_worker_executor upper_dl_executor{upper_dl_worker};
  task_worker          upper_ul_worker{"PHY_UL", task_worker_queue_size};
  task_worker_executor upper_ul_executor{upper_ul_worker};
  // Radio task executor.
  task_worker          radio_worker{"radio_thread", task_worker_queue_size};
  task_worker_executor radio_executor{radio_worker};
};

} // namespace

static lower_phy_configuration create_lower_phy_configuration()
{
  lower_phy_configuration phy_config;

  phy_config.logger                     = &du_logger;
  phy_config.scs                        = scs;
  phy_config.cp                         = cp;
  phy_config.dft_window_offset          = 0.5F;
  phy_config.max_processing_delay_slots = 2;

  phy_config.srate = srate;

  phy_config.ta_offset                  = band_helper::get_ta_offset(band);
  phy_config.time_alignment_calibration = time_alignmemt_calibration;

  // Select buffer size policy.
  if (driver_name == "zmq") {
    phy_config.baseband_tx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::half_slot;
    phy_config.baseband_rx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::half_slot;
  } else {
    phy_config.baseband_tx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::slot;
    phy_config.baseband_rx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::single_packet;
  }

  // Amplitude controller configuration.
  phy_config.amplitude_config.full_scale_lin  = full_scale_amplitude;
  phy_config.amplitude_config.ceiling_dBFS    = amplitude_ceiling_dBFS;
  phy_config.amplitude_config.enable_clipping = enable_clipping;
  phy_config.amplitude_config.input_gain_dB   = baseband_gain_dB;

  // Sector configuration.
  lower_phy_sector_description sector_config;
  sector_config.bandwidth_rb = band_helper::get_n_rbs_from_bw(channel_bw_mhz, scs, frequency_range::FR1);
  sector_config.dl_freq_hz   = band_helper::nr_arfcn_to_freq(dl_arfcn);
  sector_config.ul_freq_hz   = band_helper::nr_arfcn_to_freq(band_helper::get_ul_arfcn_from_dl_arfcn(dl_arfcn, {}));
  sector_config.nof_rx_ports = 1;
  sector_config.nof_tx_ports = 1;
  phy_config.sectors.push_back(sector_config);

  return phy_config;
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
  fmt::print("\t-c Enable amplitude clipping. [Default {}]\n", enable_clipping);
  fmt::print("\t-b Baseband gain prior to clipping (in dB). [Default {}]\n", baseband_gain_dB);
  fmt::print("\t-z Enable FAPI logs. [Default {}]\n", enable_fapi_logs);
  fmt::print("\t-r ZMQ rx address. [Default {}]\n", rx_address);
  fmt::print("\t-t ZMQ tx address. [Default {}]\n", tx_address);
  fmt::print("\t-h print this message.\n");
}

static int parse_args(int argc, char** argv)
{
  std::string profile_name;

  int opt = 0;
  while ((opt = ::getopt(argc, argv, "P:v:b:r:t:czh")) != -1) {
    switch (opt) {
      case 'P':
        if (optarg != nullptr) {
          profile_name = std::string(optarg);
        }
        break;
      case 'r':
        if (optarg != nullptr) {
          rx_address = std::string(optarg);
        }
        break;
      case 't':
        if (optarg != nullptr) {
          tx_address = std::string(optarg);
        }
        break;
      case 'v':
        log_level = std::string(optarg);
        break;
      case 'c':
        enable_clipping = true;
        break;
      case 'z':
        enable_fapi_logs = true;
        break;
      case 'b':
        if (optarg != nullptr) {
          baseband_gain_dB = std::strtof(optarg, nullptr);
        }
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
        du_logger.info("Loading profile: '{}'", profile.name);
        found = true;
        break;
      }
    }
    if (!found) {
      usage(argv[0]);
      du_logger.error("Invalid profile: '{}'", profile_name);
      return -1;
    }
  } else {
    const configuration_profile& default_profile = profiles.front();
    du_logger.info("Loading '{}' as the default profile", default_profile.name);
    default_profile.function();
  }

  return 0;
}

static fapi::prach_config generate_prach_config_tlv()
{
  fapi::prach_config config = {};

  config.prach_res_config_index = 0;
  config.prach_sequence_length  = fapi::prach_sequence_length_type::long_sequence;
  config.prach_scs              = prach_subcarrier_spacing::kHz1_25;
  config.prach_ul_bwp_pusch_scs = scs;
  config.restricted_set         = restricted_set_config::UNRESTRICTED;
  config.num_prach_fd_occasions = num_prach_fd_occasions;
  config.prach_config_index     = prach_config_index;
  config.prach_format           = prach_format_type::zero;
  config.num_prach_td_occasions = 1;
  config.num_preambles          = 1;
  config.start_preamble_index   = 0;

  // Add FD occasion info.
  fapi::prach_fd_occasion_config& fd_occasion = config.fd_occasions.emplace_back();
  fd_occasion.prach_root_sequence_index       = prach_root_sequence_index;
  fd_occasion.prach_freq_offset               = prach_msg1_freq_offset;
  fd_occasion.prach_zero_corr_conf            = zero_correlation_zone;

  return config;
}

static fapi::carrier_config generate_carrier_config_tlv()
{
  // Deduce common numerology and grid size for DL and UL.
  unsigned numerology       = to_numerology_value(scs);
  unsigned grid_size_bw_prb = band_helper::get_n_rbs_from_bw(
      channel_bw_mhz, scs, band_helper::get_freq_range(band_helper::get_band_from_dl_arfcn(dl_arfcn)));

  fapi::carrier_config fapi_config = {};

  // NOTE; for now we only need to fill the nof_prb_ul_grid and nof_prb_dl_grid for the common SCS.
  fapi_config.dl_grid_size             = {};
  fapi_config.dl_grid_size[numerology] = grid_size_bw_prb;
  fapi_config.ul_grid_size             = {};
  fapi_config.ul_grid_size[numerology] = grid_size_bw_prb;

  // Number of transmit and receive antenna ports.
  fapi_config.num_tx_ant = num_tx_ant;
  fapi_config.num_rx_ant = num_rx_ant;

  return fapi_config;
}

static radio_configuration::radio generate_radio_config()
{
  radio_configuration::radio out_cfg;

  out_cfg.args             = device_arguments;
  out_cfg.args             = "tx_port=" + tx_address + ",rx_port=" + rx_address;
  out_cfg.log_level        = log_level;
  out_cfg.sampling_rate_hz = srate.to_Hz();
  out_cfg.discontinuous_tx = false;
  out_cfg.power_ramping_us = 0.0F;
  out_cfg.otw_format       = otw_format;
  out_cfg.clock            = clock_src;

  const unsigned nof_ports = 1;
  // For each sector...
  for (unsigned sector_id = 0; sector_id != 1; ++sector_id) {
    // Each cell is mapped to a different stream.
    radio_configuration::stream tx_stream_config;
    radio_configuration::stream rx_stream_config;

    // Deduce center frequencies.
    double center_tx_freq_cal_Hz = band_helper::nr_arfcn_to_freq(dl_arfcn);
    double center_rx_freq_cal_Hz =
        band_helper::nr_arfcn_to_freq(band_helper::get_ul_arfcn_from_dl_arfcn(dl_arfcn, band));

    // For each port in the cell...
    for (unsigned port_id = 0; port_id != nof_ports; ++port_id) {
      // Create channel configuration and append it to the previous ones.
      radio_configuration::channel tx_ch_config = {};
      tx_ch_config.freq.center_frequency_hz     = center_tx_freq_cal_Hz;
      tx_ch_config.freq.lo_frequency_hz         = 0.0;
      tx_ch_config.gain_dB                      = tx_gain;

      // Add the tx ports.
      if (driver_name == "zmq") {
        tx_ch_config.args = tx_channel_args[sector_id * nof_ports + port_id];
      }
      tx_stream_config.channels.emplace_back(tx_ch_config);

      radio_configuration::channel rx_ch_config = {};
      rx_ch_config.freq.center_frequency_hz     = center_rx_freq_cal_Hz;
      rx_ch_config.freq.lo_frequency_hz         = 0.0;
      rx_ch_config.gain_dB                      = rx_gain;

      if (driver_name == "zmq") {
        rx_ch_config.args = rx_channel_args[sector_id * nof_ports + port_id];
      }
      rx_stream_config.channels.emplace_back(rx_ch_config);
    }
    out_cfg.tx_streams.emplace_back(tx_stream_config);
    out_cfg.rx_streams.emplace_back(rx_stream_config);
  }

  return out_cfg;
}

static void fill_cell_prach_cfg(du_cell_config& cell_cfg)
{
  cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.prach_config_index = prach_config_index;
  cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_fdm = num_prach_fd_occasions;
  cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.msg1_frequency_start =
      prach_msg1_freq_offset;
  cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().rach_cfg_generic.zero_correlation_zone_config =
      zero_correlation_zone;
  cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.value().prach_root_seq_index = prach_root_sequence_index;
}

static ru_generic_configuration build_ru_config(srslog::basic_logger&               rf_logger,
                                                ru_uplink_plane_rx_symbol_notifier& symbol_notifier,
                                                ru_timing_notifier&                 timing_notifier,
                                                worker_manager&                     workers,
                                                bool                                is_zmq_used)
{
  ru_generic_configuration config;

  config.radio_cfg     = generate_radio_config();
  config.device_driver = driver_name;
  config.rf_logger     = &rf_logger;
  config.lower_phy_config.push_back(create_lower_phy_configuration());
  config.timing_notifier = &timing_notifier;
  config.symbol_notifier = &symbol_notifier;
  config.radio_exec      = &workers.radio_executor;

  auto& low_cfg                = config.lower_phy_config.back();
  low_cfg.tx_task_executor     = (is_zmq_used) ? &workers.lower_tx_task_executor : &workers.lower_tx_task_executor;
  low_cfg.rx_task_executor     = (is_zmq_used) ? &workers.lower_tx_task_executor : &workers.lower_rx_task_executor;
  low_cfg.dl_task_executor     = (is_zmq_used) ? &workers.lower_tx_task_executor : &workers.lower_dl_task_executor;
  low_cfg.ul_task_executor     = (is_zmq_used) ? &workers.lower_tx_task_executor : &workers.lower_ul_task_executor;
  low_cfg.prach_async_executor = (is_zmq_used) ? &workers.lower_tx_task_executor : &workers.lower_prach_executor;

  config.statistics_printer_executor = &workers.lower_dl_task_executor;

  return config;
}

int main(int argc, char** argv)
{
  srslog::init();
  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::info);
  du_logger.set_level(srslog::basic_levels::info);

  // Parse arguments.
  int ret = parse_args(argc, argv);
  if (ret < 0) {
    return ret;
  }

  // Calculate derived frequency parameters.
  double dl_center_freq = band_helper::nr_arfcn_to_freq(dl_arfcn);
  double ul_arfcn       = band_helper::get_ul_arfcn_from_dl_arfcn(dl_arfcn, {});
  double ul_center_freq = band_helper::nr_arfcn_to_freq(ul_arfcn);
  du_logger.info("Starting du_example with DL_ARFCN={}, UL_ARFCN={}, DL center frequency {} Hz, UL center frequency {} "
                 "Hz, tx_gain={} dB, rx_gain={} dB",
                 dl_arfcn,
                 ul_arfcn,
                 dl_center_freq,
                 ul_center_freq,
                 tx_gain,
                 rx_gain);

  worker_manager workers;

  upper_ru_ul_adapter     ru_ul_adapt(1);
  upper_ru_timing_adapter ru_timing_adapt(1);

  bool is_zmq_used = driver_name == "zmq";

  ru_generic_configuration ru_cfg =
      build_ru_config(srslog::fetch_basic_logger("Radio", true), ru_ul_adapt, ru_timing_adapt, workers, is_zmq_used);

  auto ru_object = create_generic_ru(ru_cfg);
  report_error_if_not(ru_object, "Unable to create Radio Unit.");
  du_logger.info("Radio Unit created successfully");

  // Create upper PHY.
  upper_phy_params upper_params;
  upper_params.log_level      = srslog::str_to_basic_level(log_level);
  upper_params.channel_bw_mhz = channel_bw_mhz;
  upper_params.scs            = scs;

  upper_ru_dl_rg_adapter      ru_dl_rg_adapt;
  upper_ru_ul_request_adapter ru_ul_request_adapt;
  ru_dl_rg_adapt.connect(ru_object->get_downlink_plane_handler());
  ru_ul_request_adapt.connect(ru_object->get_uplink_plane_handler());

  std::vector<task_executor*> dl_executors;
  dl_executors.emplace_back((is_zmq_used) ? &workers.lower_tx_task_executor : &workers.upper_ul_executor);
  auto upper = create_upper_phy(upper_params,
                                &ru_dl_rg_adapt,
                                dl_executors,
                                (is_zmq_used) ? &workers.lower_tx_task_executor : &workers.upper_dl_executor,
                                &ru_ul_request_adapt);
  report_fatal_error_if_not(upper, "Unable to create upper PHY.");
  du_logger.info("Upper PHY created successfully");

  // Make connections between upper and RU.
  ru_ul_adapt.map_handler(0, upper->get_rx_symbol_handler());
  ru_timing_adapt.map_handler(0, upper->get_timing_handler());

  // Create FAPI adaptors.
  const unsigned sector_id       = 0;
  auto           pm_tools        = fapi_adaptor::generate_precoding_matrix_tables(num_tx_ant);
  auto           uci_part2_tools = fapi_adaptor::generate_uci_part2_correspondence(1);
  auto           phy_adaptor     = build_phy_fapi_adaptor(
      sector_id,
      scs,
      scs,
      upper->get_downlink_processor_pool(),
      upper->get_downlink_resource_grid_pool(),
      upper->get_uplink_request_processor(),
      upper->get_uplink_resource_grid_pool(),
      upper->get_uplink_slot_pdu_repository(),
      upper->get_downlink_pdu_validator(),
      upper->get_uplink_pdu_validator(),
      generate_prach_config_tlv(),
      generate_carrier_config_tlv(),
      std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_repository>>(pm_tools)),
      std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_repository>>(uci_part2_tools)),
      {0});
  report_error_if_not(phy_adaptor, "Unable to create PHY adaptor.");
  upper->set_rx_results_notifier(phy_adaptor->get_rx_results_notifier());
  upper->set_timing_notifier(phy_adaptor->get_timing_notifier());

  fapi_slot_last_message_dummy                       last_msg_notifier;
  std::unique_ptr<fapi::slot_message_gateway>        logging_slot_gateway;
  std::unique_ptr<fapi::slot_time_message_notifier>  logging_slot_time_notifier;
  std::unique_ptr<fapi::slot_error_message_notifier> logging_slot_error_notifier;
  std::unique_ptr<fapi::slot_data_message_notifier>  logging_slot_data_notifier;
  std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>    mac_adaptor;
  if (enable_fapi_logs) {
    // Create gateway loggers and intercept MAC adaptor calls.
    logging_slot_gateway = fapi::create_logging_slot_gateway(phy_adaptor->get_slot_message_gateway());
    report_error_if_not(logging_slot_gateway, "Unable to create logger for slot data notifications.");
    mac_adaptor = build_mac_fapi_adaptor(
        0,
        scs,
        *logging_slot_gateway,
        last_msg_notifier,
        std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>>(pm_tools)),
        std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper>>(uci_part2_tools)),
        get_max_Nprb(bs_channel_bandwidth_to_MHz(channel_bw_mhz), scs, srsran::frequency_range::FR1));

    // Create notification loggers.
    logging_slot_time_notifier = fapi::create_logging_slot_time_notifier(mac_adaptor->get_slot_time_notifier());
    report_error_if_not(logging_slot_time_notifier, "Unable to create logger for slot time notifications.");
    logging_slot_error_notifier = fapi::create_logging_slot_error_notifier(mac_adaptor->get_slot_error_notifier());
    report_error_if_not(logging_slot_error_notifier, "Unable to create logger for slot error notifications.");
    logging_slot_data_notifier = fapi::create_logging_slot_data_notifier(mac_adaptor->get_slot_data_notifier());
    report_error_if_not(logging_slot_data_notifier, "Unable to create logger for slot data notifications.");

    // Connect the PHY adaptor with the loggers to intercept PHY notifications.
    phy_adaptor->set_slot_time_message_notifier(*logging_slot_time_notifier);
    phy_adaptor->set_slot_error_message_notifier(*logging_slot_error_notifier);
    phy_adaptor->set_slot_data_message_notifier(*logging_slot_data_notifier);
  } else {
    mac_adaptor = build_mac_fapi_adaptor(
        0,
        scs,
        phy_adaptor->get_slot_message_gateway(),
        last_msg_notifier,
        std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>>(pm_tools)),
        std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper>>(uci_part2_tools)),
        get_max_Nprb(bs_channel_bandwidth_to_MHz(channel_bw_mhz), scs, srsran::frequency_range::FR1));
    report_error_if_not(mac_adaptor, "Unable to create MAC adaptor.");
    phy_adaptor->set_slot_time_message_notifier(mac_adaptor->get_slot_time_notifier());
    phy_adaptor->set_slot_error_message_notifier(mac_adaptor->get_slot_error_notifier());
    phy_adaptor->set_slot_data_message_notifier(mac_adaptor->get_slot_data_notifier());
  }

  du_logger.info("FAPI adaptors created successfully");

  // Cell configuration.
  struct cell_config_builder_params cell_config;
  cell_config.pci               = pci;
  cell_config.scs_common        = scs;
  cell_config.channel_bw_mhz    = channel_bw_mhz;
  cell_config.dl_arfcn          = dl_arfcn;
  cell_config.band              = band;
  cell_config.offset_to_point_a = offset_to_pointA;
  cell_config.coreset0_index    = coreset0_index;
  cell_config.k_ssb             = K_ssb;

  dummy_cu_cp_handler f1c_client;
  phy_dummy           phy(mac_adaptor->get_cell_result_notifier());

  timer_manager         app_timers{256};
  null_mac_pcap         mac_p;
  null_rlc_pcap         rlc_p;
  du_high_configuration du_hi_cfg = {};
  du_hi_cfg.exec_mapper           = &workers.du_high_exec_mapper;
  du_hi_cfg.f1c_client            = &f1c_client;
  du_hi_cfg.phy_adapter           = &phy;
  du_hi_cfg.timers                = &app_timers;
  du_hi_cfg.cells                 = {config_helpers::make_default_du_cell_config(cell_config)};
  du_hi_cfg.sched_cfg             = config_helpers::make_default_scheduler_expert_config();
  du_hi_cfg.mac_p                 = &mac_p;
  du_hi_cfg.rlc_p                 = &rlc_p;

  du_cell_config& cell_cfg = du_hi_cfg.cells.front();
  cell_cfg.ssb_cfg.k_ssb   = K_ssb;

  // Fill cell specific PRACH configuration.
  fill_cell_prach_cfg(cell_cfg);

  std::unique_ptr<du_high> du_obj = make_du_high(du_hi_cfg);
  du_logger.info("DU-High created successfully");

  // Set signal handler.
  ::signal(SIGINT, signal_handler);
  ::signal(SIGTERM, signal_handler);
  ::signal(SIGHUP, signal_handler);
  ::signal(SIGQUIT, signal_handler);
  ::signal(SIGKILL, signal_handler);

  // Start execution.
  du_obj->start();

  // Give some time to the MAC to start.
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // Configure the DU slot handler.
  du_cell_index_t cell_id = to_du_cell_index(0);
  mac_adaptor->set_cell_slot_handler(du_obj->get_slot_handler(cell_id));
  mac_adaptor->set_cell_rach_handler(du_obj->get_rach_handler(cell_id));
  mac_adaptor->set_cell_pdu_handler(du_obj->get_pdu_handler());
  mac_adaptor->set_cell_crc_handler(du_obj->get_control_info_handler(cell_id));

  // Start processing.
  du_logger.info("Starting Radio Unit...");
  ru_object->get_controller().start();
  du_logger.info("Radio Unit started successfully");

  while (is_running) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  du_logger.info("Stopping Radio Unit...");
  ru_object->get_controller().stop();
  du_logger.info("Radio Unit notify_stop successfully");

  du_logger.info("Stopping executors...");
  workers.stop();
  du_logger.info("Executors notify_stop successfully");

  srslog::flush();

  return 0;
}

/// \endcond
