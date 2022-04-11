
#ifndef SRSGNB_SSB_ENCODER_H
#define SRSGNB_SSB_ENCODER_H

#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/mac/mac_cell_manager.h"
#include "srsgnb/mac/sched_result.h"

#define BCH_PAYLOAD_GENERATION_OPTION 2

namespace srsgnb {

/// Encode SSB DL PDU.
/// \remark: See FAPI 3.4.2.4 - SSB PDU.
inline void assemble_ssb(dl_ssb_pdu& ssb_pdu, const mac_cell_configuration& cell_cfg, const ssb_information& ssb_info)
{
  auto& ssb_cfg = cell_cfg.ssb_cfg;
  ssb_pdu.pci   = cell_cfg.pci;
  // TODO: ORAN only allows 2 values, 0 or 1. FAPI allows 3 values (0, 1, or 255)
  ssb_pdu.beta_pss_profile_nr = ssb_cfg.beta_pss;
  // TODO: because of ORAN to FAPI mismatch above, this will not be filled with meaningful values
  ssb_pdu.beta_pss_profile_sss  = 0;
  ssb_pdu.ssb_index             = ssb_info.ssb_index;
  ssb_pdu.ssb_subcarrier_offset = ssb_cfg.ssb_subcarrier_offset;
  ssb_pdu.offset_to_point_A     = ssb_cfg.ssb_offset_to_point_A;
  ssb_pdu.bch_payload_flag      = BCH_PAYLOAD_GENERATION_OPTION;
  ssb_pdu.ssb_case              = ssb_cfg.ssb_case;
  ssb_pdu.L_max                 = 4;
}

} // namespace srsgnb

#endif // SRSGNB_SSB_ENCODER_H
