/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "dlt_pcap_impl.h"
#include "srsran/adt/byte_buffer.h"
#include <stdint.h>

namespace srsran {

constexpr uint16_t pcap_dlt_max_pdu_len = 9000;

dlt_pcap_impl::dlt_pcap_impl(unsigned dlt_, const std::string& layer_name_, os_sched_affinity_bitmask cpu_mask_) :
  dlt(dlt_),
  layer_name(layer_name_),
  cpu_mask(cpu_mask_),
  worker(layer_name_ + "-PCAP", 1024, os_thread_realtime_priority::no_realtime(), cpu_mask)
{
  tmp_mem.resize(pcap_dlt_max_pdu_len);
}

dlt_pcap_impl::~dlt_pcap_impl()
{
  close();
}

void dlt_pcap_impl::open(const std::string& filename_)
{
  is_open.store(true, std::memory_order_relaxed);
  // Capture filename_ by copy to prevent it goes out-of-scope when the lambda is executed later
  auto fn = [this, filename_]() { writter.dlt_pcap_open(dlt, filename_); };
  worker.push_task_blocking(fn);
}

void dlt_pcap_impl::close()
{
  bool was_open = is_open.exchange(false, std::memory_order_relaxed);
  if (was_open) {
    auto fn = [this]() { writter.dlt_pcap_close(); };
    worker.push_task_blocking(fn);
    worker.wait_pending_tasks();
    worker.stop();
  }
}

bool dlt_pcap_impl::is_write_enabled()
{
  return is_open.load(std::memory_order_relaxed);
}

void dlt_pcap_impl::push_pdu(srsran::byte_buffer pdu)
{
  auto fn = [this, pdu]() mutable { write_pdu(std::move(pdu)); };
  if (not worker.push_task(fn)) {
    srslog::fetch_basic_logger("ALL").warning("Dropped {} PCAP PDU. Worker task is full", layer_name);
  }
}

void dlt_pcap_impl::push_pdu(srsran::const_span<uint8_t> pdu)
{
  byte_buffer buffer{pdu};
  auto        fn = [this, buffer]() mutable { write_pdu(std::move(buffer)); };
  if (not worker.push_task(fn)) {
    srslog::fetch_basic_logger("ALL").warning("Dropped {} PCAP PDU. Worker task is full", layer_name);
  }
}

void dlt_pcap_impl::write_pdu(srsran::byte_buffer buf)
{
  if (!is_write_enabled() || buf.empty()) {
    // skip
    return;
  }

  if (buf.length() > pcap_dlt_max_pdu_len) {
    srslog::fetch_basic_logger("ALL").warning(
        "Dropped {} PCAP PDU. PDU is too big. pdu_len={} max_size={}", layer_name, buf.length(), pcap_dlt_max_pdu_len);
    return;
  }

  span<const uint8_t> pdu = to_span(buf, span<uint8_t>(tmp_mem).first(buf.length()));

  // write packet header
  writter.write_pcap_header(pdu.size_bytes());

  // write PDU payload
  writter.write_pcap_pdu(pdu);
}

} // namespace srsran
