/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/gtpu/psup_message.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/bit_encoding.h"

namespace srsran {

/// Packing and unpacking of PDU Session Container
///
/// Ref: TS 38.415
class psup_packing
{
public:
  psup_packing(srslog::basic_logger& logger_) : logger(logger_) {}

  bool unpack(psup_dl_pdu_session_information& dl_pdu_session_information, const span<uint8_t> container)
  {
    byte_buffer buf{container};
    bit_decoder decoder{buf};

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
      if (spare != 0) {
        logger.warning("Spare bits set in third octet. value={:#x}", spare);
        // TS 38.415 Sec. 5.5.1
        // 'Spare bits should be set to "0" by the sender and should not be checked by the receiver.'
      }
    }

    if (qmp) {
      // DL Sending Time Stamp
      uint64_t dl_sending_time_stamp = 0;
      decoder.unpack(dl_sending_time_stamp, 64);
      dl_pdu_session_information.dl_sending_time_stamp = dl_sending_time_stamp;
    }

    if (snp) {
      // DL QFI Sequence Number
      uint32_t dl_qfi_sn = 0;
      decoder.unpack(dl_qfi_sn, 32);
      dl_pdu_session_information.dl_qfi_sn = dl_qfi_sn;
    }

    return true;
  }

private:
  srslog::basic_logger& logger;
};

} // namespace srsran
