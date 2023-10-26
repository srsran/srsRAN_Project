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

class pcap_rlc_impl final : public pcap_rlc
{
public:
  pcap_rlc_impl();
  explicit pcap_rlc_impl(const os_sched_affinity_bitmask& mask);
  ~pcap_rlc_impl() override;
  pcap_rlc_impl(const pcap_rlc_impl& other)            = delete;
  pcap_rlc_impl& operator=(const pcap_rlc_impl& other) = delete;
  pcap_rlc_impl(pcap_rlc_impl&& other)                 = delete;
  pcap_rlc_impl& operator=(pcap_rlc_impl&& other)      = delete;

  void open(const std::string& filename_) override;
  void close() override;
  bool is_write_enabled() override;
  void push_pdu(const pcap_rlc_pdu_context& context, const byte_buffer_chain& pdu) override;
  void push_pdu(const pcap_rlc_pdu_context& context, const byte_buffer_slice& pdu) override;

private:
  void                      write_pdu(const pcap_rlc_pdu_context& context, const byte_buffer& pdu);
  std::vector<uint8_t>      tmp_mem;
  os_sched_affinity_bitmask cpu_mask;
  task_worker               worker;
  pcap_file_base            writter;
  std::atomic<bool>         is_open{false};
};
} // namespace srsran
