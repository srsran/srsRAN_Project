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

#include "backend_pcap_writer.h"
#include "pcap_file_writer.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/support/executors/task_worker.h"

namespace srsran {

class rlc_pcap_impl final : public rlc_pcap
{
public:
  rlc_pcap_impl(const std::string& filename, bool capture_srb, bool capture_drb, task_executor& backend_exec);
  ~rlc_pcap_impl() override;
  rlc_pcap_impl(const rlc_pcap_impl& other)            = delete;
  rlc_pcap_impl& operator=(const rlc_pcap_impl& other) = delete;
  rlc_pcap_impl(rlc_pcap_impl&& other)                 = delete;
  rlc_pcap_impl& operator=(rlc_pcap_impl&& other)      = delete;

  void close() override;
  bool is_write_enabled() const override { return writer.is_write_enabled(); }
  void push_pdu(const pcap_rlc_pdu_context& context, const byte_buffer_chain& pdu) override;
  void push_pdu(const pcap_rlc_pdu_context& context, const byte_buffer_slice& pdu) override;

private:
  byte_buffer pack_context(const pcap_rlc_pdu_context& context, const byte_buffer_chain& pdu) const;

  bool srb_enabled = true;
  bool drb_enabled = true;

  srslog::basic_logger& logger;

  backend_pcap_writer writer;
};
} // namespace srsran