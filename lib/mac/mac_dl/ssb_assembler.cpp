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

ssb_assembler::ssb_assembler(const mac_cell_creation_request& cell_cfg) :
  pci(cell_cfg.pci),
  ssb_cfg(cell_cfg.ssb_cfg),
  pdcch_config_sib1((cell_cfg.sched_req.coreset0 << 4U) + cell_cfg.sched_req.searchspace0),
  dmrs_typeA_pos(cell_cfg.sched_req.dmrs_typeA_pos),
  cell_barred(cell_cfg.cell_barred),
  intra_f_resel(cell_cfg.intra_freq_resel),
  ssb_case(ssb_get_ssb_pattern(ssb_cfg.scs, cell_cfg.dl_carrier.arfcn)),
  L_max(ssb_get_L_max(ssb_cfg.scs, cell_cfg.dl_carrier.arfcn))
{
}

void ssb_assembler::assemble_ssb(dl_ssb_pdu& ssb_pdu, const ssb_information& ssb_info)
{
  ssb_pdu.pci               = pci;
  ssb_pdu.pss_to_sss_epre   = ssb_pss_to_sss_epre::dB_0;
  ssb_pdu.ssb_index         = ssb_info.ssb_index;
  ssb_pdu.scs               = ssb_cfg.scs;
  ssb_pdu.subcarrier_offset = ssb_cfg.k_ssb;
  ssb_pdu.offset_to_pointA  = ssb_cfg.offset_to_point_A;
  ssb_pdu.ssb_case          = ssb_case;
  ssb_pdu.L_max             = L_max;

  // Fields required for PBCH payload/MIB generation.
  ssb_pdu.mib_data.cell_barred            = cell_barred;
  ssb_pdu.mib_data.intra_freq_reselection = intra_f_resel;
  ssb_pdu.mib_data.dmrs_typeA_pos         = dmrs_typeA_pos;
  ssb_pdu.mib_data.pdcch_config_sib1      = pdcch_config_sib1;
}
