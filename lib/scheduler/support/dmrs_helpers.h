/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../cell/cell_configuration.h"
#include "pdsch/pdsch_dmrs_symbol_mask.h"
#include "srsran/scheduler/scheduler_slot_handler.h"
#include "srsran/support/error_handling.h"

namespace srsran {

/// Calculates the number of RE used for DMRS per RB in PDSCH and PUSCH transmissions.
inline unsigned calculate_nof_dmrs_per_rb(const dmrs_information& dmrs)
{
  // Calculate number of RE for DMRS per RB, symbol and CDM group.
  unsigned nof_dmrs_re = get_nof_re_per_prb(dmrs.config_type);

  // Calculate the maximum number of CDM groups without data and make sure the number does not exceed the maximum.
  unsigned max_nof_cdm_groups_without_data = get_max_nof_cdm_groups_without_data(dmrs.config_type);
  srsran_assert(dmrs.num_dmrs_cdm_grps_no_data >= 1 &&
                    dmrs.num_dmrs_cdm_grps_no_data <= max_nof_cdm_groups_without_data,
                "Invalid number of CDM groups without data {}.",
                dmrs.num_dmrs_cdm_grps_no_data);

  // Determines the number of RE for DMRS per RB.
  return nof_dmrs_re * dmrs.num_dmrs_cdm_grps_no_data * dmrs.dmrs_symb_pos.count();
}

/// Helper factory of a default DM-RS Information object for DCI f1_0 or for when higher layer configuration parameters
/// are not yet available.
/// TODO: generalize to other cases (e.g. mapping type B).
/// \remark See TS 38.214, 5.1.6.2 - DM-RS reception procedure.
inline dmrs_information make_dmrs_info_common(const pdsch_config_common& pdsch_cfg,
                                              uint8_t                    time_resource,
                                              pci_t                      pci,
                                              dmrs_typeA_position        dmrs_typeA_pos)
{
  srsran_assert(pdsch_cfg.pdsch_td_alloc_list.size() > time_resource, "Invalid time resource");
  const pdsch_time_domain_resource_allocation& pdsch_time_res = pdsch_cfg.pdsch_td_alloc_list[time_resource];

  dmrs_information dmrs{};

  if (pdsch_cfg.pdsch_td_alloc_list[time_resource].map_type == sch_mapping_type::typeA) {
    // Get DM-RS symbols.
    pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration dmrscfg{};
    dmrscfg.typeA_pos                         = dmrs_typeA_pos;
    dmrscfg.additional_position               = dmrs_additional_positions::pos2;
    dmrscfg.last_symbol                       = pdsch_time_res.symbols.stop();
    dmrscfg.lte_crs_match_around              = false;
    dmrscfg.ue_capable_additional_dmrs_dl_alt = false;
    dmrs.dmrs_symb_pos                        = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(dmrscfg);

  } else {
    srsran_terminate("Mapping type B not supported");
  }

  dmrs.config_type        = dmrs_config_type::type1;
  dmrs.dmrs_scrambling_id = pci; // TODO: See TS 38.211, 7.4.1.1.1.
  dmrs.low_papr_dmrs      = false;
  dmrs.n_scid             = false;
  // When receiving PDSCH scheduled by DCI format 1_0, the UE shall assume the number of DM-RS CDM groups without data
  // is 1 which corresponds to CDM group 0 for the case of PDSCH with allocation duration of 2 symbols,
  // and the UE shall assume that the number of DM-RS CDM groups without data is 2 which corresponds to CDM group {0,1}
  // for all other cases.
  dmrs.num_dmrs_cdm_grps_no_data = pdsch_time_res.symbols.length() == 2 ? 1 : 2;
  // Port 1000.
  dmrs.dmrs_ports.resize(12);
  dmrs.dmrs_ports.set(0);

  return dmrs;
}

/// Helper factory of a default DM-RS Information object for DCI f0_0 or for when higher layer configuration parameters
/// are not yet available.
/// TODO: generalize to other cases (e.g. mapping type B).
/// \remark See TS 38.214 Section 6.2.2.
inline dmrs_information make_dmrs_info_common(const pusch_config_common& pusch_cfg,
                                              uint8_t                    time_resource,
                                              pci_t                      pci,
                                              dmrs_typeA_position        dmrs_typeA_pos)
{
  srsran_assert(pusch_cfg.pusch_td_alloc_list.size() > time_resource, "Invalid time resource");
  const pusch_time_domain_resource_allocation& pusch_time_res = pusch_cfg.pusch_td_alloc_list[time_resource];

  dmrs_information dmrs{};

  if (pusch_cfg.pusch_td_alloc_list[time_resource].map_type == sch_mapping_type::typeA) {
    // Get DM-RS symbols.
    pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration dmrscfg{};
    dmrscfg.typeA_pos                         = dmrs_typeA_pos;
    dmrscfg.additional_position               = dmrs_additional_positions::pos2;
    dmrscfg.last_symbol                       = pusch_time_res.symbols.stop();
    dmrscfg.lte_crs_match_around              = false;
    dmrscfg.ue_capable_additional_dmrs_dl_alt = false;
    dmrs.dmrs_symb_pos                        = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(dmrscfg);

  } else {
    srsran_terminate("Mapping type B not supported");
  }

  dmrs.config_type        = dmrs_config_type::type1;
  dmrs.dmrs_scrambling_id = pci; // TODO: See TS 38.211, 7.4.1.1.1.
  dmrs.low_papr_dmrs      = false;
  dmrs.n_scid             = false;
  // When receiving PDSCH scheduled by DCI format 1_0, the UE shall assume the number of DM-RS CDM groups without data
  // is 1 which corresponds to CDM group 0 for the case of PDSCH with allocation duration of 2 symbols,
  // and the UE shall assume that the number of DM-RS CDM groups without data is 2 which corresponds to CDM group {0,1}
  // for all other cases.
  dmrs.num_dmrs_cdm_grps_no_data = pusch_time_res.symbols.length() == 2 ? 1 : 2;
  // Port 1000.
  dmrs.dmrs_ports.resize(12);
  dmrs.dmrs_ports.set(0);

  return dmrs;
}

} // namespace srsran
