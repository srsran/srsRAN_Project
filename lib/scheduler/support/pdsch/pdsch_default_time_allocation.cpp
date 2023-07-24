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

#include "pdsch_default_time_allocation.h"

using namespace srsran;

static pdsch_default_time_allocation_config
pdsch_default_time_allocation_default_A_get_normal(unsigned row_index, dmrs_typeA_position dmrs_pos)
{
  // TS38.214 Table 5.1.2.1.1-2. Default PDSCH time domain resource allocation A for normal CP.
  static const std::array<pdsch_default_time_allocation_config, 16> TABLE = {{{sch_mapping_type::typeA, 0, 2, 12},
                                                                              {sch_mapping_type::typeA, 0, 2, 10},
                                                                              {sch_mapping_type::typeA, 0, 2, 9},
                                                                              {sch_mapping_type::typeA, 0, 2, 7},
                                                                              {sch_mapping_type::typeA, 0, 2, 5},
                                                                              {sch_mapping_type::typeB, 0, 9, 4},
                                                                              {sch_mapping_type::typeB, 0, 4, 4},
                                                                              {sch_mapping_type::typeB, 0, 5, 7},
                                                                              {sch_mapping_type::typeB, 0, 5, 2},
                                                                              {sch_mapping_type::typeB, 0, 9, 2},
                                                                              {sch_mapping_type::typeB, 0, 12, 2},
                                                                              {sch_mapping_type::typeA, 0, 1, 13},
                                                                              {sch_mapping_type::typeA, 0, 1, 6},
                                                                              {sch_mapping_type::typeA, 0, 2, 4},
                                                                              {sch_mapping_type::typeB, 0, 4, 7},
                                                                              {sch_mapping_type::typeB, 0, 8, 4}}};

  if (row_index >= TABLE.size()) {
    return PDSCH_DEFAULT_TIME_ALLOCATION_RESERVED;
  }

  pdsch_default_time_allocation_config result = TABLE[row_index];
  if (dmrs_pos == dmrs_typeA_position::pos3) {
    if (row_index < 5) {
      ++result.start_symbol;
      --result.duration;
    } else if (row_index == 5) {
      ++result.start_symbol;
    } else if (row_index == 6) {
      result.start_symbol += 2;
    }
  }

  return result;
}

static pdsch_default_time_allocation_config
pdsch_default_time_allocation_default_A_get_extended(unsigned row_index, dmrs_typeA_position dmrs_pos)
{
  // TS38.214 Table 5.1.2.1.1-2. Default PDSCH time domain resource allocation A for normal CP.
  static const std::array<pdsch_default_time_allocation_config, 16> TABLE = {{{sch_mapping_type::typeA, 0, 2, 6},
                                                                              {sch_mapping_type::typeA, 0, 2, 10},
                                                                              {sch_mapping_type::typeA, 0, 2, 9},
                                                                              {sch_mapping_type::typeA, 0, 2, 7},
                                                                              {sch_mapping_type::typeA, 0, 2, 5},
                                                                              {sch_mapping_type::typeB, 0, 6, 4},
                                                                              {sch_mapping_type::typeB, 0, 4, 4},
                                                                              {sch_mapping_type::typeB, 0, 5, 6},
                                                                              {sch_mapping_type::typeB, 0, 5, 2},
                                                                              {sch_mapping_type::typeB, 0, 9, 2},
                                                                              {sch_mapping_type::typeB, 0, 10, 2},
                                                                              {sch_mapping_type::typeA, 0, 1, 11},
                                                                              {sch_mapping_type::typeA, 0, 1, 6},
                                                                              {sch_mapping_type::typeA, 0, 2, 4},
                                                                              {sch_mapping_type::typeB, 0, 4, 6},
                                                                              {sch_mapping_type::typeB, 0, 8, 4}}};

  if (row_index >= TABLE.size()) {
    return PDSCH_DEFAULT_TIME_ALLOCATION_RESERVED;
  }

  pdsch_default_time_allocation_config result = TABLE[row_index];
  if (dmrs_pos == dmrs_typeA_position::pos3) {
    if (row_index < 5) {
      ++result.start_symbol;
      --result.duration;
    } else if (row_index == 5) {
      result.start_symbol += 2;
      result.duration -= 2;
    } else if (row_index == 6) {
      result.start_symbol += 2;
    }
  }

  return result;
}

pdsch_default_time_allocation_config
srsran::pdsch_default_time_allocation_default_A_get(cyclic_prefix cp, unsigned row_index, dmrs_typeA_position dmrs_pos)
{
  switch (cp) {
    case cyclic_prefix::NORMAL:
      return pdsch_default_time_allocation_default_A_get_normal(row_index, dmrs_pos);
    case cyclic_prefix::EXTENDED:
    default:
      return pdsch_default_time_allocation_default_A_get_extended(row_index, dmrs_pos);
  }
}

span<const pdsch_time_domain_resource_allocation>
srsran::pdsch_default_time_allocations_default_A_table(cyclic_prefix cp, dmrs_typeA_position dmrs_pos)
{
  // TS38.214 Table 5.1.2.1.1-2.
  static constexpr size_t PDSCH_TD_RES_ALLOC_TABLE_SIZE = 16;

  // Build PDSCH-TimeDomain tables statically.
  auto table_builder = [](cyclic_prefix cp_, dmrs_typeA_position dmrs_pos_) {
    std::array<pdsch_time_domain_resource_allocation, PDSCH_TD_RES_ALLOC_TABLE_SIZE> table;
    for (unsigned i = 0; i < PDSCH_TD_RES_ALLOC_TABLE_SIZE; ++i) {
      pdsch_default_time_allocation_config cfg = pdsch_default_time_allocation_default_A_get(cp_, i, dmrs_pos_);
      table[i].k0                              = cfg.pdcch_to_pdsch_delay;
      table[i].map_type                        = cfg.mapping_type;
      table[i].symbols                         = {cfg.start_symbol, cfg.start_symbol + cfg.duration};
    }
    return table;
  };
  static const std::array<std::array<pdsch_time_domain_resource_allocation, PDSCH_TD_RES_ALLOC_TABLE_SIZE>, 4> tables =
      {table_builder(cyclic_prefix::NORMAL, dmrs_typeA_position::pos2),
       table_builder(cyclic_prefix::NORMAL, dmrs_typeA_position::pos3),
       table_builder(cyclic_prefix::EXTENDED, dmrs_typeA_position::pos2),
       table_builder(cyclic_prefix::EXTENDED, dmrs_typeA_position::pos3)};

  // Retrieve respective table.
  return tables[static_cast<unsigned>(cp.value) * 2 + (static_cast<unsigned>(dmrs_pos) - 2)];
}

span<const pdsch_time_domain_resource_allocation>
srsran::get_c_rnti_pdsch_time_domain_list(const search_space_configuration& ss_cfg,
                                          const bwp_downlink_common&        active_bwp_dl_common,
                                          const bwp_downlink_dedicated*     active_bwp_dl_ded,
                                          dmrs_typeA_position               dmrs_typeA_pos)
{
  const bool is_fallback = ss_cfg.is_common_search_space() and ss_cfg.get_coreset_id() == to_coreset_id(0);
  srsran_assert(is_fallback or active_bwp_dl_ded != nullptr, "Invalid BWP DL dedicated configuration");

  // See TS 38.214, Table 5.1.2.1.1-1: Applicable PDSCH time domain resource allocation for DCI formats 1_0 and 1_1.
  if (not is_fallback) {
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
  // TODO: Check for multiplexing pattern and SearchSpace type (Type0 common or Type0A common).
  // NOTE: Type0 common PDCCH and multiplexing pattern 1 is assumed to fetch applicable PDSCH time domain resource
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
