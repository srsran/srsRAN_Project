/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include <netinet/in.h>

using namespace srsran;

static constexpr uint16_t UDP_DLT = 149;
static constexpr uint16_t MAC_DLT = 157;

static int nr_pcap_pack_mac_context_to_buffer(const mac_nr_context_info& context, span<uint8_t> buffer);

mac_pcap_impl::mac_pcap_impl(const std::string& filename, mac_pcap_type type_, task_executor& backend_exec_) :
  type(type_),
  logger(srslog::fetch_basic_logger("ALL")),
  writer(type == mac_pcap_type::dlt ? MAC_DLT : UDP_DLT, "MAC", filename, backend_exec_)
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
  auto pdu_buffer = byte_buffer::create(pdu);
  if (pdu_buffer.is_error()) {
    return;
  }
  push_pdu(context, std::move(pdu_buffer.value()));
}

void mac_pcap_impl::push_pdu(const mac_nr_context_info& context, byte_buffer pdu)
{
  if (!is_write_enabled() || pdu.empty()) {
    // skip.
    return;
  }

  std::array<uint8_t, PCAP_CONTEXT_HEADER_MAX> context_header = {};
  int offset = nr_pcap_pack_mac_context_to_buffer(context, span<uint8_t>(context_header));
  if (offset < 0) {
    logger.warning("Discarding MAC PCAP PDU. Cause: Failed to generate header.");
    return;
  }

  if (type == mac_pcap_type::udp) {
    pcap_pdu_data buf{
        0xbeef, 0xdead, MAC_NR_START_STRING, span<const uint8_t>(context_header).first(offset), std::move(pdu)};
    writer.write_pdu(std::move(buf));
  } else {
    pcap_pdu_data buf{span<const uint8_t>(context_header).first(offset), std::move(pdu)};
    writer.write_pdu(std::move(buf));
  }
}

/// Helper function to serialize MAC NR context.
int nr_pcap_pack_mac_context_to_buffer(const mac_nr_context_info& context, span<uint8_t> buffer)
{
  if (buffer.size() < PCAP_CONTEXT_HEADER_MAX) {
    srslog::fetch_basic_logger("ALL").error("Writing buffer length is too small");
    return -1;
  }

  int      offset = 0;
  uint16_t tmp16  = 0;

  /*****************************************************************/
  /* Context information (same as written by UDP heuristic clients */
  buffer[offset++] = context.radioType;
  buffer[offset++] = context.direction;
  buffer[offset++] = context.rntiType;

  /* RNTI */
  buffer[offset++] = MAC_NR_RNTI_TAG;
  tmp16            = htons(context.rnti);
  std::memcpy(buffer.begin() + offset, &tmp16, 2);
  offset += 2;

  /* UEId */
  buffer[offset++] = MAC_NR_UEID_TAG;
  tmp16            = htons(context.ueid);
  std::memcpy(buffer.begin() + offset, &tmp16, 2);
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
  std::memcpy(buffer.begin() + offset, &tmp16, 2);
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
