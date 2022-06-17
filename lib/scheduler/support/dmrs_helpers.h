/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DMRS_HELPERS_H
#define SRSGNB_DMRS_HELPERS_H

#include "../cell/cell_configuration.h"
#include "pdsch/pdsch_dmrs_symbol_mask.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

/// Helper factory of a default DM-RS Information object for DCI f1_0.
inline dmrs_information make_default_f1_0_dmrs_info(const pdsch_config_common& pdsch_cfg,
                                                    pci_t                      pci,
                                                    dmrs_typeA_position        dmrs_typeA_pos,
                                                    unsigned                   nof_symbols)
{
  dmrs_information dmrs{};
  // Get DM-RS symbols.
  pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration dmrscfg{};
  dmrscfg.typeA_pos                         = dmrs_typeA_pos;
  dmrscfg.additional_position               = pdsch_cfg.dmrs_dl_pdsch_typeA->additional_positions;
  dmrscfg.duration                          = nof_symbols;
  dmrscfg.lte_crs_match_around              = false;
  dmrscfg.ue_capable_additional_dmrs_dl_alt = false;

  dmrs.config_type               = pdsch_cfg.dmrs_dl_pdsch_typeA->type;
  dmrs.dmrs_symb_pos             = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(dmrscfg);
  dmrs.dmrs_scrambling_id        = pci; // TODO: See TS 38.211, 7.4.1.1.1.
  dmrs.low_papr_dmrs             = false;
  dmrs.n_scid                    = false;
  dmrs.num_dmrs_cdm_grps_no_data = 2;
  dmrs.dmrs_ports.resize(12);
  dmrs.dmrs_ports.set(0);

  return dmrs;
}

} // namespace srsgnb

#endif // SRSGNB_DMRS_HELPERS_H
