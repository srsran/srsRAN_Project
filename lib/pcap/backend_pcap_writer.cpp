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

#include "backend_pcap_writer.h"
#include "srsran/support/executors/sync_task_executor.h"
#include <linux/udp.h>
#include <netinet/in.h>
#include <thread>

using namespace srsran;

pcap_pdu_data::pcap_pdu_data(uint16_t            src,
                             uint16_t            dest,
                             const char*         layer_str,
                             span<const uint8_t> context_header,
                             byte_buffer         payload_)
{
  const size_t layer_str_len = strlen(layer_str);

  // Add dummy UDP header, start with src and dest port
  udphdr udp_header;
  udp_header.source = htons(src);
  udp_header.dest   = htons(dest);
  // length
  unsigned length = sizeof(udphdr) + layer_str_len + context_header.size() + payload_.length();
  srsran_assert(length < std::numeric_limits<uint16_t>::max(), "PDU length is too large");
  udp_header.len = htons(length);
  // dummy CRC
  udp_header.check = 0x0;

  if (not header_buf.append(span<const uint8_t>{(const uint8_t*)&udp_header, sizeof(udphdr)}) or
      not header_buf.append(span<const uint8_t>{(const uint8_t*)layer_str, layer_str_len}) or
      not header_buf.append(context_header)) {
    header_buf.clear();
    return;
  }
  payload_buf = std::move(payload_);
}

backend_pcap_writer::backend_pcap_writer(uint32_t           dlt,
                                         const std::string& layer_name_,
                                         const std::string& filename,
                                         task_executor&     backend_exec_) :
  layer_name(layer_name_), backend_exec(backend_exec_), logger(srslog::fetch_basic_logger("ALL"))
{
  writer.open(dlt, filename);
}

backend_pcap_writer::~backend_pcap_writer()
{
  close();
}

void backend_pcap_writer::close()
{
  bool prev_value = is_open.exchange(false, std::memory_order_relaxed);
  if (not prev_value) {
    // It was already closed.
    return;
  }

  logger.debug("Scheduling the closing of the \"{}\" pcap writer", layer_name);

  // The pcap writing is still enabled. Dispatch closing of the pcap writer to backend executor.
  // Note: We block waiting until the pcap finishes closing.
  force_blocking_execute(
      backend_exec,
      [this]() { writer.close(); },
      []() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
}

void backend_pcap_writer::write_pdu(byte_buffer pdu)
{
  if (pdu.empty()) {
    return;
  }
  if (not is_write_enabled()) {
    logger.warning("Dropped {} PCAP PDU. Cause: The pcap file is closed", layer_name);
    return;
  }
  if (not backend_exec.defer([this, pdu = std::move(pdu)]() { write_pdu_impl(pdu); })) {
    logger.warning("Dropped {} PCAP PDU. Cause: Task executor queue is full", layer_name);
  }
}

void backend_pcap_writer::write_pdu(pcap_pdu_data pdu)
{
  if (pdu.payload().empty()) {
    return;
  }
  if (not is_write_enabled()) {
    logger.warning("Dropped {} PCAP PDU. Cause: The pcap file is closed", layer_name);
    return;
  }
  if (not backend_exec.defer([this, pdu = std::move(pdu)]() { write_context_pdu_impl(pdu); })) {
    logger.warning("Dropped {} PCAP PDU. Cause: Task executor queue is full", layer_name);
  }
}

void backend_pcap_writer::write_pdu_impl(const byte_buffer& pdu)
{
  if (not is_write_enabled()) {
    logger.warning("Dropped {} PCAP PDU. Cause: The pcap file is closed", layer_name);
    return;
  }

  // write packet header
  unsigned length = pdu.length();
  writer.write_pdu_header(length);

  // write PDU payload
  writer.write_pdu(pdu);
}

void backend_pcap_writer::write_context_pdu_impl(const pcap_pdu_data& pdu)
{
  if (not is_write_enabled()) {
    logger.warning("Dropped {} PCAP PDU. Cause: The pcap file is closed", layer_name);
    return;
  }

  // write packet header
  writer.write_pdu_header(pdu.header().length() + pdu.payload().length());

  // write PDU header
  writer.write_pdu(pdu.header());

  // write PDU payload
  writer.write_pdu(pdu.payload());
}
