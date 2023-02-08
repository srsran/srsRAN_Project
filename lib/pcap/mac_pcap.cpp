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

inline int NR_PCAP_PACK_MAC_CONTEXT_TO_BUFFER(mac_nr_context_info* context, uint8_t* buffer, unsigned int length)
{
  int      offset = 0;
  uint16_t tmp16;

  if (buffer == NULL || length < PCAP_CONTEXT_HEADER_MAX) {
    printf("Error: Writing buffer null or length to small \n");
    return -1;
  }

  /*****************************************************************/
  /* Context information (same as written by UDP heuristic clients */
  buffer[offset++] = context->radioType;
  buffer[offset++] = context->direction;
  buffer[offset++] = context->rntiType;

  /* RNTI */
  buffer[offset++] = MAC_NR_RNTI_TAG;
  tmp16            = htons(context->rnti);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* UEId */
  buffer[offset++] = MAC_NR_UEID_TAG;
  tmp16            = htons(context->ueid);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* HARQID */
  buffer[offset++] = MAC_NR_HARQID;
  buffer[offset++] = context->harqid;

  /* PHR Type2 other cell */
  buffer[offset++] = MAC_NR_PHR_TYPE2_OTHERCELL_TAG;
  buffer[offset++] = context->phr_type2_othercell;

  /* Subframe Number and System Frame Number */
  /* SFN is stored in 12 MSB and SF in 4 LSB */
  buffer[offset++] = MAC_NR_FRAME_SUBFRAME_TAG;
  tmp16            = (context->system_frame_number << 4) | context->sub_frame_number;
  tmp16            = htons(tmp16);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* Data tag immediately preceding PDU */
  buffer[offset++] = MAC_NR_PAYLOAD_TAG;
  return offset;
}

mac_pcap::~mac_pcap()
{
  close();
}

void mac_pcap::open(const char* filename_)
{
  dlt_pcap_open(UDP_DLT, filename_);
}

void mac_pcap::close()
{
  dlt_pcap_close();
}

void mac_pcap::write_pdu(mac_nr_context_info& context, srsgnb::const_span<uint8_t> pdu)
{
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

  offset += NR_PCAP_PACK_MAC_CONTEXT_TO_BUFFER(&context, &context_header[offset], PCAP_CONTEXT_HEADER_MAX);

  udp_header->len = htons(offset + length);

  if (offset != 31) {
    printf("ERROR Does not match offset %d != 31\n", offset);
  }

  /****************************************************************/
  /* PCAP Header                                                  */
  write_pcap_header(offset + pdu.size());
  write_pcap_pdu(span<uint8_t>(context_header, offset));
  write_pcap_pdu(pdu);
}
} // namespace srsgnb

