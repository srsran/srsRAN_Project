
#include "ssb_assembler.h"
#include "srsgnb/mac/cell_configuration.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/ran/ssb_mapping.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

using namespace srsgnb;

ssb_assembler::ssb_assembler(const mac_cell_configuration& cell_cfg) :
  pci(cell_cfg.pci),
  ssb_cfg(cell_cfg.ssb_cfg),
  ssb_case(ssb_get_ssb_pattern(cell_cfg.ssb_cfg.scs, cell_cfg.dl_carrier.arfcn)),
  L_max(ssb_get_L_max(cell_cfg.ssb_cfg.scs, cell_cfg.dl_carrier.arfcn))
{}

void ssb_assembler::assemble_ssb(dl_ssb_pdu& ssb_pdu, const ssb_information& ssb_info)
{
  ssb_pdu.pci = pci;
  // TODO: ORAN only allows 2 values, 0 or 1. FAPI allows 3 values (0, 1, or 255)
  ssb_pdu.beta_pss_profile_nr = 0;
  // TODO: because of ORAN to FAPI mismatch above, this will not be filled with meaningful values
  ssb_pdu.beta_pss_profile_sss = ssb_cfg.beta_pss;
  ssb_pdu.ssb_index            = ssb_info.ssb_index;
  // TODO: Verify whether this is the correct SCS
  ssb_pdu.scs                   = static_cast<uint8_t>(ssb_cfg.scs);
  ssb_pdu.ssb_subcarrier_offset = ssb_cfg.ssb_subcarrier_offset;
  ssb_pdu.offset_to_point_A     = ssb_cfg.ssb_offset_to_point_A;
  ssb_pdu.bch_payload_flag      = BCH_PAYLOAD_GENERATION_OPTION;
  ssb_pdu.ssb_case              = ssb_case;
  ssb_pdu.L_max                 = L_max;

  /// Fields required for PBCH payload/MIB generation.
  // TODO: Understand where these parameters should be taken (They come from RRC MIB msg)
  ssb_pdu.mib_data.cell_barred            = false;
  ssb_pdu.mib_data.intra_freq_reselection = false;
  ssb_pdu.mib_data.dmrs_typeA_position    = 0;
  ssb_pdu.mib_data.pdcch_config_sib1      = 0;
}