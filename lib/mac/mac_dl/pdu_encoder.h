
#ifndef SRSGNB_PDU_ENCODER_H
#define SRSGNB_PDU_ENCODER_H

#include "srsgnb/mac/mac_oam_manager.h"
#include "srsgnb/mac/sched_result.h"

namespace srsgnb {

inline void encode_sib_pdu(const mac_cell_configuration& cell_cfg, const sib_information& rar, byte_buffer& pdu)
{
  // TODO
}

inline void encode_rar_pdu(const mac_cell_configuration& cell_cfg, const rar_information& rar, byte_buffer& pdu)
{
  // TODO
}

} // namespace srsgnb

#endif // SRSGNB_PDU_ENCODER_H
