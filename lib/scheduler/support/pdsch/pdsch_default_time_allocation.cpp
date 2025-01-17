/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "pdsch_default_time_allocation.h"
#include "srsran/scheduler/config/serving_cell_config.h"

using namespace srsran;

// Helper to construct ofdm symbol range
static constexpr ofdm_symbol_range s_and_len(unsigned s, unsigned l)
{
  return ofdm_symbol_range::start_and_len(s, l);
}

/// Reserved default PDSCH time-domain allocation. It indicates the configuration is invalid.
static constexpr pdsch_time_domain_resource_allocation PDSCH_DEFAULT_TIME_ALLOCATION_RESERVED = {};

span<const pdsch_time_domain_resource_allocation>
srsran::pdsch_default_time_allocations_default_A_table(cyclic_prefix cp, dmrs_typeA_position dmrs_pos)
{
  // TS38.214 Table 5.1.2.1.1-2. Default PDSCH time domain resource allocation A for normal CP and DMRS pos2.
  static constexpr std::array<pdsch_time_domain_resource_allocation, 16> table_normal_cp_dmrs2 = {
      {{0, sch_mapping_type::typeA, s_and_len(2, 12)},
       {0, sch_mapping_type::typeA, s_and_len(2, 10)},
       {0, sch_mapping_type::typeA, s_and_len(2, 9)},
       {0, sch_mapping_type::typeA, s_and_len(2, 7)},
       {0, sch_mapping_type::typeA, s_and_len(2, 5)},
       {0, sch_mapping_type::typeB, s_and_len(9, 4)},
       {0, sch_mapping_type::typeB, s_and_len(4, 4)},
       {0, sch_mapping_type::typeB, s_and_len(5, 7)},
       {0, sch_mapping_type::typeB, s_and_len(5, 2)},
       {0, sch_mapping_type::typeB, s_and_len(9, 2)},
       {0, sch_mapping_type::typeB, s_and_len(12, 2)},
       {0, sch_mapping_type::typeA, s_and_len(1, 13)},
       {0, sch_mapping_type::typeA, s_and_len(1, 6)},
       {0, sch_mapping_type::typeA, s_and_len(2, 4)},
       {0, sch_mapping_type::typeB, s_and_len(4, 7)},
       {0, sch_mapping_type::typeB, s_and_len(8, 4)}}};

  // TS38.214 Table 5.1.2.1.1-2. Default PDSCH time domain resource allocation A for normal CP and DMRS pos3.
  static constexpr std::array<pdsch_time_domain_resource_allocation, 16> table_normal_cp_dmrs3 = []() {
    std::array<pdsch_time_domain_resource_allocation, 16> table = table_normal_cp_dmrs2;
    for (unsigned row_index = 0; row_index <= 6; ++row_index) {
      if (row_index < 5) {
        table[row_index].symbols = {table[row_index].symbols.start() + 1, table[row_index].symbols.stop()};
      } else if (row_index == 5) {
        table[row_index].symbols.displace_by(1);
      } else {
        // row_index == 6.
        table[row_index].symbols.displace_by(2);
      }
    }
    return table;
  }();

  // TS38.214 Table 5.1.2.1.1-2. Default PDSCH time domain resource allocation A for extended CP and DMRS pos2.
  static constexpr std::array<pdsch_time_domain_resource_allocation, 16> table_extended_cp_dmrs2 = {
      {{0, sch_mapping_type::typeA, s_and_len(2, 6)},
       {0, sch_mapping_type::typeA, s_and_len(2, 10)},
       {0, sch_mapping_type::typeA, s_and_len(2, 9)},
       {0, sch_mapping_type::typeA, s_and_len(2, 7)},
       {0, sch_mapping_type::typeA, s_and_len(2, 5)},
       {0, sch_mapping_type::typeB, s_and_len(6, 4)},
       {0, sch_mapping_type::typeB, s_and_len(4, 4)},
       {0, sch_mapping_type::typeB, s_and_len(5, 6)},
       {0, sch_mapping_type::typeB, s_and_len(5, 2)},
       {0, sch_mapping_type::typeB, s_and_len(9, 2)},
       {0, sch_mapping_type::typeB, s_and_len(10, 2)},
       {0, sch_mapping_type::typeA, s_and_len(1, 11)},
       {0, sch_mapping_type::typeA, s_and_len(1, 6)},
       {0, sch_mapping_type::typeA, s_and_len(2, 4)},
       {0, sch_mapping_type::typeB, s_and_len(4, 6)},
       {0, sch_mapping_type::typeB, s_and_len(8, 4)}}};

  // TS38.214 Table 5.1.2.1.1-2. Default PDSCH time domain resource allocation A for extended CP and DMRS pos3.
  static constexpr std::array<pdsch_time_domain_resource_allocation, 16> table_extended_cp_dmrs3 = []() {
    std::array<pdsch_time_domain_resource_allocation, 16> table = table_extended_cp_dmrs2;
    for (unsigned row_index = 0; row_index <= 6; ++row_index) {
      if (row_index < 5) {
        table[row_index].symbols = {table[row_index].symbols.start() + 1, table[row_index].symbols.stop()};
      } else if (row_index == 5) {
        table[row_index].symbols = {table[row_index].symbols.start() + 2, table[row_index].symbols.stop()};
      } else {
        // row_index == 6.
        table[row_index].symbols.displace_by(2);
      }
    }
    return table;
  }();

  if (cp == cyclic_prefix::NORMAL) {
    return dmrs_pos == dmrs_typeA_position::pos2 ? table_normal_cp_dmrs2 : table_normal_cp_dmrs3;
  }
  return dmrs_pos == dmrs_typeA_position::pos2 ? table_extended_cp_dmrs2 : table_extended_cp_dmrs3;
}

pdsch_time_domain_resource_allocation
srsran::pdsch_default_time_allocation_default_A_get(cyclic_prefix cp, unsigned row_index, dmrs_typeA_position dmrs_pos)
{
  span<const pdsch_time_domain_resource_allocation> table =
      pdsch_default_time_allocations_default_A_table(cp, dmrs_pos);
  return row_index >= table.size() ? PDSCH_DEFAULT_TIME_ALLOCATION_RESERVED : table[row_index];
}

span<const pdsch_time_domain_resource_allocation>
srsran::get_c_rnti_pdsch_time_domain_list(const search_space_configuration& ss_cfg,
                                          const bwp_downlink_common&        active_bwp_dl_common,
                                          const bwp_downlink_dedicated*     active_bwp_dl_ded,
                                          dmrs_typeA_position               dmrs_typeA_pos)
{
  const bool is_fallback_ss = ss_cfg.is_common_search_space() and ss_cfg.get_coreset_id() == to_coreset_id(0);
  srsran_assert(is_fallback_ss or active_bwp_dl_ded != nullptr, "Invalid BWP DL dedicated configuration");

  // See TS 38.214, Table 5.1.2.1.1-1: Applicable PDSCH time domain resource allocation for DCI formats 1_0 and 1_1.
  if (not is_fallback_ss) {
    if (not active_bwp_dl_ded->pdsch_cfg->pdsch_td_alloc_list.empty()) {
      // UE dedicated pdsch-TimeDomain list.
      return active_bwp_dl_ded->pdsch_cfg->pdsch_td_alloc_list;
    }
  }

  if (not active_bwp_dl_common.pdsch_common.pdsch_td_alloc_list.empty()) {
    // common pdsch-TimeDomain list.
    return active_bwp_dl_common.pdsch_common.pdsch_td_alloc_list;
  }

  // default A table case.
  return pdsch_default_time_allocations_default_A_table(active_bwp_dl_common.generic_params.cp, dmrs_typeA_pos);
}

span<const pdsch_time_domain_resource_allocation>
srsran::get_si_rnti_pdsch_time_domain_list(cyclic_prefix cp, dmrs_typeA_position dmrs_typeA_pos)
{
  // TODO: Check for multiplexing pattern and SearchSpace type (Type0 common).
  // NOTE: Type0 common PDCCH and multiplexing pattern 1 is assumed to fetch applicable PDSCH time domain resource
  // allocation.
  return pdsch_default_time_allocations_default_A_table(cp, dmrs_typeA_pos);
}

span<const pdsch_time_domain_resource_allocation>
srsran::get_si_rnti_type0A_common_pdsch_time_domain_list(const pdsch_config_common& pdsch_common,
                                                         cyclic_prefix              cp,
                                                         dmrs_typeA_position        dmrs_typeA_pos)
{
  // See TS 38.214, Table 5.1.2.1.1-1: Applicable PDSCH time domain resource allocation for SI-RNTI Type0A common.
  if (not pdsch_common.pdsch_td_alloc_list.empty()) {
    return pdsch_common.pdsch_td_alloc_list;
  }
  // TODO: Check for multiplexing pattern and SearchSpace type (Type0A common).
  // NOTE: Type0A common PDCCH and multiplexing pattern 1 is assumed to fetch applicable PDSCH time domain resource
  // allocation.
  return pdsch_default_time_allocations_default_A_table(cp, dmrs_typeA_pos);
}

span<const pdsch_time_domain_resource_allocation>
srsran::get_ra_rnti_pdsch_time_domain_list(const pdsch_config_common& pdsch_common,
                                           cyclic_prefix              cp,
                                           dmrs_typeA_position        dmrs_typeA_pos)
{
  // See TS 38.214, Table 5.1.2.1.1-1: Applicable PDSCH time domain resource allocation for RA-RNTI.
  if (not pdsch_common.pdsch_td_alloc_list.empty()) {
    return pdsch_common.pdsch_td_alloc_list;
  }
  return pdsch_default_time_allocations_default_A_table(cp, dmrs_typeA_pos);
}
