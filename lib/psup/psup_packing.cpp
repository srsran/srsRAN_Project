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

#include "srsran/psup/psup_packing.h"
#include "srsran/support/bit_encoding.h"

namespace srsran {

bool psup_packing::unpack(psup_dl_pdu_session_information& dl_pdu_session_information, byte_buffer_view container) const
{
  bit_decoder decoder{container};

  // PDU Type
  uint8_t pdu_type = 0;
  decoder.unpack(pdu_type, 4);
  if (uint_to_psup_pdu_type(pdu_type) != dl_pdu_session_information.pdu_type) {
    logger.error("Invalid PDU type. unpacked={} expected={}",
                 uint_to_psup_pdu_type(pdu_type),
                 dl_pdu_session_information.pdu_type);
    return false;
  }

  // QMP
  bool qmp = false;
  decoder.unpack(qmp, 1);

  // SNP
  bool snp = false;
  decoder.unpack(snp, 1);

  // Spare
  uint8_t spare = 0;
  decoder.unpack(spare, 2);
  if (spare != 0) {
    logger.warning("Spare bits set in first octet. value={:#x}", spare);
    // TS 38.415 Sec. 5.5.1
    // 'Spare bits should be set to "0" by the sender and should not be checked by the receiver.'
  }

  // PPP
  bool ppp = false;
  decoder.unpack(ppp, 1);

  // RQI
  decoder.unpack(dl_pdu_session_information.rqi, 1);

  // QoS Flow Identifier
  uint8_t qos_flow_id = 0;
  decoder.unpack(qos_flow_id, 6);
  dl_pdu_session_information.qos_flow_id = uint_to_qos_flow_id(qos_flow_id);

  if (ppp) {
    // PPI
    uint8_t ppi = 0;
    decoder.unpack(ppi, 3);
    dl_pdu_session_information.ppi = uint_to_psup_ppi(ppi);

    // Spare
    spare = 0;
    decoder.unpack(spare, 5);
    if (spare != 0) {
      logger.warning("Spare bits set in third octet. value={:#x}", spare);
      // TS 38.415 Sec. 5.5.1
      // 'Spare bits should be set to "0" by the sender and should not be checked by the receiver.'
    }
  }

  if (qmp) {
    // DL Sending Time Stamp
    dl_pdu_session_information.dl_sending_time_stamp = 0;
    decoder.unpack(dl_pdu_session_information.dl_sending_time_stamp.value(), 64);
  }

  if (snp) {
    // DL QFI Sequence Number
    dl_pdu_session_information.dl_qfi_sn = 0;
    decoder.unpack(dl_pdu_session_information.dl_qfi_sn.value(), 24);
  }

  return true;
}

bool psup_packing::pack(byte_buffer& out_buf, const psup_dl_pdu_session_information& dl_pdu_session_information) const
{
  size_t      start_len = out_buf.length();
  bit_encoder encoder{out_buf};

  // PDU Type
  encoder.pack(psup_pdu_type_to_uint(dl_pdu_session_information.pdu_type), 4);

  // QMP
  encoder.pack(dl_pdu_session_information.dl_sending_time_stamp.has_value(), 1);

  // SNP
  encoder.pack(dl_pdu_session_information.dl_qfi_sn.has_value(), 1);

  // Spare
  encoder.pack(0, 2);

  // PPP
  encoder.pack(dl_pdu_session_information.ppi.has_value(), 1);

  // RQI
  encoder.pack(dl_pdu_session_information.rqi, 1);

  // QoS Flow Identifier
  encoder.pack(qos_flow_id_to_uint(dl_pdu_session_information.qos_flow_id), 6);

  if (dl_pdu_session_information.ppi.has_value()) {
    // PPI
    encoder.pack(psup_ppi_to_uint(dl_pdu_session_information.ppi.value()), 3);
    // Spare
    encoder.pack(0, 5);
  }

  if (dl_pdu_session_information.dl_sending_time_stamp.has_value()) {
    // DL Sending Time Stamp
    encoder.pack(dl_pdu_session_information.dl_sending_time_stamp.value(), 64);
  }

  if (dl_pdu_session_information.dl_qfi_sn.has_value()) {
    // DL QFI Sequence Number
    encoder.pack(dl_pdu_session_information.dl_qfi_sn.value(), 24);
  }

  // Add padding such that length is (n*4-2) octets, where n is a positive integer.
  while (((out_buf.length() - start_len) + 2) % 4) {
    if (not out_buf.append(0x0)) {
      return false;
    }
  }
  return true;
}

} // namespace srsran
