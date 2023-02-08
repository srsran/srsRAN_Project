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
#include <netinet/in.h>

namespace srsgnb {

void mac_pcap::write_context(mac_nr_context_info_t* context)
{
  uint8_t  buffer[PCAP_CONTEXT_HEADER_MAX] = {};
  int      offset                          = 0;
  uint16_t tmp16;

  /*****************************************************************/
  /* Context information (same as written by UDP heuristic clients */
  buffer[offset++] = context->radioType;
  buffer[offset++] = context->direction;
  buffer[offset++] = context->rntiType;

  /* RNTI */
  buffer[offset++] = MAC_NR_RNTI_TAG;
  tmp16            = ::htons(context->rnti);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* UEId */
  buffer[offset++] = MAC_NR_UEID_TAG;
  tmp16            = ::htons(context->ueid);
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
  write_pcap_pdu(buffer);
  return;
}

void mac_pcap::write_pdu(srsgnb::const_span<uint8_t>& pdu)
{
  mac_nr_context_info_t context = {};
  write_context(&context);
  write_pcap_pdu(pdu);
}
} // namespace srsgnb

