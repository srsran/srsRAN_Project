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

#include "../../../lib/phy/support/resource_grid_impl.h"
#include "helpers.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/circular_map.h"
#include "srsran/ofh/ecpri/ecpri_constants.h"
#include "srsran/ofh/ethernet/ethernet_frame_notifier.h"
#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/ru/ru_controller.h"
#include "srsran/ru/ru_downlink_plane.h"
#include "srsran/ru/ru_error_notifier.h"
#include "srsran/ru/ru_ofh_factory.h"
#include "srsran/ru/ru_timing_notifier.h"
#include "srsran/ru/ru_uplink_plane.h"
#include "srsran/support/executors/task_execution_manager.h"
#include "srsran/support/executors/task_executor.h"
#include <arpa/inet.h>
#include <getopt.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <random>
#include <sys/ioctl.h>

using namespace srsran;
using namespace ofh;
using namespace std::chrono_literals;

/// Random generator.
static std::mt19937 rgen(0);

/// Transmission window parameters expressed in symbols, given the 30kHz scs.
unsigned T1a_max_cp_dl = 13; // 470us.
unsigned T1a_min_cp_dl = 8;  // 258us.
unsigned T1a_max_cp_ul = 8;  // 300us.
unsigned T1a_min_cp_ul = 8;  // 285us.
unsigned T1a_max_up    = 9;  // 350us.
unsigned T1a_min_up    = 2;  // 50us.
/// Reception window parameters expressed in symbols, given the 30kHz scs.
unsigned Ta4_min = 1; // 150us.
unsigned Ta4_max = 5; // 25us.

static const tdd_ul_dl_pattern tdd_pattern_7d2u{10, 7, 0, 2, 0};
static const tdd_ul_dl_pattern tdd_pattern_6d3u{10, 6, 0, 3, 0};
static tdd_ul_dl_pattern       tdd_pattern = tdd_pattern_7d2u;

static const unsigned vlan_tag               = 9;
static const unsigned processing_delay_slots = 6;
static unsigned       nof_antennas_dl        = 4;
static unsigned       nof_antennas_ul        = 2;

static std::atomic<bool>     slot_synchronized{false};
static std::atomic<unsigned> slot_val{0};
static std::atomic<unsigned> nof_malformed_packets{0};
static std::atomic<unsigned> nof_missing_dl_packets{0};
static unsigned              nof_test_slots{1000};

namespace {

/// User-defined test parameters.
struct test_parameters {
  bool                     silent                              = false;
  std::string              log_level                           = "info";
  std::string              log_filename                        = "stdout";
  bool                     is_prach_control_plane_enabled      = true;
  bool                     is_downlink_broadcast_enabled       = false;
  bool                     ignore_ecpri_payload_size_field     = false;
  std::string              data_compr_method                   = "bfp";
  unsigned                 data_bitwidth                       = 9;
  std::string              prach_compr_method                  = "bfp";
  unsigned                 prach_bitwidth                      = 9;
  bool                     is_downlink_static_comp_hdr_enabled = false;
  bool                     is_uplink_static_comp_hdr_enabled   = false;
  bool                     is_downlink_parallelized            = true;
  units::bytes             mtu                                 = units::bytes(9000);
  std::vector<unsigned>    prach_port_id                       = {4, 5};
  std::vector<unsigned>    dl_port_id                          = {0, 1, 2, 3};
  std::vector<unsigned>    ul_port_id                          = {0, 1};
  bs_channel_bandwidth_fr1 bw                                  = srsran::bs_channel_bandwidth_fr1::MHz20;
  subcarrier_spacing       scs                                 = subcarrier_spacing::kHz30;
  std::string              tdd_pattern_str                     = "7d2u";
  bool                     use_loopback_receiver               = false;
};

/// Dummy Radio Unit error notifier.
class dummy_ru_error_notifier : public ru_error_notifier
{
public:
  void on_late_downlink_message(const ru_error_context& context) override {}
};
} // namespace

static test_parameters test_params;

/// Prints usage information of the app.
static void usage(const char* prog)
{
  fmt::print("Usage: {} [-s silent]\n", prog);
  fmt::print("\t-w Channel bandwidth [Default {}]\n", test_params.bw);
  fmt::print("\t-c Subcarrier spacing. [Default {}]\n", to_string(test_params.scs));
  fmt::print("\t-d Array of downlink eAxCs [default is {}]\n", port_ids_to_str(test_params.dl_port_id));
  fmt::print("\t-u Array of uplink eAxCs [default is {}]\n", port_ids_to_str(test_params.ul_port_id));
  fmt::print("\t-p Array of PRACH eAxCs [default is {}]\n", port_ids_to_str(test_params.prach_port_id));
  fmt::print("\t-T Type of compression for DL/UL data ['none', 'bfp', default is {}]\n", test_params.data_compr_method);
  fmt::print("\t-t Type of compression for PRACH ['none', 'bfp', default is {}]\n", test_params.prach_compr_method);
  fmt::print("\t-B Bitwidth of compressed DL/UL data [9, 16, default is {}]\n", test_params.data_bitwidth);
  fmt::print("\t-b Bitwidth of compressed PRACH data [9, 16, default is {}]\n", test_params.prach_bitwidth);
  fmt::print("\t-A Use static compression header for DL data [Default {}]\n",
             test_params.is_downlink_static_comp_hdr_enabled);
  fmt::print("\t-a Use static compression header for UL data [Default {}]\n",
             test_params.is_uplink_static_comp_hdr_enabled);
  fmt::print("\t-e Broadcasts the contents of a single antenna port to all downlink eAxCs [Default {}]\n",
             test_params.is_downlink_broadcast_enabled);
  fmt::print("\t-r Enable the Control-Plane PRACH message signalling [Default {}]\n",
             test_params.is_prach_control_plane_enabled);
  fmt::print("\t-i If set to true, the payload size encoded in a eCPRI header is ignored [Default {}]\n",
             test_params.ignore_ecpri_payload_size_field);
  fmt::print("\t-P TDD pattern ['7d2u', '6d3u', default is {}]\n", test_params.tdd_pattern_str);
  fmt::print("\t-m Ethernet frame size [1500-9600, default is {}]\n", test_params.mtu.value());
  fmt::print("\t-l Use loopback Ethernet interface (requires root permissions) [default is {}]\n",
             test_params.use_loopback_receiver);
  fmt::print("\t-N Number of slots processed in the test [Default {}]]\n", nof_test_slots);
  fmt::print("\t-s Toggle silent operation [Default {}]\n", test_params.silent);
  fmt::print("\t-v Logging level. [Default {}]\n", test_params.log_level);
  fmt::print("\t-f Log file name. [Default {}]\n", test_params.log_filename);
  fmt::print("\t-h Show this message\n");
}

/// Parses arguments of the app.
static void parse_args(int argc, char** argv)
{
  int  opt         = 0;
  bool invalid_arg = false;

  while ((opt = ::getopt(argc, argv, "f:T:t:B:b:w:c:d:u:p:P:v:m:N:lAaerish")) != -1) {
    switch (opt) {
      case 'T':
        test_params.data_compr_method = std::string(optarg);
        break;
      case 't':
        test_params.prach_compr_method = std::string(optarg);
        break;
      case 'B':
        test_params.data_bitwidth = std::strtol(optarg, nullptr, 10);
        break;
      case 'b':
        test_params.prach_bitwidth = std::strtol(optarg, nullptr, 10);
        break;
      case 'A':
        test_params.is_downlink_static_comp_hdr_enabled = true;
        break;
      case 'a':
        test_params.is_uplink_static_comp_hdr_enabled = true;
        break;
      case 'e':
        test_params.is_downlink_broadcast_enabled = true;
        break;
      case 'r':
        test_params.is_prach_control_plane_enabled = true;
        break;
      case 'i':
        test_params.ignore_ecpri_payload_size_field = true;
        break;
      case 'w':
        if (optarg != nullptr) {
          if (!is_valid_bw(std::strtol(optarg, nullptr, 10))) {
            fmt::print("Invalid bandwidth\n");
            invalid_arg = true;
          } else {
            test_params.bw = MHz_to_bs_channel_bandwidth(std::strtol(optarg, nullptr, 10));
          }
        }
        break;
      case 'c':
        if (optarg != nullptr) {
          test_params.scs = to_subcarrier_spacing(std::string(optarg));
          if (test_params.scs == subcarrier_spacing::invalid) {
            fmt::print("Invalid subcarrier spacing\n");
            invalid_arg = true;
          }
        }
        break;
      case 'P':
        if (std::string(optarg) == "7d2u") {
          tdd_pattern = tdd_pattern_7d2u;
        } else if (std::string(optarg) == "6d3u") {
          tdd_pattern = tdd_pattern_6d3u;
        } else {
          fmt::print("Invalid TDD pattern provided\n");
          invalid_arg = true;
        }
        break;
      case 'd':
        test_params.dl_port_id = parse_port_id(std::string(optarg));
        if (test_params.dl_port_id.empty()) {
          fmt::print("Invalid array of DL ports provided\n");
          invalid_arg = true;
        }
        break;
      case 'u':
        test_params.ul_port_id = parse_port_id(std::string(optarg));
        if (test_params.ul_port_id.empty()) {
          fmt::print("Invalid array of UL ports provided\n");
          invalid_arg = true;
        }
        break;
      case 'p':
        test_params.prach_port_id = parse_port_id(std::string(optarg));
        if (test_params.prach_port_id.empty()) {
          fmt::print("Invalid array of PRACH ports provided\n");
          invalid_arg = true;
        }
        break;
      case 'm':
        test_params.mtu = units::bytes(std::strtol(optarg, nullptr, 10));
        if (test_params.mtu.value() < 1500 || test_params.mtu.value() > 9600) {
          fmt::print("MTU size is out of valid range of [1500; 9600]\n");
          invalid_arg = true;
        }
        break;
      case 'N':
        nof_test_slots = std::strtol(optarg, nullptr, 10);
        break;
      case 'l':
        test_params.use_loopback_receiver = (!test_params.use_loopback_receiver);
        break;
      case 's':
        test_params.silent = (!test_params.silent);
        break;
      case 'v':
        test_params.log_level = std::string(optarg);
        break;
      case 'f':
        test_params.log_filename = std::string(optarg);
        break;
      case 'h':
      default:
        usage(argv[0]);
        ::exit(0);
    }
    if (invalid_arg) {
      usage(argv[0]);
      ::exit(0);
    }
    nof_antennas_dl = test_params.dl_port_id.size();
    nof_antennas_ul = test_params.ul_port_id.size();
  }
}

namespace {

class dummy_frame_notifier : public ether::frame_notifier
{
  // See interface for documentation.
  void on_new_frame(span<const uint8_t> payload) override{};
};
dummy_frame_notifier dummy_notifier;

/// Test Ethernet receiver interface.
class test_ether_receiver : public ether::receiver
{
public:
  test_ether_receiver(srslog::basic_logger& logger_) : logger(logger_), notifier(dummy_notifier) {}
  virtual ~test_ether_receiver() = default;

  void start(ether::frame_notifier& notifier_) override
  {
    notifier = std::ref(notifier_);
    logger.debug("Test Ethernet receiver started");
  }
  void stop() override {}

  virtual void push_new_data(span<const uint8_t> frame) = 0;

protected:
  srslog::basic_logger&                         logger;
  std::reference_wrapper<ether::frame_notifier> notifier;
};

/// Dummy Ethernet receiver that receives data from RU emulator and pushes them to the OFH receiver without using real
/// Ethernet interface.
class dummy_eth_receiver : public test_ether_receiver
{
  static constexpr unsigned BUFFER_SIZE = 9600;
  static constexpr unsigned QUEUE_SIZE  = 320;

public:
  dummy_eth_receiver(srslog::basic_logger& logger_, task_executor& executor_) :
    test_ether_receiver(logger_), executor(executor_), write_pos(-1), read_pos(-1)
  {
    for (auto& buffer : queue) {
      buffer.reserve(BUFFER_SIZE);
    }
  }

  void push_new_data(span<const uint8_t> frame) override
  {
    {
      std::lock_guard<std::mutex> lock(mutex);
      int                         pos = (write_pos + 1) % QUEUE_SIZE;
      if (pos == read_pos) {
        logger.warning("Ethernet receiver dropped data - queue is full");
        return;
      }
      queue[pos].resize(frame.size());
      std::memcpy(queue[pos].data(), frame.data(), frame.size());
      write_pos = pos;
    }
    while (!executor.defer([this]() {
      {
        std::lock_guard<std::mutex> lock(mutex);
        read_pos = (read_pos + 1) % QUEUE_SIZE;
      }
      notifier.get().on_new_frame(span<const uint8_t>(queue[read_pos].data(), queue[read_pos].size()));
    })) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

private:
  task_executor&                               executor;
  std::array<std::vector<uint8_t>, QUEUE_SIZE> queue;
  int                                          write_pos;
  int                                          read_pos;
  std::mutex                                   mutex;
};

/// Ethernet receiver using loopback ('lo') interface.
class lo_eth_receiver : public test_ether_receiver
{
public:
  lo_eth_receiver(srslog::basic_logger& logger_) : test_ether_receiver(logger_) { init_loopback_connection(); }

  // See interface for documentation.
  void push_new_data(span<const uint8_t> frame) override
  {
    if (::sendto(socket_fd,
                 frame.data(),
                 frame.size(),
                 0,
                 reinterpret_cast<::sockaddr*>(&socket_address),
                 sizeof(socket_address)) < 0) {
      fmt::print("sendto failed to transmit {} bytes", frame.size());
    }
  }

private:
  void init_loopback_connection()
  {
    socket_fd = ::socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, IPPROTO_RAW);
    if (socket_fd < 0) {
      report_error("Unable to open raw socket for Ethernet gateway: {}", strerror(errno));
    }

    // Get the index of loopback interface.
    ::ifreq if_idx = {};
    ::strncpy(if_idx.ifr_name, "lo", IFNAMSIZ - 1);
    if (::ioctl(socket_fd, SIOCGIFINDEX, &if_idx) < 0) {
      report_error("Unable to get index for loopback interface");
    }

    // Prepare the socket address used by sendto.
    socket_address             = {};
    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    socket_address.sll_halen   = ETH_ALEN;
  }

  /// Ethernet structures.
  int         socket_fd = -1;
  sockaddr_ll socket_address;
};

/// Dummy RU notifier class for symbol events.
class dummy_rx_symbol_notifier : public ru_uplink_plane_rx_symbol_notifier
{
public:
  // See interface for documentation.
  void on_new_uplink_symbol(const ru_uplink_rx_symbol_context& context, const resource_grid_reader& grid) override {}

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& buffer) override {}

  // See interface for documentation.
  void on_rx_srs_symbol(const ru_uplink_rx_symbol_context& context) override {}
};

/// Dummy RU notifier class for timing events.
/// It is used to synchronize time between OFH RU implementation and the DU emulator used in this test.
class dummy_timing_notifier : public ru_timing_notifier
{
public:
  // See interface for documentation.
  void on_tti_boundary(slot_point slot_) override
  {
    if (!slot_synchronized) {
      slot_val          = (slot_ + processing_delay_slots).to_uint();
      slot_synchronized = true;
      fmt::print("Initial slot set to {}\n", slot_point(slot_.numerology(), slot_val));
    }
  }

  // See interface for documentation.
  void on_ul_half_slot_boundary(slot_point slot) override {}

  // See interface for documentation.
  void on_ul_full_slot_boundary(slot_point slot) override {}
};

/// RU emulator class responsible for generating uplink packets with random IQ data.
class test_ru_emulator
{
  /// Ethernet packet size, set to the value used by OFH implementation.
  static constexpr unsigned ethernet_frame_size = 9000;

  /// Helper structure used internally to group OFH header parameters.
  struct header_parameters {
    uint8_t  port;
    unsigned payload_size;
    unsigned start_prb;
    unsigned nof_prbs;
  };

public:
  /// Constructor.
  test_ru_emulator(srslog::basic_logger& logger_,
                   task_executor&        executor_,
                   test_ether_receiver&  receiver_,
                   ru_compression_params compr_params_,
                   unsigned              nof_prb_) :
    logger(logger_), executor(executor_), receiver(receiver_), compr_params(compr_params_), nof_prb(nof_prb_)
  {
    ul_eaxc.assign(test_params.ul_port_id.begin(), test_params.ul_port_id.end());
    prepare_test_data();

    for (unsigned K = 0; K != MAX_SUPPORTED_EAXC_ID_VALUE; ++K) {
      seq_counters.insert(K, 0);
    }
  }

  /// Generates UL packets with random IQ data for the specified slot and sends to loopback ethernet interface.
  void send_uplink_data(slot_point slot)
  {
    if (!executor.execute([this, slot]() { send_uplink(slot); })) {
      logger.warning("Failed to dispatch uplink task");
    }
  }

private:
  void send_uplink(slot_point slot)
  {
    for (unsigned symbol = 0; symbol != get_nsymb_per_slot(cyclic_prefix::NORMAL); ++symbol) {
      // Prepare symbol for all eAxC.
      for (unsigned eaxc_id = 0, end = ul_eaxc.size(); eaxc_id != end; ++eaxc_id) {
        auto& frames = test_data[eaxc_id];
        for (auto& frame : frames) {
          set_header_parameters(frame, slot, symbol, ul_eaxc[eaxc_id]);
        }
      }
      // Send symbol.
      for (const auto& frames : test_data) {
        send(frames);
      }
    }
    logger.info("RU sent UL in slot {}", slot);
  }

  /// Sends byte arrays to the loopback ethernet interface.
  void send(const std::vector<std::vector<uint8_t>>& frames)
  {
    for (const auto& frame : frames) {
      receiver.push_new_data(frame);
    }
  }

  void set_header_parameters(span<uint8_t> frame, slot_point slot, unsigned symbol, unsigned eaxc)
  {
    // Real receiver sends VLAN parameters as part of a Ethernet frame.
    unsigned offset = (test_params.use_loopback_receiver) ? 4 : 0;

    // Set timestamp.
    uint8_t octet      = 0;
    frame[23 + offset] = uint8_t(slot.sfn());
    // Subframe index; offset: 4, 4 bits long.
    octet |= uint8_t(slot.subframe_index()) << 4u;
    // Four MSBs of the slot index within 1ms subframe; offset: 4, 6 bits long.
    octet |= uint8_t(slot.subframe_slot_index() >> 2u);
    frame[24 + offset] = octet;

    octet = 0;
    octet |= uint8_t(slot.subframe_slot_index() & 0x3) << 6u;
    octet |= uint8_t(symbol);
    frame[25 + offset] = octet;

    // Set sequence index.
    uint8_t& seq_id    = seq_counters[eaxc];
    frame[20 + offset] = seq_id++;
  }

  void initialize_header(span<uint8_t> frame, header_parameters params) const
  {
    // Doesn't include VLAN header, as the OFH receiver expects a NIC to strip it.
    static const uint8_t hdr_template_no_vlan[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0xae, 0xfe, 0x10, 0x00, 0x1d, 0xea, 0x00, 0x00, 0x00, 0x80,
                                                   0x10, 0xee, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x00};

    static const uint8_t hdr_template_vlan[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                0x81, 0x00, 0x00, 0x02, 0xae, 0xfe, 0x10, 0x00, 0x1d, 0xea, 0x00, 0x00,
                                                0x00, 0x80, 0x10, 0xee, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x00};

    const uint8_t* hdr_template_ptr = (test_params.use_loopback_receiver) ? hdr_template_vlan : hdr_template_no_vlan;
    const size_t   hdr_template_size =
        (test_params.use_loopback_receiver) ? sizeof(hdr_template_vlan) : sizeof(hdr_template_no_vlan);
    unsigned offset = (test_params.use_loopback_receiver) ? 4 : 0;

    // Copy default header.
    std::memcpy(frame.data(), hdr_template_ptr, hdr_template_size);

    // Set VLAN tag.
    if (test_params.use_loopback_receiver) {
      frame[15] = vlan_tag;
    }

    // Set correct payload size.
    uint16_t payload_size = htons(params.payload_size);
    std::memcpy(&frame[16 + offset], &payload_size, sizeof(uint16_t));

    // Set port ID.
    frame[19 + offset] = params.port;

    // Set start PRB and number of PRBs.
    frame[27 + offset] = uint8_t(params.start_prb >> 8u) & 0x3;
    frame[28 + offset] = uint8_t(params.start_prb);
    frame[29 + offset] = uint8_t((params.nof_prbs == nof_prb) ? 0 : params.nof_prbs);

    // Set compression header.
    uint8_t octet = 0U;
    octet |= uint8_t(compr_params.data_width) << 4U;
    octet |= uint8_t(to_value(compr_params.type));
    frame[30 + offset] = octet;
  }

  static void fill_random_data(span<uint8_t> frame)
  {
    std::uniform_int_distribution<uint8_t> dist{0, 255};
    std::generate(frame.begin(), frame.end(), [&]() { return dist(rgen); });
  }

  void prepare_test_data()
  {
    units::bytes ecpri_iq_data_header_size(8);
    units::bytes ofh_header_size(10);
    units::bytes ether_header_size(14);
    if (test_params.use_loopback_receiver) {
      // VLAN parameters are added in case real Ethernet receiver is used.
      ether_header_size = units::bytes(18);
    }
    unsigned headers_size = (ether_header_size + ecpri_iq_data_header_size + ofh_header_size).value();

    prb_size = units::bits(compr_params.data_width * NOF_SUBCARRIERS_PER_RB * 2 +
                           (compr_params.type == compression_type::BFP ? 8 : 0))
                   .round_up_to_bytes();
    unsigned iq_data_size = nof_prb * prb_size.value();

    // It is assumed that maximum 2 packets required to send all data for antenna.
    unsigned              nof_frames = ((headers_size + iq_data_size) > ethernet_frame_size) ? 2u : 1u;
    std::vector<unsigned> nof_frame_prbs;
    if (nof_frames == 1) {
      nof_frame_prbs.push_back(nof_prb);
    } else {
      unsigned nof_prbs_first  = (ethernet_frame_size - headers_size) / prb_size.value();
      unsigned nof_prbs_second = nof_prb - nof_prbs_first;
      nof_frame_prbs.push_back(nof_prbs_first);
      nof_frame_prbs.push_back(nof_prbs_second);
    }

    /// Initializes ethernet packet headers for all antennas. Timestamp, sequence index and symbol index will be updated
    /// on every transmission.
    for (unsigned port = 0; port != nof_antennas_ul; ++port) {
      test_data.emplace_back();
      std::vector<std::vector<uint8_t>>& ether_frames = test_data.back();

      unsigned start_prb = 0;
      for (unsigned j = 0; j != nof_frames; ++j) {
        unsigned data_size = nof_frame_prbs[j] * prb_size.value();

        ether_frames.emplace_back();
        std::vector<uint8_t>& frame = ether_frames.back();
        frame.resize(headers_size + data_size);

        // Prepare header.
        span<uint8_t>     frame_header(frame.data(), headers_size);
        header_parameters params;
        params.port         = port;
        params.payload_size = data_size + ofh_header_size.value() + ecpri::ECPRI_COMMON_HEADER_SIZE.value();
        params.start_prb    = start_prb;
        params.nof_prbs     = nof_frame_prbs[j];
        initialize_header(frame_header, params);

        // Prepare IQ data.
        fill_random_data(span<uint8_t>(frame).last(data_size));

        start_prb += nof_frame_prbs[j];
      }
    }
  }

private:
  srslog::basic_logger&       logger;
  task_executor&              executor;
  test_ether_receiver&        receiver;
  const ru_compression_params compr_params;
  const unsigned              nof_prb;
  units::bytes                prb_size;
  /// Stores byte arrays for each antenna.
  std::vector<std::vector<std::vector<uint8_t>>>               test_data;
  circular_map<unsigned, uint8_t, MAX_SUPPORTED_EAXC_ID_VALUE> seq_counters;
  static_vector<unsigned, ofh::MAX_NOF_SUPPORTED_EAXC>         ul_eaxc;
};

/// DU emulator that pushes resource grids to the OFH RU implementation.
class test_du_emulator
{
public:
  test_du_emulator(srslog::basic_logger&      logger_,
                   task_executor&             executor_,
                   ru_downlink_plane_handler& dl_handler_,
                   ru_uplink_plane_handler&   ul_handler_,
                   unsigned                   nof_prb_) :
    logger(logger_), executor(executor_), dl_handler(dl_handler_), ul_handler(ul_handler_), nof_prb(nof_prb_)
  {
    std::uniform_real_distribution<float> dist(-1.0, +1.0);

    // Create resource grids according to TDD pattern.
    for (unsigned rg_id = 0; rg_id != tdd_pattern.nof_dl_slots; rg_id++) {
      // Downlink.
      dl_resource_grids.push_back(std::make_unique<resource_grid_impl>(
          nof_antennas_dl, get_nsymb_per_slot(cyclic_prefix::NORMAL), nof_prb * NOF_SUBCARRIERS_PER_RB, nullptr));
      resource_grid_impl&   grid      = *dl_resource_grids.back();
      resource_grid_writer& rg_writer = grid.get_writer();

      // Pre-generate random downlink data.
      for (unsigned sym = 0; sym != get_nsymb_per_slot(cyclic_prefix::NORMAL); ++sym) {
        for (unsigned port = 0; port != nof_antennas_dl; ++port) {
          std::vector<cf_t> test_data(nof_prb * NOF_SUBCARRIERS_PER_RB);
          std::generate(test_data.begin(), test_data.end(), [&]() { return cf_t{dist(rgen), dist(rgen)}; });
          rg_writer.put(port, sym, 0, test_data);
        }
      }
    }
    // Uplink.
    for (unsigned rg_id = 0; rg_id != tdd_pattern.nof_ul_slots; rg_id++) {
      ul_resource_grids.push_back(std::make_unique<resource_grid_impl>(
          nof_antennas_ul, get_nsymb_per_slot(cyclic_prefix::NORMAL), nof_prb * NOF_SUBCARRIERS_PER_RB, nullptr));
    }
  }

  /// Starts the DU emulator.
  void start()
  {
    slot_point slot(0, to_numerology_value(test_params.scs));
    slot_duration_us   = std::chrono::microseconds(1000 * SUBFRAME_DURATION_MSEC / slot.nof_slots_per_subframe());
    symbol_duration_us = std::chrono::microseconds(static_cast<unsigned>(
        std::ceil(1e3 / (get_nsymb_per_slot(cyclic_prefix::NORMAL) * get_nof_slots_per_subframe(test_params.scs)))));
    if (!executor.execute([this]() { run_test(); })) {
      report_fatal_error("Failed to start DU emulator");
    }
  }

  bool is_test_finished() const { return test_finished.load(std::memory_order_relaxed); }

private:
  void run_test()
  {
    for (unsigned test_slot_id = 0; test_slot_id != nof_test_slots; ++test_slot_id) {
      auto t0 = std::chrono::high_resolution_clock::now();

      slot_point slot(to_numerology_value(test_params.scs), slot_val);
      unsigned   slot_id    = slot.slot_index() % tdd_pattern.dl_ul_tx_period_nof_slots;
      bool       is_dl_slot = (slot_id < tdd_pattern.nof_dl_slots);
      bool       is_ul_slot = (slot_id >= tdd_pattern.dl_ul_tx_period_nof_slots - tdd_pattern.nof_ul_slots);

      // Push downlink data.
      if (is_dl_slot) {
        resource_grid_impl&   grid = *dl_resource_grids[slot_id];
        resource_grid_context context{slot, 0};
        dl_handler.handle_dl_data(context, grid.get_reader());
        logger.info("DU emulator pushed DL data in slot {}", slot);
      }

      // Request uplink data.
      if (is_ul_slot) {
        slot_id = tdd_pattern.dl_ul_tx_period_nof_slots - slot_id - 1;
        resource_grid_context context{slot, 0};
        ul_handler.handle_new_uplink_slot(context, *ul_resource_grids[slot_id]);
      }

      // Sleep until the end of the slot.
      auto t1                 = std::chrono::high_resolution_clock::now();
      auto slot_sim_exec_time = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
      std::this_thread::sleep_for(slot_duration_us - slot_sim_exec_time - 5us);
      slot_val = (++slot).to_uint();
    }
    // Leave time fo the uplink slots to be processed.
    auto proc_time = processing_delay_slots * slot_duration_us + (T1a_max_cp_ul * symbol_duration_us) + 100ms;
    std::this_thread::sleep_for(proc_time);
    test_finished.store(true, std::memory_order_relaxed);
  }

  srslog::basic_logger&                            logger;
  std::vector<std::unique_ptr<resource_grid_impl>> dl_resource_grids;
  std::vector<std::unique_ptr<resource_grid_impl>> ul_resource_grids;
  task_executor&                                   executor;
  ru_downlink_plane_handler&                       dl_handler;
  ru_uplink_plane_handler&                         ul_handler;

  const unsigned            nof_prb;
  std::chrono::microseconds slot_duration_us;
  std::chrono::microseconds symbol_duration_us;
  std::atomic<bool>         test_finished{false};
};

/// Ethernet transmitter gateway that analyzes incoming packets and checks integrity of the DL packets, as well as asks
/// RU emulator for UL traffic generation.
class test_gateway : public ether::gateway
{
public:
  test_gateway() :
    scs(test_params.scs),
    nof_symbols(get_nsymb_per_slot(cyclic_prefix::NORMAL)),
    seq_counter_initialized(MAX_SUPPORTED_EAXC_ID_VALUE)
  {
    for (unsigned K = 0; K != MAX_SUPPORTED_EAXC_ID_VALUE; ++K) {
      seq_counters.insert(K, 0);
    }
  }

  void connect_ru(test_ru_emulator* ru_emulator_) { ru_emulator = ru_emulator_; }

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override
  {
    slot_point sent_ul_slot = {};
    for (auto frame : frames) {
      // For DL messages check seq id and make sure packets for all antennas were transmitted.
      if (peek_direction(frame) == data_direction::downlink) {
        if (peek_message_type(frame) == ecpri::message_type::iq_data) {
          check_and_update_sequence_id(frame);
        }
        continue;
      }
      // For UL message ask the RU emulator to send UP packets to the loopback interface.
      srsran_assert(ru_emulator != nullptr, "RU emulator uninitialized");
      slot_point slot = peek_slot_point(frame);
      if (slot != sent_ul_slot) {
        ru_emulator->send_uplink_data(slot);
        sent_ul_slot = slot;
      }
    }
  }

private:
  void check_and_update_sequence_id(span<const uint8_t> message)
  {
    // Retrieve eAxC and SeqID from codified message.
    unsigned seq_id = message[24];
    unsigned eaxc   = (unsigned(message[22]) << 8u | message[23]);

    srsran_assert(eaxc < MAX_SUPPORTED_EAXC_ID_VALUE, "Invalid eAxC={} detected", eaxc);

    if (!seq_counter_initialized.test(eaxc)) {
      seq_counter_initialized.set(eaxc);
      seq_counters[eaxc] = seq_id + 1;
      return;
    }

    uint8_t& expected_seq_id = seq_counters[eaxc];
    if (seq_id == expected_seq_id) {
      expected_seq_id++;
      return;
    }

    nof_missing_dl_packets++;
    expected_seq_id = seq_id + 1;
  }

  static data_direction peek_direction(span<const uint8_t> message)
  {
    if (message.size() < 27) {
      nof_malformed_packets++;
      return data_direction::downlink;
    }
    // Filter index is codified in the byte 26, bit 7.
    unsigned direction = (message[26] & 0x80) >> 7u;
    return (direction == 1) ? data_direction::downlink : data_direction::uplink;
  }

  static ecpri::message_type peek_message_type(span<const uint8_t> message)
  {
    return static_cast<ecpri::message_type>(message[19]);
  }

  slot_point peek_slot_point(span<const uint8_t> message) const
  {
    // Slot is codified in the bytes 27-29 of the Ethernet packet.
    if (message.size() < 30) {
      return slot_point{};
    }

    uint8_t  frame             = message[27];
    uint8_t  subframe_and_slot = message[28];
    uint8_t  subframe          = subframe_and_slot >> 4;
    unsigned slot_id           = 0;
    slot_id |= (subframe_and_slot & 0x0f) << 2;

    uint8_t slot_and_symbol = message[29];
    slot_id |= slot_and_symbol >> 6;

    return {to_numerology_value(scs), frame, subframe, slot_id};
  }

private:
  const subcarrier_spacing                                     scs;
  const unsigned                                               nof_symbols;
  circular_map<unsigned, uint8_t, MAX_SUPPORTED_EAXC_ID_VALUE> seq_counters;
  bounded_bitset<MAX_SUPPORTED_EAXC_ID_VALUE>                  seq_counter_initialized;
  test_ru_emulator*                                            ru_emulator;
};

/// Manages the workers of the test application and OFH RU.
struct worker_manager {
  static constexpr uint32_t task_worker_queue_size = 2048;

  worker_manager() { create_ofh_executors(); }

  void stop() { exec_mng.stop(); }

  void create_ofh_executors()
  {
    using namespace execution_config_helper;

    // Timing executor.
    {
      const std::string name      = "ru_timing";
      const std::string exec_name = "ru_timing_exec";

      const single_worker ru_worker{name,
                                    {concurrent_queue_policy::lockfree_spsc, 4},
                                    {{exec_name}},
                                    std::chrono::microseconds{0},
                                    os_thread_realtime_priority::max() - 0};
      if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_timing_exec = exec_mng.executors().at(exec_name);
    }

    // Executor for the Open Fronthaul User and Control messages codification.
    {
      unsigned          nof_workers = (test_params.is_downlink_parallelized) ? std::max(nof_antennas_dl / 2U, 1U) : 1U;
      const std::string name        = "ru_dl";
      const std::string exec_name   = "ru_dl_exec";

      const worker_pool ru_pool{name,
                                nof_workers,
                                {{concurrent_queue_policy::locking_mpmc, task_worker_queue_size}},
                                {{exec_name}},
                                std::chrono::microseconds(0),
                                os_thread_realtime_priority::max() - 5,
                                {}};
      if (!exec_mng.add_execution_context(create_execution_context(ru_pool))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_pool.name);
      }
      ru_dl_exec = exec_mng.executors().at(exec_name);
    }

    // Executor for Open Fronthaul messages transmission.
    {
      const std::string name      = "ru_tx";
      const std::string exec_name = "ru_tx_exec";

      const single_worker ru_worker{name,
                                    {concurrent_queue_policy::lockfree_spsc, task_worker_queue_size},
                                    {{exec_name}},
                                    std::chrono::microseconds{5},
                                    os_thread_realtime_priority::max() - 1};
      if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_tx_exec = exec_mng.executors().at(exec_name);
    }

    // Executor for Open Fronthaul messages reception.
    {
      const std::string name      = "ru_rx";
      const std::string exec_name = "ru_rx_exec";

      const single_worker ru_worker{name,
                                    {concurrent_queue_policy::lockfree_spsc, task_worker_queue_size},
                                    {{exec_name}},
                                    std::chrono::microseconds{1},
                                    os_thread_realtime_priority::max() - 1};
      if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_rx_exec = exec_mng.executors().at(exec_name);
    }

    // Executor for DU emulator responsible for resource grids generation.
    {
      const std::string name      = "du_sim";
      const std::string exec_name = "du_sim_exec";

      const single_worker du_sim_worker{name,
                                        {concurrent_queue_policy::lockfree_spsc, 2},
                                        {{exec_name}},
                                        std::chrono::microseconds{1},
                                        os_thread_realtime_priority::max() - 10};
      if (!exec_mng.add_execution_context(create_execution_context(du_sim_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", du_sim_worker.name);
      }
      test_du_sim_exec = exec_mng.executors().at(exec_name);
    }

    // Executor for RU emulator responsible for rx packets generation.
    {
      const std::string name      = "ru_sim";
      const std::string exec_name = "ru_sim_exec";

      const single_worker ru_sim_worker{name,
                                        {concurrent_queue_policy::lockfree_spsc, task_worker_queue_size},
                                        {{exec_name}},
                                        std::chrono::microseconds{1},
                                        os_thread_realtime_priority::max() - 8};
      if (!exec_mng.add_execution_context(create_execution_context(ru_sim_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_sim_worker.name);
      }
      test_ru_sim_exec = exec_mng.executors().at(exec_name);
    }
  }

  task_execution_manager exec_mng;
  task_executor*         ru_timing_exec = nullptr;
  task_executor*         ru_dl_exec;
  task_executor*         ru_tx_exec;
  task_executor*         ru_rx_exec;
  task_executor*         test_du_sim_exec;
  task_executor*         test_ru_sim_exec;
};
} // namespace

static void configure_ofh_sector(ru_ofh_sector_configuration& sector_cfg)
{
  // Default IQ data scaling to be applied prior to downlink data compression.
  const float iq_scaling = 0.9f;

  std::chrono::duration<double, std::nano> symbol_duration(
      (1e6 / (get_nsymb_per_slot(cyclic_prefix::NORMAL) * get_nof_slots_per_subframe(test_params.scs))));

  sector_cfg.interface                       = "lo";
  sector_cfg.mac_src_address                 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  sector_cfg.mac_dst_address                 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  sector_cfg.mtu_size                        = test_params.mtu;
  sector_cfg.tci                             = vlan_tag;
  sector_cfg.scs                             = test_params.scs;
  sector_cfg.bw                              = test_params.bw;
  sector_cfg.cp                              = cyclic_prefix::NORMAL;
  sector_cfg.is_prach_control_plane_enabled  = test_params.is_prach_control_plane_enabled;
  sector_cfg.ignore_ecpri_payload_size_field = test_params.ignore_ecpri_payload_size_field;
  sector_cfg.tx_window_timing_params         = {
              T1a_max_cp_dl, T1a_min_cp_dl, T1a_max_cp_ul, T1a_min_cp_ul, T1a_max_up, T1a_min_up};
  sector_cfg.rx_window_timing_params       = {Ta4_min, Ta4_max};
  sector_cfg.is_downlink_broadcast_enabled = test_params.is_downlink_broadcast_enabled;

  // Configure compression
  ru_compression_params dl_ul_compression_params{to_compression_type(test_params.data_compr_method),
                                                 test_params.data_bitwidth};
  ru_compression_params prach_compression_params{to_compression_type(test_params.prach_compr_method),
                                                 test_params.prach_bitwidth};
  sector_cfg.dl_compression_params               = dl_ul_compression_params;
  sector_cfg.ul_compression_params               = dl_ul_compression_params;
  sector_cfg.prach_compression_params            = prach_compression_params;
  sector_cfg.iq_scaling                          = iq_scaling;
  sector_cfg.is_downlink_static_comp_hdr_enabled = test_params.is_downlink_static_comp_hdr_enabled;
  sector_cfg.is_uplink_static_comp_hdr_enabled   = test_params.is_uplink_static_comp_hdr_enabled;

  // Configure eAxCs.
  sector_cfg.prach_eaxc.assign(test_params.prach_port_id.begin(), test_params.prach_port_id.end());
  sector_cfg.dl_eaxc.assign(test_params.dl_port_id.begin(), test_params.dl_port_id.end());
  sector_cfg.ul_eaxc.assign(test_params.ul_port_id.begin(), test_params.ul_port_id.end());
  sector_cfg.nof_antennas_ul = nof_antennas_ul;
}

static ru_ofh_configuration generate_ru_config()
{
  // Downlink processing time in microseconds.
  const std::chrono::microseconds dl_processing_time = 400us;

  ru_ofh_configuration ru_cfg;
  ru_cfg.max_processing_delay_slots = processing_delay_slots;
  ru_cfg.gps_Alpha                  = 0;
  ru_cfg.gps_Beta                   = 0;
  ru_cfg.dl_processing_time         = dl_processing_time;
  ru_cfg.uses_dpdk                  = false;

  ru_cfg.sector_configs.emplace_back();
  ru_ofh_sector_configuration& sector_cfg = ru_cfg.sector_configs.back();
  configure_ofh_sector(sector_cfg);

  return ru_cfg;
}

static ru_ofh_dependencies generate_ru_dependencies(srslog::basic_logger&               logger,
                                                    worker_manager&                     workers,
                                                    ru_timing_notifier*                 timing_notifier,
                                                    ru_uplink_plane_rx_symbol_notifier* rx_symbol_notifier,
                                                    test_gateway*&                      tx_gateway,
                                                    test_ether_receiver*&               eth_receiver,
                                                    ru_error_notifier&                  error_notifier)
{
  ru_ofh_dependencies dependencies;
  dependencies.logger             = &logger;
  dependencies.timing_notifier    = timing_notifier;
  dependencies.rx_symbol_notifier = rx_symbol_notifier;
  dependencies.rt_timing_executor = workers.ru_timing_exec;
  dependencies.error_notifier     = &error_notifier;

  dependencies.sector_dependencies.emplace_back();
  auto& sector_deps                = dependencies.sector_dependencies.back();
  sector_deps.logger               = &logger;
  sector_deps.downlink_executor    = workers.ru_dl_exec;
  sector_deps.receiver_executor    = workers.ru_rx_exec;
  sector_deps.transmitter_executor = workers.ru_tx_exec;

  // Configure Ethernet gateway.
  auto gateway            = std::make_unique<test_gateway>();
  tx_gateway              = gateway.get();
  sector_deps.eth_gateway = std::move(gateway);

  // Configure Ethernet receiver.
  auto dummy_receiver      = std::make_unique<dummy_eth_receiver>(logger, *workers.ru_rx_exec);
  eth_receiver             = dummy_receiver.get();
  sector_deps.eth_receiver = std::move(dummy_receiver);

  return dependencies;
}

int main(int argc, char** argv)
{
  std::unique_ptr<test_ether_receiver> eth_receiver_ptr;
  parse_args(argc, argv);

  // Set up logging.
  srslog::sink* log_sink = (test_params.log_filename == "stdout") ? srslog::create_stdout_sink()
                                                                  : srslog::create_file_sink(test_params.log_filename);
  if (log_sink == nullptr) {
    report_error("Could not create application main log sink.\n");
  }
  srslog::set_default_sink(*log_sink);
  srslog::init();

  srslog::basic_logger& logger = srslog::fetch_basic_logger("OFH_TEST", false);
  logger.set_level(srslog::str_to_basic_level(test_params.log_level));

  unsigned nof_prb = get_max_Nprb(bs_channel_bandwidth_to_MHz(test_params.bw), test_params.scs, frequency_range::FR1);

  worker_manager           workers;
  dummy_rx_symbol_notifier rx_symbol_notifier;
  dummy_timing_notifier    timing_notifier;
  test_gateway*            tx_gateway;
  test_ether_receiver*     eth_receiver;
  dummy_ru_error_notifier  error_notifier;

  ru_ofh_configuration ru_cfg  = generate_ru_config();
  ru_ofh_dependencies  ru_deps = generate_ru_dependencies(
      logger, workers, &timing_notifier, &rx_symbol_notifier, tx_gateway, eth_receiver, error_notifier);

  if (test_params.use_loopback_receiver) {
    ru_deps.sector_dependencies[0].eth_receiver.reset();
    eth_receiver_ptr = std::make_unique<lo_eth_receiver>(logger);
    eth_receiver     = eth_receiver_ptr.get();
  }
  std::unique_ptr<radio_unit> ru_object = create_ofh_ru(ru_cfg, std::move(ru_deps));

  // Get RU downlink plane handler.
  auto& ru_dl_handler = ru_object->get_downlink_plane_handler();
  auto& ru_ul_handler = ru_object->get_uplink_plane_handler();

  if (test_params.use_loopback_receiver) {
    eth_receiver_ptr = std::make_unique<lo_eth_receiver>(logger);
    eth_receiver     = eth_receiver_ptr.get();
  }

  // Create RU emulator instance.
  ru_compression_params ul_compression_params{to_compression_type(test_params.data_compr_method),
                                              test_params.data_bitwidth};
  test_ru_emulator      ru_emulator(logger, *workers.test_ru_sim_exec, *eth_receiver, ul_compression_params, nof_prb);

  // Create DU emulator instance.
  test_du_emulator du_emulator(logger, *workers.test_du_sim_exec, ru_dl_handler, ru_ul_handler, nof_prb);

  // Connect Ethernet gateway to the RU emulator.
  tx_gateway->connect_ru(&ru_emulator);

  // Start the RU.
  fmt::print("Starting RU...\n");
  ru_object->get_controller().start();

  // Wait until TTI callback is called and slot point gets initialized.
  while (!slot_synchronized) {
    std::this_thread::sleep_for(std::chrono::microseconds(2));
  }
  // Start the DU emulator.
  du_emulator.start();
  fmt::print("Running the test...\n");

  // Wait until test is finished.
  while (!du_emulator.is_test_finished()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  fmt::print("DU emulator stopped\n");

  fmt::print("Stopping the RU...\n");
  ru_object->get_controller().stop();
  fmt::print("RU stopped successfully.\n");

  workers.stop();
  srslog::flush();

  fmt::print("Test finished, nof_missing_dl_packets={}, nof_malformed_packets={}\n",
             nof_missing_dl_packets,
             nof_malformed_packets);

  return 0;
}
