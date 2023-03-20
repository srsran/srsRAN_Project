/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dlt_pcap_impl.h"
#include "srsran/adt/byte_buffer.h"
#include <stdint.h>

namespace srsran {

constexpr uint16_t pcap_ngap_max_len = 2000;

dlt_pcap_impl::dlt_pcap_impl(unsigned dlt_, std::string layer_name) : dlt(dlt_), worker(layer_name + "-PCAP", 1024)
{
  tmp_mem.resize(pcap_ngap_max_len);
}

dlt_pcap_impl::~dlt_pcap_impl()
{
  close();
}

void dlt_pcap_impl::open(const char* filename_)
{
  auto fn = [this, filename_]() { writter.dlt_pcap_open(dlt, filename_); };
  worker.push_task_blocking(fn);
}

bool dlt_pcap_impl::is_write_enabled()
{
  return writter.is_write_enabled();
}

void dlt_pcap_impl::close()
{
  if (is_write_enabled()) {
    auto fn = [this]() { writter.dlt_pcap_close(); };
    worker.push_task_blocking(fn);
    worker.wait_pending_tasks();
    worker.stop();
  }
}

void dlt_pcap_impl::push_pdu(srsran::byte_buffer pdu)
{
  auto fn = [this, pdu]() mutable { write_pdu(std::move(pdu)); };
  worker.push_task(fn);
}

void dlt_pcap_impl::push_pdu(srsran::const_span<uint8_t> pdu)
{
  byte_buffer buffer{pdu};
  auto        fn = [this, buffer]() mutable { write_pdu(std::move(buffer)); };
  worker.push_task(fn);
}

void dlt_pcap_impl::write_pdu(srsran::byte_buffer buf)
{
  if (!is_write_enabled() || buf.empty()) {
    // skip
    return;
  }

  span<const uint8_t> pdu = to_span(buf, span<uint8_t>(tmp_mem).first(buf.length()));

  // write packet header
  writter.write_pcap_header(pdu.size_bytes());

  // write PDU payload
  writter.write_pcap_pdu(pdu);
}

} // namespace srsran
