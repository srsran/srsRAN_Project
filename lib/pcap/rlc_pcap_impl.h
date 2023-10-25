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
#include "srsran/pcap/pcap_rlc.h"
#include "srsran/support/executors/task_worker.h"

namespace srsran {

// PCAP tags
constexpr const char* RLC_NR_START_STRING    = "rlc-nr";
constexpr uint8_t     RLC_NR_PAYLOAD_TAG     = 0x01;
constexpr uint8_t     RLC_NR_DIRECTION_TAG   = 0x02;
constexpr uint8_t     RLC_NR_UEID_TAG        = 0x03;
constexpr uint8_t     RLC_NR_BEARER_TYPE_TAG = 0x04;
constexpr uint8_t     RLC_NR_BEARER_ID_TAG   = 0x05;

class rlc_pcap_impl final : public rlc_pcap
{
public:
  rlc_pcap_impl();
  explicit rlc_pcap_impl(const os_sched_affinity_bitmask& mask);
  ~rlc_pcap_impl() override;
  rlc_pcap_impl(const rlc_pcap_impl& other)            = delete;
  rlc_pcap_impl& operator=(const rlc_pcap_impl& other) = delete;
  rlc_pcap_impl(rlc_pcap_impl&& other)                 = delete;
  rlc_pcap_impl& operator=(rlc_pcap_impl&& other)      = delete;

  void open(const std::string& filename_) override;
  void close() override;
  bool is_write_enabled() override;
  void push_pdu(const rlc_nr_context_info& context, const_span<uint8_t> pdu) override;
  void push_pdu(const rlc_nr_context_info& context, const byte_buffer& pdu) override;

private:
  void                      write_pdu(const rlc_nr_context_info& context, const byte_buffer& pdu);
  std::vector<uint8_t>      tmp_mem;
  os_sched_affinity_bitmask cpu_mask;
  task_worker               worker;
  pcap_file_base            writter;
  std::atomic<bool>         is_open{false};
};
} // namespace srsran
