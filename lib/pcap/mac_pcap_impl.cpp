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

namespace srsran {

constexpr uint16_t UDP_DLT = 149;
constexpr uint16_t MAC_DLT = 157;

constexpr uint32_t pcap_mac_max_pdu_len = 131072;

int nr_pcap_pack_mac_context_to_buffer(const mac_nr_context_info& context, uint8_t* buffer, unsigned int length);

mac_pcap_impl::mac_pcap_impl(const std::string& filename, mac_pcap_type type_, task_executor& backend_exec_) :
  type(type_), writer(type == mac_pcap_type::dlt ? MAC_DLT : UDP_DLT, "MAC", filename, backend_exec_)
{
  tmp_mem.resize(pcap_mac_max_pdu_len);
}

mac_pcap_impl::~mac_pcap_impl()
{
  close();
}

void mac_pcap_impl::close()
{
  writer.close();
}

void mac_pcap_impl::push_pdu(mac_nr_context_info context, const_span<uint8_t> pdu)
{
  push_pdu(context, byte_buffer{pdu});
}

void mac_pcap_impl::push_pdu(mac_nr_context_info context, srsran::byte_buffer pdu)
{
  if (pdu.empty()) {
    // skip.
    return;
  }

  writer.dispatch([this, pdu = std::move(pdu), context](pcap_file_base& file) { write_pdu(file, context, pdu); });
}

void mac_pcap_impl::write_pdu(pcap_file_base& file, const mac_nr_context_info& context, const byte_buffer& buf)
{
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
  }

  offset += nr_pcap_pack_mac_context_to_buffer(context, &context_header[offset], PCAP_CONTEXT_HEADER_MAX);

  if (type == mac_pcap_type::udp) {
    udp_header->len = htons(offset + length);
  }

  // Write header
  file.write_pcap_header(offset + pdu.size());
  // Write context
  file.write_pcap_pdu(span<uint8_t>(context_header, offset));
  // Write PDU
  file.write_pcap_pdu(pdu);
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
