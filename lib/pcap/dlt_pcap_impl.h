/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/pcap/dlt_pcap.h"

namespace srsran {

class dlt_pcap_impl final : public dlt_pcap
{
public:
  dlt_pcap_impl(unsigned           dlt_,
                const std::string& layer_name_,
                const std::string& filename,
                task_executor&     backend_exec_);

  ~dlt_pcap_impl() override;

  dlt_pcap_impl(const dlt_pcap_impl& other)            = delete;
  dlt_pcap_impl& operator=(const dlt_pcap_impl& other) = delete;
  dlt_pcap_impl(dlt_pcap_impl&& other)                 = delete;
  dlt_pcap_impl& operator=(dlt_pcap_impl&& other)      = delete;

  void flush() override;

  void close() override;

  bool is_write_enabled() const override { return writer.is_write_enabled(); }

  void push_pdu(byte_buffer pdu) override;

  void push_pdu(const_span<uint8_t> pdu) override;

  void handle_signal(int signal) override { close(); }

private:
  srslog::basic_logger& logger;
  backend_pcap_writer   writer;
};

} // namespace srsran
