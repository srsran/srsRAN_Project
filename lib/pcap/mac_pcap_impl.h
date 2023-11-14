/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "pcap_file_base.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/pcap/pcap.h"
#include "srsran/support/executors/task_worker.h"

namespace srsran {

// PCAP tags
constexpr const char* MAC_NR_START_STRING            = "mac-nr";
constexpr uint8_t     MAC_NR_PAYLOAD_TAG             = 0x01;
constexpr uint8_t     MAC_NR_RNTI_TAG                = 0x02;
constexpr uint8_t     MAC_NR_UEID_TAG                = 0x03;
constexpr uint8_t     MAC_NR_FRAME_SUBFRAME_TAG      = 0x04;
constexpr uint8_t     MAC_NR_PHR_TYPE2_OTHERCELL_TAG = 0x05;
constexpr uint8_t     MAC_NR_HARQID                  = 0x06;

class mac_pcap_impl final : public mac_pcap
{
public:
  mac_pcap_impl();
  explicit mac_pcap_impl(const os_sched_affinity_bitmask& mask);
  ~mac_pcap_impl() override;
  mac_pcap_impl(const mac_pcap_impl& other)            = delete;
  mac_pcap_impl& operator=(const mac_pcap_impl& other) = delete;
  mac_pcap_impl(mac_pcap_impl&& other)                 = delete;
  mac_pcap_impl& operator=(mac_pcap_impl&& other)      = delete;

  void open(const std::string& filename_, mac_pcap_type type) override;
  void close() override;
  bool is_write_enabled() override;
  void push_pdu(mac_nr_context_info context, const_span<uint8_t> pdu) override;
  void push_pdu(mac_nr_context_info context, byte_buffer pdu) override;

private:
  void                         write_pdu(const mac_nr_context_info& context, byte_buffer pdu);
  mac_pcap_type                type;
  std::vector<uint8_t>         tmp_mem;
  os_sched_affinity_bitmask    cpu_mask;
  std::unique_ptr<task_worker> worker = nullptr;
  pcap_file_base               writter;
  std::atomic<bool>            is_open{false};
};
} // namespace srsran
