/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_pcap_impl.h"
#include "srsran/adt/byte_buffer.h"
#include <stdint.h>

namespace srsran {

constexpr uint16_t NGAP_DLT = 152;

constexpr uint16_t pcap_ngap_max_len = 2000;

ngap_pcap_impl::ngap_pcap_impl() : worker("NGAP-PCAP", 1024)
{
  tmp_mem.resize(pcap_ngap_max_len);
}

ngap_pcap_impl::~ngap_pcap_impl()
{
  close();
}

void ngap_pcap_impl::open(const char* filename_)
{
  auto fn = [this, filename_]() { writter.dlt_pcap_open(NGAP_DLT, filename_); };
  worker.push_task_blocking(fn);
}

bool ngap_pcap_impl::is_write_enabled()
{
  return writter.is_write_enabled();
}

void ngap_pcap_impl::close()
{
  if (is_write_enabled()) {
    auto fn = [this]() { writter.dlt_pcap_close(); };
    worker.push_task_blocking(fn);
    worker.wait_pending_tasks();
    worker.stop();
  }
}

void ngap_pcap_impl::push_pdu(srsran::byte_buffer pdu)
{
  auto fn = [this, pdu]() mutable { write_pdu(std::move(pdu)); };
  worker.push_task(fn);
}

void ngap_pcap_impl::push_pdu(srsran::const_span<uint8_t> pdu)
{
  byte_buffer buffer{pdu};
  auto        fn = [this, buffer]() mutable { write_pdu(std::move(buffer)); };
  worker.push_task(fn);
}

void ngap_pcap_impl::write_pdu(srsran::byte_buffer buf)
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
