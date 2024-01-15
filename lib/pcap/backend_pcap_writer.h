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

#pragma once

#include "pcap_file_writer.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/unique_function.h"
#include <mutex>

namespace srsran {

class task_executor;

class pcap_pdu_data
{
public:
  pcap_pdu_data(byte_buffer payload_) : payload_buf(std::move(payload_)) {}
  pcap_pdu_data(span<const uint8_t> context_header, byte_buffer payload)
  {
    if (not header_buf.append(byte_buffer{context_header})) {
      return;
    }
    if (not payload_buf.append(std::move(payload))) {
      header_buf.clear();
      return;
    }
  }
  pcap_pdu_data(uint16_t            src,
                uint16_t            dest,
                const char*         layer_str,
                span<const uint8_t> context_header,
                byte_buffer         payload);

  const byte_buffer& header() const { return header_buf; }
  const byte_buffer& payload() const { return payload_buf; }

private:
  byte_buffer header_buf;
  byte_buffer payload_buf;
};

/// \brief This class provides a thread-safe interface to write to the pcap file, by only allowing accesses to the
/// file via a single executor.
class backend_pcap_writer
{
public:
  backend_pcap_writer(unsigned           dlt_,
                      const std::string& layer_name_,
                      const std::string& filename,
                      task_executor&     backend_exec_);
  ~backend_pcap_writer();
  backend_pcap_writer(const backend_pcap_writer& other)            = delete;
  backend_pcap_writer& operator=(const backend_pcap_writer& other) = delete;
  backend_pcap_writer(backend_pcap_writer&& other)                 = delete;
  backend_pcap_writer& operator=(backend_pcap_writer&& other)      = delete;

  void close();
  bool is_write_enabled() const { return is_open.load(std::memory_order_relaxed); }

  void write_pdu(byte_buffer pdu);

  void write_pdu(pcap_pdu_data pdu);

private:
  void write_pdu_impl(const byte_buffer& pdu);
  void write_context_pdu_impl(const pcap_pdu_data& pdu);

  std::string           layer_name;
  task_executor&        backend_exec;
  srslog::basic_logger& logger;

  pcap_file_writer  writer;
  std::atomic<bool> is_open{true};

  std::mutex close_mutex;
};

} // namespace srsran