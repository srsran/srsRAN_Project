/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/pcap/ngap_pcap.h"
#include "srsgnb/adt/byte_buffer.h"
#include <stdint.h>

namespace srsgnb {

constexpr uint16_t NGAP_DLT = 152;

ngap_pcap::ngap_pcap() : worker("NGAP-PCAP", 1024) {}

ngap_pcap::~ngap_pcap()
{
  close();
}

void ngap_pcap::open(const char* filename_)
{
  auto fn = [this, filename_]() { dlt_pcap_open(NGAP_DLT, filename_); };
  worker.push_task_blocking(fn);
}

void ngap_pcap::close()
{
  if (is_write_enabled()) {
    auto fn = [this]() { dlt_pcap_close(); };
    worker.push_task_blocking(fn);
    worker.wait_pending_tasks();
    worker.stop();
  }
}

void ngap_pcap::push_pdu(srsgnb::byte_buffer pdu)
{
  auto fn = [this, pdu]() mutable { write_pdu(std::move(pdu)); };
  worker.push_task(fn);
}

void ngap_pcap::push_pdu(srsgnb::const_span<uint8_t> pdu)
{
  byte_buffer buffer{pdu};
  auto        fn = [this, buffer]() mutable { write_pdu(std::move(buffer)); };
  worker.push_task(fn);
}

void ngap_pcap::write_pdu(srsgnb::byte_buffer buf)
{
  if (!is_write_enabled() || buf.empty()) {
    // skip
    return;
  }

  std::array<uint8_t, pcap_max_len> tmp_mem; // no init
  span<const uint8_t>               pdu = to_span(buf, tmp_mem);

  // write packet header
  write_pcap_header(pdu.size_bytes());

  // write PDU payload
  write_pcap_pdu(pdu);
}

} // namespace srsgnb
