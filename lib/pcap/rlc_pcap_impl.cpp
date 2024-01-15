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

#include "rlc_pcap_impl.h"
#include <linux/udp.h>
#include <netinet/in.h>
#include <sys/time.h>

using namespace srsran;

// PCAP tags as defined in Wireshark's "packet-rlc-nr.h"
constexpr const char* PCAP_RLC_NR_START_STRING    = "rlc-nr";
constexpr uint8_t     PCAP_RLC_NR_PAYLOAD_TAG     = 0x01;
constexpr uint8_t     PCAP_RLC_NR_DIRECTION_TAG   = 0x02;
constexpr uint8_t     PCAP_RLC_NR_UEID_TAG        = 0x03;
constexpr uint8_t     PCAP_RLC_NR_BEARER_TYPE_TAG = 0x04;
constexpr uint8_t     PCAP_RLC_NR_BEARER_ID_TAG   = 0x05;

// Other constants
constexpr uint16_t UDP_DLT = 149;

int nr_pcap_pack_rlc_context_to_buffer(const pcap_rlc_pdu_context& context, uint8_t* buffer, unsigned int length);

rlc_pcap_impl::rlc_pcap_impl(const std::string& filename,
                             bool               capture_srb,
                             bool               capture_drb,
                             task_executor&     backend_exec) :
  srb_enabled(capture_srb),
  drb_enabled(capture_drb),
  logger(srslog::fetch_basic_logger("ALL")),
  writer(UDP_DLT, "RLC", filename, backend_exec)
{
}

rlc_pcap_impl::~rlc_pcap_impl()
{
  close();
}

void rlc_pcap_impl::close()
{
  writer.close();
}

void rlc_pcap_impl::push_pdu(const pcap_rlc_pdu_context& context, const span<uint8_t> pdu)
{
  if (!is_write_enabled() || pdu.empty()) {
    return;
  }

  // Filter DRBs if disabled
  if (!drb_enabled && context.bearer_type == PCAP_RLC_BEARER_TYPE_DRB) {
    return;
  }

  // Filter SRBs if disabled
  if (!srb_enabled &&
      (context.bearer_type == PCAP_RLC_BEARER_TYPE_SRB || context.bearer_type == PCAP_RLC_BEARER_TYPE_CCCH)) {
    return;
  }

  // Encode RLC header.
  uint8_t context_header[PCAP_CONTEXT_HEADER_MAX] = {};
  int     offset = nr_pcap_pack_rlc_context_to_buffer(context, &context_header[0], PCAP_CONTEXT_HEADER_MAX);
  if (offset < 0) {
    logger.warning("Discarding RLC PCAP PDU. Cause: Failed to generate header.");
    return;
  }

  // Copy byte buffer.
  byte_buffer buf;
  if (not buf.append(pdu)) {
    logger.warning("Discarding RLC PCAP PDU. Cause: Failed to allocate memory for PCAP PDU.");
    return;
  }

  writer.write_pdu(pcap_pdu_data{
      0xbeef, 0xdead, PCAP_RLC_NR_START_STRING, span<const uint8_t>(context_header, offset), std::move(buf)});
}

void rlc_pcap_impl::push_pdu(const pcap_rlc_pdu_context& context, const byte_buffer_slice& pdu)
{
  if (!is_write_enabled() || pdu.empty()) {
    return;
  }

  // Filter DRBs if disabled
  if (!drb_enabled && context.bearer_type == PCAP_RLC_BEARER_TYPE_DRB) {
    return;
  }

  // Filter SRBs if disabled
  if (!srb_enabled &&
      (context.bearer_type == PCAP_RLC_BEARER_TYPE_SRB || context.bearer_type == PCAP_RLC_BEARER_TYPE_CCCH)) {
    return;
  }

  // Encode RLC header.
  uint8_t context_header[PCAP_CONTEXT_HEADER_MAX] = {};
  int     offset = nr_pcap_pack_rlc_context_to_buffer(context, &context_header[0], PCAP_CONTEXT_HEADER_MAX);
  if (offset < 0) {
    logger.warning("Discarding RLC PCAP PDU. Cause: Failed to generate header.");
    return;
  }

  // Copy byte buffer.
  byte_buffer buf;
  if (not buf.append(pdu)) {
    logger.warning("Discarding RLC PCAP PDU. Cause: Failed to allocate memory for PCAP PDU.");
    return;
  }

  writer.write_pdu(pcap_pdu_data{
      0xbeef, 0xdead, PCAP_RLC_NR_START_STRING, span<const uint8_t>(context_header, offset), std::move(buf)});
}

/// Helper function to serialize RLC NR context
int nr_pcap_pack_rlc_context_to_buffer(const pcap_rlc_pdu_context& context, uint8_t* buffer, unsigned int length)
{
  int      offset = {};
  uint16_t tmp16  = {};

  if (buffer == nullptr || length < PCAP_CONTEXT_HEADER_MAX) {
    srslog::fetch_basic_logger("ALL").error("Writing buffer null or length to small\n");
    return -1;
  }

  /*****************************************************************/
  /* Context information (same as written by UDP heuristic clients */
  buffer[offset++] = context.rlc_mode;
  buffer[offset++] = context.sequence_number_length;

  /* Direction */
  buffer[offset++] = PCAP_RLC_NR_DIRECTION_TAG;
  buffer[offset++] = context.direction;

  /* UEID */
  buffer[offset++] = PCAP_RLC_NR_UEID_TAG;
  tmp16            = htons(context.ueid);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* Bearer type */
  buffer[offset++] = PCAP_RLC_NR_BEARER_TYPE_TAG;
  buffer[offset++] = context.bearer_type;

  /* Bearer ID */
  buffer[offset++] = PCAP_RLC_NR_BEARER_ID_TAG;
  buffer[offset++] = context.bearer_id;

  /* Data tag immediately preceding PDU */
  buffer[offset++] = PCAP_RLC_NR_PAYLOAD_TAG;
  return offset;
}

std::unique_ptr<rlc_pcap> srsran::create_rlc_pcap(const std::string& filename,
                                                  task_executor&     backend_exec,
                                                  bool               srb_pdus_enabled,
                                                  bool               drb_pdus_enabled)
{
  srsran_assert(not filename.empty(), "File name is empty");
  return std::make_unique<rlc_pcap_impl>(filename, srb_pdus_enabled, drb_pdus_enabled, backend_exec);
}

std::unique_ptr<rlc_pcap> srsran::create_null_rlc_pcap()
{
  return std::make_unique<null_rlc_pcap>();
}
