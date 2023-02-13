/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/pcap/mac_pcap.h"
#include "srsgnb/adt/byte_buffer.h"
#include <linux/udp.h>
#include <netinet/in.h>
#include <sys/time.h>

namespace srsgnb {

constexpr uint16_t UDP_DLT = 149;

int nr_pcap_pack_mac_context_to_buffer(const mac_nr_context_info& context, uint8_t* buffer, unsigned int length);

mac_pcap::mac_pcap() : worker("MAC-PCAP", 1024) {}

mac_pcap::~mac_pcap()
{
  close();
}

void mac_pcap::open(const char* filename_)
{
  auto fn = [this, filename_]() { dlt_pcap_open(UDP_DLT, filename_); };
  worker.push_task_blocking(fn);
}

void mac_pcap::close()
{
  if (is_write_enabled()) {
    auto fn = [this]() { dlt_pcap_close(); };
    worker.push_task_blocking(fn);
    worker.wait_pending_tasks();
    worker.stop();
  }
}

void mac_pcap::push_pdu(mac_nr_context_info context, srsgnb::const_span<uint8_t> pdu)
{
  byte_buffer buffer{pdu};
  auto        fn = [this, context, buffer]() mutable { write_pdu(context, std::move(buffer)); };
  worker.push_task(fn);
}

void mac_pcap::push_pdu(mac_nr_context_info context, srsgnb::byte_buffer pdu)
{
  auto fn = [this, context, pdu]() mutable { write_pdu(context, std::move(pdu)); };
  worker.push_task(fn);
}

void mac_pcap::write_pdu(const mac_nr_context_info& context, srsgnb::byte_buffer buf)
{
  if (!is_write_enabled() || buf.empty()) {
    // skip
    return;
  }
  std::array<uint8_t, pcap_max_len> tmp_mem; // no init
  span<const uint8_t>               pdu = to_span(buf, tmp_mem);

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
  write_pcap_header(offset + pdu.size());
  // Write context
  write_pcap_pdu(span<uint8_t>(context_header, offset));
  // Write PDU
  write_pcap_pdu(pdu);
}

/// Helper function to serialize MAC NR context
int nr_pcap_pack_mac_context_to_buffer(const mac_nr_context_info& context, uint8_t* buffer, unsigned int length)
{
  int      offset = 0;
  uint16_t tmp16;

  if (buffer == NULL || length < PCAP_CONTEXT_HEADER_MAX) {
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
} // namespace srsgnb

