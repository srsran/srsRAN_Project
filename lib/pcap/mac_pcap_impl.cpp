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

#include "mac_pcap_impl.h"
#include "srsran/adt/byte_buffer.h"
#include <linux/udp.h>
#include <netinet/in.h>
#include <sys/time.h>

namespace srsran {

constexpr uint16_t UDP_DLT = 149;

constexpr uint16_t pcap_mac_max_pdu_len = 32768;

int nr_pcap_pack_mac_context_to_buffer(const mac_nr_context_info& context, uint8_t* buffer, unsigned int length);

mac_pcap_impl::mac_pcap_impl() : worker("MAC-PCAP", 1024)
{
  tmp_mem.resize(pcap_mac_max_pdu_len);
}

mac_pcap_impl::~mac_pcap_impl()
{
  close();
}

void mac_pcap_impl::open(const std::string& filename_)
{
  is_open = true;
  // Capture filename_ by copy to prevent it goes out-of-scope when the lambda is executed later
  auto fn = [this, filename_]() { writter.dlt_pcap_open(UDP_DLT, filename_); };
  worker.push_task_blocking(fn);
}

void mac_pcap_impl::close()
{
  bool was_open = is_open.exchange(false, std::memory_order_relaxed);
  if (was_open) {
    auto fn = [this]() { writter.dlt_pcap_close(); };
    worker.push_task_blocking(fn);
    worker.wait_pending_tasks();
    worker.stop();
  }
}

bool mac_pcap_impl::is_write_enabled()
{
  return is_open.load(std::memory_order_relaxed);
}

void mac_pcap_impl::push_pdu(mac_nr_context_info context, srsran::const_span<uint8_t> pdu)
{
  byte_buffer buffer{pdu};
  auto        fn = [this, context, buffer = std::move(buffer)]() mutable { write_pdu(context, std::move(buffer)); };
  if (not worker.push_task(fn)) {
    srslog::fetch_basic_logger("ALL").warning("Dropped MAC PCAP PDU. Cause: worker task is full");
  }
}

void mac_pcap_impl::push_pdu(mac_nr_context_info context, srsran::byte_buffer pdu)
{
  auto fn = [this, context, pdu = std::move(pdu)]() mutable { write_pdu(context, std::move(pdu)); };
  if (not worker.push_task(fn)) {
    srslog::fetch_basic_logger("ALL").warning("Dropped MAC PCAP PDU. Cause: worker task is full");
  }
}

void mac_pcap_impl::write_pdu(const mac_nr_context_info& context, srsran::byte_buffer buf)
{
  if (!is_write_enabled() || buf.empty()) {
    // skip
    return;
  }

  if (buf.length() > pcap_mac_max_pdu_len) {
    srslog::fetch_basic_logger("ALL").warning(
        "Dropped MAC PCAP PDU. PDU is too big. pdu_len={} max_size={}", buf.length(), pcap_mac_max_pdu_len);
    return;
  }

  span<const uint8_t> pdu = to_span(buf, span<uint8_t>(tmp_mem).first(buf.length()));

  uint8_t        context_header[PCAP_CONTEXT_HEADER_MAX] = {};
  const uint16_t length                                  = pdu.size();

  struct udphdr* udp_header;
  int            offset = 0;

  /* Can't write if file wasn't successfully opened */
  // TODO

  // Add dummy UDP header, start with src and dest port
  udp_header       = (struct udphdr*)context_header;
  udp_header->dest = htons(0xdead);
  offset += 2;
  udp_header->source = htons(0xbeef);
  offset += 2;
  // length to be filled later
  udp_header->len = 0x0000;
  offset += 2;
  // dummy CRC
  udp_header->check = 0x0000;
  offset += 2;

  // Start magic string
  memcpy(&context_header[offset], MAC_NR_START_STRING, strlen(MAC_NR_START_STRING));
  offset += strlen(MAC_NR_START_STRING);

  offset += nr_pcap_pack_mac_context_to_buffer(context, &context_header[offset], PCAP_CONTEXT_HEADER_MAX);

  udp_header->len = htons(offset + length);

  if (offset != 31) {
    printf("ERROR Does not match offset %d != 31\n", offset);
  }

  // Write header
  writter.write_pcap_header(offset + pdu.size());
  // Write context
  writter.write_pcap_pdu(span<uint8_t>(context_header, offset));
  // Write PDU
  writter.write_pcap_pdu(pdu);
}

/// Helper function to serialize MAC NR context
int nr_pcap_pack_mac_context_to_buffer(const mac_nr_context_info& context, uint8_t* buffer, unsigned int length)
{
  int      offset = {};
  uint16_t tmp16  = {};

  if (buffer == nullptr || length < PCAP_CONTEXT_HEADER_MAX) {
    printf("Error: Writing buffer null or length to small \n");
    return -1;
  }

  /*****************************************************************/
  /* Context information (same as written by UDP heuristic clients */
  buffer[offset++] = context.radioType;
  buffer[offset++] = context.direction;
  buffer[offset++] = context.rntiType;

  /* RNTI */
  buffer[offset++] = MAC_NR_RNTI_TAG;
  tmp16            = htons(context.rnti);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* UEId */
  buffer[offset++] = MAC_NR_UEID_TAG;
  tmp16            = htons(context.ueid);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* HARQID */
  buffer[offset++] = MAC_NR_HARQID;
  buffer[offset++] = context.harqid;

  /* PHR Type2 other cell */
  buffer[offset++] = MAC_NR_PHR_TYPE2_OTHERCELL_TAG;
  buffer[offset++] = context.phr_type2_othercell;

  /* Subframe Number and System Frame Number */
  /* SFN is stored in 12 MSB and SF in 4 LSB */
  buffer[offset++] = MAC_NR_FRAME_SUBFRAME_TAG;
  tmp16            = (context.system_frame_number << 4) | context.sub_frame_number;
  tmp16            = htons(tmp16);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* Data tag immediately preceding PDU */
  buffer[offset++] = MAC_NR_PAYLOAD_TAG;
  return offset;
}
} // namespace srsran
