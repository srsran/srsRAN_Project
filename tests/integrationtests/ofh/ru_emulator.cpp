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

#include "../../../lib/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "helpers.h"
#include "ru_emulator_appconfig.h"
#include "ru_emulator_cli11_schema.h"
#include "srsran/adt/circular_map.h"
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#include "srsran/ofh/compression/compression_params.h"
#include "srsran/ofh/ecpri/ecpri_constants.h"
#include "srsran/ofh/ecpri/ecpri_packet_properties.h"
#include "srsran/ofh/ethernet/ethernet_frame_notifier.h"
#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/ofh/ethernet/ethernet_mac_address.h"
#include "srsran/ofh/ethernet/ethernet_properties.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/serdes/ofh_message_properties.h"
#include "srsran/ran/resource_block.h"
#include "srsran/ran/slot_point.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/config_parsers.h"
#include "srsran/support/executors/task_execution_manager.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/format_utils.h"
#include "srsran/support/signal_handler.h"
#include "fmt/chrono.h"
#include <future>
#include <random>
#include <rte_ethdev.h>

using namespace srsran;
using namespace ofh;
using namespace ether;

/// Ethernet packet size.
static constexpr unsigned ETHERNET_FRAME_SIZE = 9000;

namespace {

/// RU emulator configuration structure.
struct ru_emulator_config {
  /// Static compression parameters.
  ru_compression_params compr_params;
  /// Cell bandwidth in number of PRBs.
  unsigned nof_prb;
  /// RU emulator Ethernet MAC address.
  mac_address ru_mac;
  /// DU Ethernet MAC address.
  mac_address du_mac;
  /// VLAN tag.
  unsigned vlan_tag;
};

/// Helper structure used to group OFH header parameters.
struct header_parameters {
  uint8_t  port;
  unsigned payload_size;
  unsigned start_prb;
  unsigned nof_prbs;
};

} // namespace

/// Fills the given array with random bytes.
static void fill_random_data(span<uint8_t> frame, unsigned seed)
{
  std::mt19937                           rgen(seed);
  std::uniform_int_distribution<uint8_t> dist{0, 255};
  std::generate(frame.begin(), frame.end(), [&]() { return dist(rgen); });
}

/// Fills static OFH header parameters given the static RU config.
void set_static_header_params(span<uint8_t> frame, header_parameters params, const ru_emulator_config& cfg)
{
  static const uint8_t hdr_template[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                         0x81, 0x00, 0x00, 0x02, 0xae, 0xfe, 0x10, 0x00, 0x1d, 0xea, 0x00, 0x00,
                                         0x00, 0x80, 0x10, 0xee, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x00};

  // Copy default header.
  std::memcpy(frame.data(), hdr_template, sizeof(hdr_template));

  // Set MAC addresses.
  std::memcpy(&frame[0], cfg.du_mac.data(), ETH_ADDR_LEN);
  std::memcpy(&frame[ETH_ADDR_LEN], cfg.ru_mac.data(), ETH_ADDR_LEN);

  // Set VLAN tag.
  frame[15] = cfg.vlan_tag;

  // Set correct payload size.
  uint16_t payload_size = ::htons(params.payload_size);
  std::memcpy(&frame[20], &payload_size, sizeof(uint16_t));

  // Set port ID.
  frame[23] = params.port;

  // Set start PRB and number of PRBs.
  frame[31] = uint8_t(params.start_prb >> 8u) & 0x3;
  frame[32] = uint8_t(params.start_prb);
  frame[33] = uint8_t((params.nof_prbs == cfg.nof_prb) ? 0 : params.nof_prbs);

  // Set compression header.
  uint8_t octet = 0U;
  octet |= uint8_t(cfg.compr_params.data_width) << 4U;
  octet |= uint8_t(to_value(cfg.compr_params.type));
  frame[34] = octet;
}

/// Returns pre-generated test data for each symbol for each configured eAxC.
std::vector<std::vector<std::vector<std::vector<uint8_t>>>> generate_test_data(const ru_emulator_config& cfg,
                                                                               span<const unsigned>      ul_eaxc)
{
  // Vector of bytes for each frame (up to 2) of each OFDM symbol of each eAxC.
  std::vector<std::vector<std::vector<std::vector<uint8_t>>>> test_data;

  const units::bytes ecpri_iq_data_header_size(8);
  const units::bytes ofh_header_size(10);
  const units::bytes ether_header_size(18);

  unsigned headers_size = (ether_header_size + ecpri_iq_data_header_size + ofh_header_size).value();
  // Size in bytes of one PRB using the given static compression parameters.
  units::bytes prb_size = units::bits(cfg.compr_params.data_width * NOF_SUBCARRIERS_PER_RB * 2 +
                                      (cfg.compr_params.type == compression_type::BFP ? 8 : 0))
                              .round_up_to_bytes();
  unsigned iq_data_size = cfg.nof_prb * prb_size.value();

  // It is assumed that maximum 2 packets required to send symbol data for antenna.
  unsigned nof_frames = ((headers_size + iq_data_size) > ETHERNET_FRAME_SIZE) ? 2u : 1u;
  // Save number of PRBs to be sent in each allocated frame.
  std::vector<unsigned> nof_frame_prbs;
  if (nof_frames == 1) {
    nof_frame_prbs.push_back(cfg.nof_prb);
  } else {
    unsigned nof_prbs_first  = (ETHERNET_FRAME_SIZE - headers_size) / prb_size.value();
    unsigned nof_prbs_second = cfg.nof_prb - nof_prbs_first;
    nof_frame_prbs.push_back(nof_prbs_first);
    nof_frame_prbs.push_back(nof_prbs_second);
  }

  // Initializes IQ data and Ethernet packet headers for all configured eAxCs (timestamp and sequence index
  // will be updated on every transmission).
  for (unsigned port = 0, last = ul_eaxc.size(); port != last; ++port) {
    test_data.emplace_back();
    auto& eaxc_frames = test_data.back();

    for (unsigned symbol = 0, end = get_nsymb_per_slot(cyclic_prefix::NORMAL); symbol != end; ++symbol) {
      eaxc_frames.emplace_back();
      auto& symbol_frames = eaxc_frames.back();

      unsigned start_prb = 0;
      for (unsigned j = 0; j != nof_frames; ++j) {
        unsigned data_size = nof_frame_prbs[j] * prb_size.value();

        symbol_frames.emplace_back();
        std::vector<uint8_t>& frame = symbol_frames.back();
        frame.resize(headers_size + data_size);

        // Prepare header.
        span<uint8_t>     frame_header(frame.data(), headers_size);
        header_parameters params;
        params.port         = port;
        params.payload_size = data_size + ofh_header_size.value() + ecpri::ECPRI_COMMON_HEADER_SIZE.value();
        params.start_prb    = start_prb;
        params.nof_prbs     = nof_frame_prbs[j];

        set_static_header_params(frame_header, params, cfg);

        // Prepare IQ data.
        fill_random_data(span<uint8_t>(frame).last(data_size), ul_eaxc[port] + symbol);

        start_prb += nof_frame_prbs[j];
      }
    }
  }
  return test_data;
}

namespace {

/// Aggregates information received in UL C-Plane message.
struct uplink_request {
  unsigned   eaxc;
  slot_point slot;
};

/// Encapsulates DPDK Ethernet transmitter and receiver functionalities.
class dpdk_transceiver : public gateway, public receiver
{
  enum class status { idle, running, stop_requested, stopped };

public:
  dpdk_transceiver(srslog::basic_logger&              logger_,
                   task_executor&                     executor_,
                   std::shared_ptr<dpdk_port_context> port_ctx_ptr_) :
    logger(logger_), executor(executor_), port_ctx_ptr(port_ctx_ptr_), port_ctx(*port_ctx_ptr)
  {
    srsran_assert(port_ctx_ptr, "Invalid port context");
  }

  // See interface for documentation.
  void start(frame_notifier& notifier_) override
  {
    notifier = &notifier_;

    std::promise<void> p;
    std::future<void>  fut = p.get_future();

    if (!executor.defer([this, &p]() {
          trx_status.store(status::running, std::memory_order_relaxed);
          // Signal start() caller thread that the operation is complete.
          p.set_value();
          receive_loop();
        })) {
      report_error("Unable to start the DPDK ethernet frame receiver");
    }

    // Block waiting for timing executor to start.
    fut.wait();

    logger.info("Started the DPDK ethernet frame receiver");
  }

  // See interface for documentation.
  void stop() override
  {
    logger.info("Requesting stop of the DPDK ethernet frame receiver");
    trx_status.store(status::stop_requested, std::memory_order_relaxed);

    // Wait for the receiver thread to stop.
    while (trx_status.load(std::memory_order_acquire) != status::stopped) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    logger.info("Stopped the DPDK ethernet frame receiver");
  }

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override
  {
    if (frames.size() >= MAX_BURST_SIZE) {
      logger.warning("Unable to send a transmission burst size of '{}' frames in the DPDK Ethernet transmitter",
                     frames.size());
      return;
    }

    static_vector<::rte_mbuf*, MAX_BURST_SIZE> mbufs(frames.size());
    if (::rte_pktmbuf_alloc_bulk(port_ctx.get_mempool(), mbufs.data(), frames.size()) < 0) {
      logger.warning("Not enough entries in the mempool to send '{}' frames in the DPDK Ethernet transmitter",
                     frames.size());
      return;
    }

    for (unsigned idx = 0, end = frames.size(); idx != end; ++idx) {
      const auto  frame = frames[idx];
      ::rte_mbuf* mbuf  = mbufs[idx];

      if (::rte_pktmbuf_append(mbuf, frame.size()) == nullptr) {
        ::rte_pktmbuf_free(mbuf);
        logger.warning("Unable to append '{}' bytes to the allocated mbuf in the DPDK Ethernet transmitter",
                       frame.size());
        ::rte_pktmbuf_free_bulk(mbufs.data(), mbufs.size());
        return;
      }
      mbuf->data_len = frame.size();
      mbuf->pkt_len  = frame.size();

      uint8_t* data = rte_pktmbuf_mtod(mbuf, uint8_t*);
      std::memcpy(data, frame.data(), frame.size());
    }

    unsigned nof_sent_packets = ::rte_eth_tx_burst(port_ctx.get_port_id(), 0, mbufs.data(), mbufs.size());

    if (SRSRAN_UNLIKELY(nof_sent_packets < mbufs.size())) {
      logger.warning("DPDK dropped '{}' packets out of a total of '{}' in the tx burst",
                     mbufs.size() - nof_sent_packets,
                     mbufs.size());
      for (unsigned buf_idx = nof_sent_packets, last_idx = mbufs.size(); buf_idx != last_idx; ++buf_idx) {
        ::rte_pktmbuf_free(mbufs[buf_idx]);
      }
    }
  }

private:
  void receive_loop()
  {
    if (trx_status.load(std::memory_order_relaxed) == status::stop_requested) {
      trx_status.store(status::stopped, std::memory_order_release);
      return;
    }

    receive();

    // Retry the task deferring when it fails.
    while (!executor.defer([this]() { receive_loop(); })) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

  void receive()
  {
    std::array<::rte_mbuf*, MAX_BURST_SIZE> mbufs;
    unsigned num_frames = ::rte_eth_rx_burst(port_ctx.get_port_id(), 0, mbufs.data(), MAX_BURST_SIZE);
    if (num_frames == 0) {
      std::this_thread::sleep_for(std::chrono::microseconds(1));
      return;
    }

    for (auto mbuf : span<::rte_mbuf*>(mbufs.data(), num_frames)) {
      ::rte_vlan_strip(mbuf);

      uint8_t* data   = rte_pktmbuf_mtod(mbuf, uint8_t*);
      unsigned length = mbuf->pkt_len;
      notifier->on_new_frame(span<const uint8_t>(data, length));

      ::rte_pktmbuf_free(mbuf);
    }
  }

  srslog::basic_logger&              logger;
  task_executor&                     executor;
  std::shared_ptr<dpdk_port_context> port_ctx_ptr;
  dpdk_port_context&                 port_ctx;
  frame_notifier*                    notifier;
  std::atomic<status>                trx_status{status::idle};
};

/// Analyzes content of received OFH packets.
class packet_inspector
{
public:
  static bool is_uplink_cplane(span<const uint8_t> packet, srslog::basic_logger& logger)
  {
    // Drop malformed packet.
    if (packet.size() < 26) {
      logger.debug("Dropping packet of size smaller than 26 bytes");
      return false;
    }

    // Verify the Ethernet type is eCPRI.
    uint16_t eth_type = (uint16_t(packet[12]) << 8u) | packet[13];
    if (eth_type != ECPRI_ETH_TYPE) {
      logger.debug("Dropping packet as it is not of eCPRI type");
      return false;
    }

    // Skip U-Plane packets.
    auto message_type = static_cast<ecpri::message_type>(packet[15]);
    if (message_type == ecpri::message_type::iq_data) {
      logger.debug("Discarding U-Plane packet");
      return false;
    }

    // Check the filter index in the byte 26, bits 0-3.
    auto filter_index = static_cast<filter_index_type>(packet[22] & 0x0f);
    if (filter_index != filter_index_type::standard_channel_filter) {
      logger.debug("Discarding packet with non-standard filter");
      return false;
    }

    // Check direction, which is codified in the byte 26, bit 7.
    auto direction = static_cast<data_direction>((packet[22] & 0x80) >> 7u);
    logger.debug("Packet direction is {}", direction == data_direction::uplink ? "uplink" : "downlink");
    return direction == data_direction::uplink;
  }

  static uplink_request get_uplink_params(span<const uint8_t> packet)
  {
    uplink_request ul_request;

    // First, peek the eAxC.
    ul_request.eaxc = packet[19];

    // Second, peek the timestamp.
    // Slot is codified in the bytes 27-29 of the Ethernet packet.
    uint8_t  frame             = packet[23];
    uint8_t  subframe_and_slot = packet[24];
    uint8_t  subframe          = subframe_and_slot >> 4;
    unsigned slot_id           = 0;
    slot_id |= (subframe_and_slot & 0x0f) << 2;

    uint8_t slot_and_symbol = packet[25];
    slot_id |= slot_and_symbol >> 6;
    ul_request.slot = slot_point(to_numerology_value(subcarrier_spacing::kHz30), frame, subframe, slot_id);
    return ul_request;
  }
};

/// RU emulator receives OFH traffic and replies with UL packets to a DU.
class ru_emulator : public frame_notifier
{
  srslog::basic_logger& logger;
  task_executor&        executor;
  dpdk_transceiver&     transceiver;

  // RU emulator configuration.
  const ru_emulator_config cfg;
  // Pre-generated test data for each symbol for each configured eAxC.
  std::vector<std::vector<std::vector<std::vector<uint8_t>>>> test_data;
  // Keeps track of last used seq_id for each eAxC.
  circular_map<unsigned, uint8_t, MAX_SUPPORTED_EAXC_ID_VALUE> seq_counters;
  // Stores the list of configured eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  // Counts number of received UL C-Plane packets for PUSCH.
  std::atomic<unsigned> nof_rx_cplane_packets;

public:
  ru_emulator(srslog::basic_logger& logger_,
              task_executor&        executor_,
              dpdk_transceiver&     transceiver_,
              ru_emulator_config    cfg_,
              std::vector<unsigned> ul_eaxc_) :
    logger(logger_), executor(executor_), transceiver(transceiver_), cfg(cfg_), nof_rx_cplane_packets(0)
  {
    ul_eaxc.assign(ul_eaxc_.begin(), ul_eaxc_.end());
    for (auto eaxc : ul_eaxc) {
      srsran_assert(eaxc <= MAX_SUPPORTED_EAXC_ID_VALUE, "Unsupported eAxC value requested");
      seq_counters.insert(eaxc, 0);
    }

    test_data = generate_test_data(cfg, ul_eaxc);
  }

  // See interface for documentation.
  void on_new_frame(span<const uint8_t> payload) override
  {
    if (!executor.execute([this, payload, rx_payload = [payload]() -> std::array<uint8_t, MAX_BUFFER_SIZE> {
          std::array<uint8_t, MAX_BUFFER_SIZE> buffer;
          std::memcpy(buffer.data(), payload.data(), payload.size());
          return buffer;
        }()]() { process_new_frame(span<const uint8_t>(rx_payload.data(), payload.size())); })) {
      logger.warning("Failed to dispatch uplink task");
    }
  }

  void start() { transceiver.start(*this); }

  unsigned get_statistics() const { return nof_rx_cplane_packets.load(std::memory_order_relaxed); }

private:
  void process_new_frame(span<const uint8_t> payload)
  {
    if (!packet_inspector::is_uplink_cplane(payload, logger)) {
      return;
    }
    ++nof_rx_cplane_packets;

    auto ul_request = packet_inspector::get_uplink_params(payload);
    if (std::find(ul_eaxc.begin(), ul_eaxc.end(), ul_request.eaxc) == ul_eaxc.end()) {
      logger.info("Dropping received packet as its eAxC = '{}' is not configured in the RU emulator", ul_request.eaxc);
      return;
    }
    if (!ul_request.slot.valid()) {
      logger.warning("Dropping malformed packet as its timestamp is not correct");
      return;
    }

    // Set correct header parameters and send UL U-Plane packets for each symbol.
    static_vector<span<const uint8_t>, MAX_BURST_SIZE> frame_burst;
    auto&                                              eaxc_frames = test_data[ul_request.eaxc];

    for (unsigned symbol = 0, end = get_nsymb_per_slot(cyclic_prefix::NORMAL); symbol != end; ++symbol) {
      auto& symbol_frames = eaxc_frames[symbol];
      // Set runtime header parameters.
      for (auto& frame : symbol_frames) {
        set_runtime_header_params(frame, ul_request.slot, symbol, ul_request.eaxc);
        frame_burst.emplace_back(frame.data(), frame.size());
      }
    }
    // Send symbols.
    transceiver.send(frame_burst);
  }

  void set_runtime_header_params(span<uint8_t> frame, slot_point slot, unsigned symbol, unsigned eaxc)
  {
    // Set timestamp.
    uint8_t octet = 0;
    frame[27]     = uint8_t(slot.sfn());
    // Subframe index; offset: 4, 4 bits long.
    octet |= uint8_t(slot.subframe_index()) << 4u;
    // Four MSBs of the slot index within 1ms subframe; offset: 4, 6 bits long.
    octet |= uint8_t(slot.subframe_slot_index() >> 2u);
    frame[28] = octet;

    octet = 0;
    octet |= uint8_t(slot.subframe_slot_index() & 0x3) << 6u;
    octet |= uint8_t(symbol);
    frame[29] = octet;

    // Set sequence index.
    uint8_t& seq_id = seq_counters[eaxc];
    frame[24]       = seq_id++;
  }
};

/// Manages the workers of the RU emulators.
struct worker_manager {
  static constexpr uint32_t task_worker_queue_size = 128;

  worker_manager(unsigned nof_emulators) { create_executors(nof_emulators); }

  void create_executors(unsigned nof_emulators)
  {
    using namespace execution_config_helper;

    // Executor for Open Fronthaul messages reception.
    {
      const std::string name      = "ru_rx";
      const std::string exec_name = "ru_rx_exec";

      const single_worker ru_worker{name,
                                    {concurrent_queue_policy::lockfree_spsc, 2},
                                    {{exec_name}},
                                    std::chrono::microseconds{1},
                                    os_thread_realtime_priority::max() - 1};
      if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_rx_exec = exec_mng.executors().at(exec_name);
    }

    for (unsigned i = 0; i != nof_emulators; ++i) {
      // Executors for the RU emulators.
      {
        const std::string   name      = "ru_emu_#" + std::to_string(i);
        const std::string   exec_name = "ru_emu_exec_#" + std::to_string(i);
        const single_worker ru_worker{name,
                                      {concurrent_queue_policy::lockfree_spsc, task_worker_queue_size},
                                      {{exec_name}},
                                      std::chrono::microseconds{5},
                                      os_thread_realtime_priority::max() - 5};
        if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
          report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
        }
        ru_emulators_exec.push_back(exec_mng.executors().at(exec_name));
      }
    }
  }

  void stop() { exec_mng.stop(); }

  task_execution_manager      exec_mng;
  task_executor*              ru_rx_exec = nullptr;
  std::vector<task_executor*> ru_emulators_exec;
};

} // namespace

static std::string config_file;

const int                MAX_CONFIG_FILES(6);
static std::atomic<bool> is_running = {true};

static void local_signal_handler()
{
  is_running = false;
}

int main(int argc, char** argv)
{
  // Set signal handler.
  register_signal_handler(local_signal_handler);

  // Setup and configure config parsing.
  CLI::App app("RU emulator application");
  app.config_formatter(create_yaml_config_parser());
  app.allow_config_extras(CLI::config_extras_mode::error);
  app.set_config("-c,", config_file, "Read config from file", false)->expected(1, MAX_CONFIG_FILES);

  ru_emulator_appconfig ru_emulator_parsed_cfg;
  // Configure CLI11 with the RU emulator application configuration schema.
  configure_cli11_with_ru_emulator_appconfig_schema(app, ru_emulator_parsed_cfg);

  // Parse arguments.
  CLI11_PARSE(app, argc, argv);

  if (ru_emulator_parsed_cfg.ru_cfg.empty()) {
    report_error("Invalid configuration detected, at least one RU configuration must be provided\n");
  }

  // Set up logging.
  srslog::sink* log_sink = (ru_emulator_parsed_cfg.log_cfg.filename == "stdout")
                               ? srslog::create_stdout_sink()
                               : srslog::create_file_sink(ru_emulator_parsed_cfg.log_cfg.filename);
  if (log_sink == nullptr) {
    report_error("Could not create application main log sink.\n");
  }
  srslog::set_default_sink(*log_sink);
  srslog::init();

  srslog::basic_logger& logger = srslog::fetch_basic_logger("RU_EMU", false);
  logger.set_level(srslog::str_to_basic_level(ru_emulator_parsed_cfg.log_cfg.level));

  // Initialize DPDK EAL.
  std::unique_ptr<dpdk::dpdk_eal> eal;
  // Prepend the application name in argv[0] as it is expected by EAL.
  eal = dpdk::create_dpdk_eal(std::string(argv[0]) + " " + ru_emulator_parsed_cfg.dpdk_config.eal_args,
                              srslog::fetch_basic_logger("EAL", false));
  if (!eal) {
    report_error("Failed to initialize DPDK EAL\n");
  }

  // Create workers and executors.
  worker_manager workers(ru_emulator_parsed_cfg.ru_cfg.size());

  // Set up DPDK transceivers and create RU emulators.
  std::vector<std::unique_ptr<dpdk_transceiver>> transceivers;
  std::vector<std::unique_ptr<ru_emulator>>      ru_emulators;

  for (unsigned i = 0, e = ru_emulator_parsed_cfg.ru_cfg.size(); i != e; ++i) {
    ru_emulator_ofh_appconfig ru_cfg = ru_emulator_parsed_cfg.ru_cfg[i];
    dpdk_port_config          port_cfg;
    port_cfg.pcie_id                     = ru_cfg.network_interface;
    port_cfg.mtu_size                    = units::bytes{ETHERNET_FRAME_SIZE};
    port_cfg.is_promiscuous_mode_enabled = false;
    auto ctx                             = dpdk_port_context::create(port_cfg);
    transceivers.push_back(std::make_unique<dpdk_transceiver>(logger, *workers.ru_rx_exec, ctx));

    ru_emulator_config emu_cfg;
    emu_cfg.nof_prb =
        get_max_Nprb(bs_channel_bandwidth_to_MHz(ru_cfg.bandwidth), subcarrier_spacing::kHz30, frequency_range::FR1);
    emu_cfg.compr_params = {to_compression_type(ru_cfg.ul_compr_method), ru_cfg.ul_compr_bitwidth};
    emu_cfg.vlan_tag     = ru_cfg.vlan_tag;
    if (!parse_mac_address(ru_cfg.ru_mac_address, emu_cfg.ru_mac)) {
      report_error("Invalid MAC address provided: '{}'", ru_cfg.ru_mac_address);
    }
    if (!parse_mac_address(ru_cfg.du_mac_address, emu_cfg.du_mac)) {
      report_error("Invalid MAC address provided: '{}'", ru_cfg.du_mac_address);
    }

    ru_emulators.push_back(std::make_unique<ru_emulator>(
        logger, *workers.ru_emulators_exec[i], *transceivers[i], emu_cfg, ru_cfg.ru_ul_port_id));
  }

  // Start RU emulators.
  for (auto& ru : ru_emulators) {
    ru->start();
  }
  fmt::print("Running. Waiting for incoming packets...\n");

  while (is_running) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto    now          = std::chrono::system_clock::now();
    std::tm current_time = fmt::gmtime(std::chrono::system_clock::to_time_t(now));

    fmt::memory_buffer buffer;
    for (unsigned i = 0, e = ru_emulators.size(); i != e; ++i) {
      auto nof_rx_pacckets = ru_emulators[i]->get_statistics();
      if (nof_rx_pacckets) {
        fmt::format_to(buffer,
                       "Cell #{}: {:%F}T{:%H:%M:%S} Received {} UL C-Plane packets\n",
                       i,
                       current_time,
                       current_time,
                       nof_rx_pacckets);
      }
    }
    fmt::print(to_c_str(buffer));
  }

  for (auto& txrx : transceivers) {
    txrx->stop();
  }
  workers.stop();
  srslog::flush();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  fmt::print("\nRU emulator app stopped\n");

  return 0;
}
