
#ifndef SRSGNB_SSB_ENCODER_H
#define SRSGNB_SSB_ENCODER_H

#include "srsgnb/mac/mac_dl_result.h"
#include "srsgnb/mac/mac_oam_manager.h"
#include "srsgnb/mac/sched_result.h"

namespace srsgnb {

/// Encode SSB DL PDU.
/// \remark: See FAPI 3.4.2.4 - SSB PDU.
inline void encode_ssb(const mac_cell_configuration& cell_cfg, const ssb_information& ssb_info, dl_ssb_pdu& ssb_pdu)
{
  // TODO
}

} // namespace srsgnb

#endif // SRSGNB_SSB_ENCODER_H
