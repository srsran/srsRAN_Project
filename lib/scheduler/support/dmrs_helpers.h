/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../config/cell_configuration.h"
#include "pdsch/pdsch_dmrs_symbol_mask.h"
#include "srsran/ran/dmrs.h"
#include "srsran/ran/pdsch/pdsch_antenna_ports_mapping.h"
#include "srsran/scheduler/config/dmrs.h"
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
inline dmrs_information make_dmrs_info_common(const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                              pci_t                                        pci,
                                              dmrs_typeA_position                          dmrs_typeA_pos)
{
  dmrs_information dmrs{};

  if (pdsch_td_cfg.map_type == sch_mapping_type::typeA) {
    // Get DM-RS symbols.
    pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration dmrscfg{};
    dmrscfg.typeA_pos                         = dmrs_typeA_pos;
    dmrscfg.additional_position               = dmrs_additional_positions::pos2;
    dmrscfg.last_symbol                       = pdsch_td_cfg.symbols.stop();
    dmrscfg.lte_crs_match_around              = false;
    dmrscfg.ue_capable_additional_dmrs_dl_alt = false;
    dmrs.dmrs_symb_pos                        = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(dmrscfg);

  } else {
    srsran_terminate("Mapping type B not supported");
  }

  dmrs.config_type = dmrs_config_type::type1;
  // TODO: See TS 38.211, 7.4.1.1.1.
  dmrs.dmrs_scrambling_id = pci;
  dmrs.low_papr_dmrs      = false;
  dmrs.n_scid             = false;
  // See TS 38.214, 5.1.6.2.
  // When receiving PDSCH scheduled by DCI format 1_0, the UE shall assume the number of DM-RS CDM groups without data
  // is 1 which corresponds to CDM group 0 for the case of PDSCH with allocation duration of 2 symbols,
  // and the UE shall assume that the number of DM-RS CDM groups without data is 2 which corresponds to CDM group {0,1}
  // for all other cases.
  dmrs.num_dmrs_cdm_grps_no_data = pdsch_td_cfg.symbols.length() == 2 ? 1 : 2;
  // Port 1000.
  dmrs.dmrs_ports.resize(12);
  dmrs.dmrs_ports.set(0);

  return dmrs;
}

inline dmrs_information make_dmrs_info_dedicated(const pdsch_time_domain_resource_allocation& pdsch_td_cfg,
                                                 pci_t                                        pci,
                                                 dmrs_typeA_position                          dmrs_typeA_pos,
                                                 const dmrs_downlink_config&                  dmrs_dl_cfg_ded,
                                                 unsigned                                     nof_layers,
                                                 unsigned                                     nof_dl_antenna_ports,
                                                 bool                                         are_both_cws_enabled)
{
  dmrs_information dmrs{};

  // See TS 38.211, 7.4.1.1.2.
  if (dmrs_dl_cfg_ded.additional_positions == dmrs_additional_positions::pos3 and
      dmrs_typeA_pos != dmrs_typeA_position::pos2) {
    srsran_assertion_failure("Invalid PDSCH DMRS configuration. Cause: DMRS Additional Position of pos3 is only "
                             "supported when DMRS TypeA position is equal to pos2.");
  }

  if (pdsch_td_cfg.map_type == sch_mapping_type::typeA) {
    // Get DM-RS symbols.
    pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration dmrscfg{};
    dmrscfg.typeA_pos                         = dmrs_typeA_pos;
    dmrscfg.additional_position               = dmrs_dl_cfg_ded.additional_positions;
    dmrscfg.last_symbol                       = pdsch_td_cfg.symbols.stop();
    dmrscfg.lte_crs_match_around              = false;
    dmrscfg.ue_capable_additional_dmrs_dl_alt = false;
    dmrs.dmrs_symb_pos                        = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(dmrscfg);

  } else {
    srsran_terminate("Mapping type B not supported");
  }

  dmrs.config_type = dmrs_dl_cfg_ded.is_dmrs_type2 ? dmrs_config_type::type2 : dmrs_config_type::type1;
  // TODO: See TS 38.211, 7.4.1.1.1.
  dmrs.dmrs_scrambling_id = pci;
  dmrs.low_papr_dmrs      = false;
  dmrs.n_scid             = false;
  const pdsch_antenna_ports_mapping ant_mapping =
      get_pdsch_antenna_port_mapping(nof_layers,
                                     nof_dl_antenna_ports,
                                     dmrs.config_type,
                                     dmrs_dl_cfg_ded.is_max_length_len2 ? dmrs_max_length::len2 : dmrs_max_length::len1,
                                     are_both_cws_enabled);
  // Number of DMRS CDM groups without data indicates whether PDSCH is multiplexed with DMRS or not.
  // For DMRS configuration type 1:
  //    num_dmrs_cdm_grps_no_data = 1 (multiplexing of DMRS and PDSCH).
  //    num_dmrs_cdm_grps_no_data = 2 (no multiplexing of DMRS and PDSCH).
  dmrs.num_dmrs_cdm_grps_no_data = ant_mapping.nof_dmrs_cdm_groups_without_data;
  dmrs.dmrs_ports.resize(12);
  for (const auto port : ant_mapping.dmrs_ports) {
    dmrs.dmrs_ports.set(port);
  }

  return dmrs;
}

/// Helper factory of a default DM-RS Information object for DCI f1_0 or for when higher layer configuration parameters
/// are not yet available.
/// TODO: generalize to other cases (e.g. mapping type B).
/// \remark See TS 38.214, 5.1.6.2 - DM-RS reception procedure.
inline dmrs_information make_dmrs_info_common(span<const pdsch_time_domain_resource_allocation> pdsch_td_list,
                                              uint8_t                                           time_resource,
                                              pci_t                                             pci,
                                              dmrs_typeA_position                               dmrs_typeA_pos)
{
  srsran_assert(pdsch_td_list.size() > time_resource, "Invalid time resource");
  const pdsch_time_domain_resource_allocation& pdsch_time_res = pdsch_td_list[time_resource];

  return make_dmrs_info_common(pdsch_time_res, pci, dmrs_typeA_pos);
}

/// Helper factory of a default DM-RS Information object for DCI f0_0 or for when higher layer configuration parameters
/// are not yet available.
/// TODO: generalize to other cases (e.g. mapping type B).
/// \remark See TS 38.214 Section 6.2.2.
inline dmrs_information make_dmrs_info_common(const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                              pci_t                                        pci,
                                              dmrs_typeA_position                          dmrs_typeA_pos)
{
  dmrs_information dmrs{};

  if (pusch_td_cfg.map_type == sch_mapping_type::typeA) {
    // Get DM-RS symbols.
    pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration dmrscfg{};
    dmrscfg.typeA_pos                         = dmrs_typeA_pos;
    dmrscfg.additional_position               = dmrs_additional_positions::pos2;
    dmrscfg.last_symbol                       = pusch_td_cfg.symbols.stop();
    dmrscfg.lte_crs_match_around              = false;
    dmrscfg.ue_capable_additional_dmrs_dl_alt = false;
    dmrs.dmrs_symb_pos                        = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(dmrscfg);

  } else {
    srsran_terminate("Mapping type B not supported");
  }

  dmrs.config_type = dmrs_config_type::type1;
  // TODO: See TS 38.211, 7.4.1.1.1.
  dmrs.dmrs_scrambling_id = pci;
  dmrs.low_papr_dmrs      = false;
  dmrs.n_scid             = false;
  // See TS 38.214, 6.2.2.
  // For PUSCH scheduled by DCI format 0_0 or by activation DCI format 0_0 with CRC scrambled by CS-RNTI, the UE
  // shall assume the number of DM-RS CDM groups without data is 1 which corresponds to CDM group 0 for the case of
  // PUSCH with allocation duration of 2 or less OFDM symbols with transform precoding disabled, the UE shall assume
  // that the number of DM-RS CDM groups without data is 3 which corresponds to CDM group {0,1,2} for the case of
  // PUSCH scheduled by activation DCI format 0_0 and the dmrs-Type in cg-DMRS-Configuration equal to 'type2' and the
  // PUSCH allocation duration being more than 2 OFDM symbols, and the UE shall assume that the number of DM-RS
  // CDM groups without data is 2 which corresponds to CDM group {0,1} for all other cases.
  dmrs.num_dmrs_cdm_grps_no_data = pusch_td_cfg.symbols.length() <= 2 ? 1 : 2;
  // Port 1000.
  dmrs.dmrs_ports.resize(12);
  dmrs.dmrs_ports.set(0);

  return dmrs;
}

inline dmrs_information make_dmrs_info_dedicated(const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                 pci_t                                        pci,
                                                 dmrs_typeA_position                          dmrs_typeA_pos,
                                                 const dmrs_uplink_config&                    dmrs_ul_cfg,
                                                 unsigned                                     nof_layers,
                                                 unsigned                                     nof_ul_antenna_ports,
                                                 bool                                         are_both_cws_enabled)
{
  dmrs_information dmrs{};

  // See TS 38.211, 6.4.1.1.3.
  if (dmrs_ul_cfg.additional_positions == dmrs_additional_positions::pos3 and
      dmrs_typeA_pos != dmrs_typeA_position::pos2) {
    srsran_assertion_failure("Invalid PUSCH DMRS configuration. Cause: DMRS Additional Position of pos3 is only "
                             "supported when DMRS TypeA position is equal to pos2.");
  }

  if (pusch_td_cfg.map_type == sch_mapping_type::typeA) {
    // Get DM-RS symbols.
    pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration dmrscfg{};
    dmrscfg.typeA_pos                         = dmrs_typeA_pos;
    dmrscfg.additional_position               = dmrs_ul_cfg.additional_positions;
    dmrscfg.last_symbol                       = pusch_td_cfg.symbols.stop();
    dmrscfg.lte_crs_match_around              = false;
    dmrscfg.ue_capable_additional_dmrs_dl_alt = false;
    dmrs.dmrs_symb_pos                        = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(dmrscfg);

  } else {
    srsran_terminate("Mapping type B not supported");
  }

  dmrs.config_type = dmrs_ul_cfg.is_dmrs_type2 ? dmrs_config_type::type2 : dmrs_config_type::type1;
  // TODO: See TS 38.211, 7.4.1.1.1.
  dmrs.dmrs_scrambling_id = pci;
  dmrs.low_papr_dmrs      = false;
  dmrs.n_scid             = false;
  // TODO: Derive antenna port mapping based on PUSCH-Config and other DMRS configuration parameters.
  // Since in our configuration transformPrecoder=disabled, dmrs-Type=1, maxLength=1, rank = 1, we refer to
  // Table 7.3.1.1.2-8 in TS 38.212.
  // Number of DMRS CDM groups without data indicates whether PUSCH is multiplexed with DMRS or not.
  // For DMRS configuration type 1:
  //    num_dmrs_cdm_grps_no_data = 1 (multiplexing of DMRS and PUSCH).
  //    num_dmrs_cdm_grps_no_data = 2 (no multiplexing of DMRS and PUSCH).
  dmrs.num_dmrs_cdm_grps_no_data = 2;
  // Port 1000.
  // Since we are setting num_dmrs_cdm_grps_no_data = 2 it corresponds to row 3 in Table 7.3.1.1.2-8 in TS 38.212.
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

  return make_dmrs_info_common(pusch_time_res, pci, dmrs_typeA_pos);
}

} // namespace srsran
