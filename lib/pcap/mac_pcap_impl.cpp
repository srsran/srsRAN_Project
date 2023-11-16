/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_pcap_impl.h"
#include <linux/udp.h>
#include <netinet/in.h>

using namespace srsran;

constexpr uint16_t UDP_DLT = 149;
constexpr uint16_t MAC_DLT = 157;

int nr_pcap_pack_mac_context_to_buffer(const mac_nr_context_info& context, uint8_t* buffer, unsigned int length);

mac_pcap_impl::mac_pcap_impl(const std::string& filename, mac_pcap_type type_, task_executor& backend_exec_) :
  type(type_), writer(type == mac_pcap_type::dlt ? MAC_DLT : UDP_DLT, "MAC", filename, backend_exec_)
{
}

mac_pcap_impl::~mac_pcap_impl()
{
  close();
}

void mac_pcap_impl::close()
{
  writer.close();
}

void mac_pcap_impl::push_pdu(const mac_nr_context_info& context, const_span<uint8_t> pdu)
{
  push_pdu(context, byte_buffer{pdu});
}

void mac_pcap_impl::push_pdu(const mac_nr_context_info& context, byte_buffer pdu)
{
  if (!is_write_enabled() || pdu.empty()) {
    // skip.
    return;
  }

  byte_buffer buffer = pack_context(context, pdu);
  if (buffer.empty() or not buffer.append(std::move(pdu))) {
    srslog::fetch_basic_logger("ALL").error("Failed to generate MAC PCAP PDU");
    return;
  }
  writer.write_pdu(std::move(buffer));
}

byte_buffer mac_pcap_impl::pack_context(const mac_nr_context_info& context, const byte_buffer& pdu) const
{
  const size_t buf_len = pdu.length();

  uint8_t context_header[PCAP_CONTEXT_HEADER_MAX] = {};

  struct udphdr* udp_header;
  int            offset = 0;

  if (type == mac_pcap_type::udp) {
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

    // pack MAC context.
    offset += nr_pcap_pack_mac_context_to_buffer(context, &context_header[offset], PCAP_CONTEXT_HEADER_MAX);

    // Set length in UDP header.
    srsran_assert(offset + buf_len < std::numeric_limits<uint16_t>::max(), "PDU length is too large");
    udp_header->len = htons(offset + buf_len);
  } else {
    // pack MAC context.
    offset += nr_pcap_pack_mac_context_to_buffer(context, &context_header[offset], PCAP_CONTEXT_HEADER_MAX);
  }

  return byte_buffer{span<uint8_t>(context_header, offset)};
}

/// Helper function to serialize MAC NR context
int nr_pcap_pack_mac_context_to_buffer(const mac_nr_context_info& context, uint8_t* buffer, unsigned int length)
{
  int      offset = 0;
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

std::unique_ptr<mac_pcap>
srsran::create_mac_pcap(const std::string& filename, mac_pcap_type pcap_type, task_executor& backend_exec)
{
  srsran_assert(not filename.empty(), "File name is empty");
  return std::make_unique<mac_pcap_impl>(filename, pcap_type, backend_exec);
}

std::unique_ptr<mac_pcap> srsran::create_null_mac_pcap()
{
  return std::make_unique<null_mac_pcap>();
}
