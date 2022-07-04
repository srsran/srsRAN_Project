/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ssb_assembler.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/ran/ssb_mapping.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

using namespace srsgnb;

ssb_assembler::ssb_assembler(pci_t pci_, const ssb_configuration& ssb_cfg_, unsigned dl_arfcn) :
  pci(pci_),
  ssb_cfg(ssb_cfg_),
  ssb_case(ssb_get_ssb_pattern(ssb_cfg.scs, dl_arfcn)),
  L_max(ssb_get_L_max(ssb_cfg.scs, dl_arfcn))
{
}

void ssb_assembler::assemble_ssb(dl_ssb_pdu&            ssb_pdu,
                                 const ssb_information& ssb_info,
                                 uint8_t                pdcch_config_sib1,
                                 dmrs_typeA_position    dmrs_typeA_pos,
                                 bool                   cell_barred,
                                 bool                   intra_f_resel)
{
  ssb_pdu.pci                   = pci;
  ssb_pdu.beta_pss_profile_nr   = ssb_beta_pss::dB_0;
  ssb_pdu.ssb_index             = ssb_info.ssb_index;
  ssb_pdu.scs                   = ssb_cfg.scs;
  ssb_pdu.ssb_subcarrier_offset = ssb_cfg.ssb_subcarrier_offset;
  ssb_pdu.offset_to_point_A     = ssb_cfg.ssb_offset_to_point_A;
  ssb_pdu.ssb_case              = ssb_case;
  ssb_pdu.L_max                 = L_max;

  /// Fields required for PBCH payload/MIB generation.
  // TODO: Understand where these parameters should be taken (They come from RRC MIB msg)
  ssb_pdu.mib_data.cell_barred            = cell_barred;
  ssb_pdu.mib_data.intra_freq_reselection = intra_f_resel;
  ssb_pdu.mib_data.dmrs_typeA_pos         = dmrs_typeA_pos;
  ssb_pdu.mib_data.pdcch_config_sib1      = pdcch_config_sib1;
}
