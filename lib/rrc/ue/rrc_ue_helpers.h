/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rrc_ue_logger.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_cu_cp {

// Helper to create PDU from RRC message.
template <class T>
byte_buffer pack_into_pdu(const T& msg, const char* context_name = nullptr)
{
  context_name = context_name == nullptr ? __FUNCTION__ : context_name;
  byte_buffer   pdu{};
  asn1::bit_ref bref{pdu};
  if (msg.pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    srslog::fetch_basic_logger("RRC").error("Failed to pack message in {}. Discarding it.", context_name);
  }
  return pdu;
}

// Logging.
typedef enum { Rx = 0, Tx } direction_t;

template <class T>
void log_rrc_message(rrc_ue_logger&    logger,
                     const direction_t dir,
                     byte_buffer_view  pdu,
                     const T&          msg,
                     srb_id_t          srb_id,
                     const char*       msg_type);

} // namespace srs_cu_cp
} // namespace srsran
