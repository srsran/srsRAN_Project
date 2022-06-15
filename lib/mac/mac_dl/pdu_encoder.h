/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PDU_ENCODER_H
#define SRSGNB_PDU_ENCODER_H

#include "srsgnb/mac/mac_cell_manager.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

inline void encode_sib_pdu(const mac_cell_creation_request& cell_cfg, byte_buffer& pdu)
{
  pdu = cell_cfg.sib1_payload.copy();
}

inline void encode_rar_pdu(const mac_cell_creation_request& cell_cfg, const rar_information& rar, byte_buffer& pdu)
{
  // TODO
}

} // namespace srsgnb

#endif // SRSGNB_PDU_ENCODER_H
