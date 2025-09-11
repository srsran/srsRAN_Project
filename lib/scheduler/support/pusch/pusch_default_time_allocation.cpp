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

#include "pusch_default_time_allocation.h"
#include "srsran/scheduler/config/serving_cell_config.h"

using namespace srsran;

/// Reserved default PUSCH time-domain allocation. It indicates the configuration is invalid.
static constexpr pusch_time_domain_resource_allocation PUSCH_DEFAULT_TIME_ALLOCATION_RESERVED = {};

/// Helper to construct ofdm symbol range
static constexpr ofdm_symbol_range s_and_len(unsigned s, unsigned l)
{
  return ofdm_symbol_range::start_and_len(s, l);
}

static constexpr unsigned pusch_default_time_allocation_default_A_get_j(subcarrier_spacing scs)
{
  // TS38.214 Table 6.1.2.1.1-4. Definition of value j.
  constexpr std::array<unsigned, 4> values = {1, 1, 2, 3};

  unsigned numerology = to_numerology_value(scs);
  srsran_assert(numerology < values.size(), "Invalid numerology.");
  return values[numerology];
}

span<const pusch_time_domain_resource_allocation>
srsran::pusch_default_time_allocations_default_A_table(cyclic_prefix cp, subcarrier_spacing scs)
{
  // TS38.214 Table 6.1.2.1.1-2. Default PUSCH time domain resource allocation A for normal CP.
  static constexpr std::array<std::array<pusch_time_domain_resource_allocation, 16>, 4> table_normal_cp = []() {
    std::array<std::array<pusch_time_domain_resource_allocation, 16>, 4> table = {};
    table[0] = {{{0, sch_mapping_type::typeA, s_and_len(0, 14)},
                 {0, sch_mapping_type::typeA, s_and_len(0, 12)},
                 {0, sch_mapping_type::typeA, s_and_len(0, 10)},
                 {0, sch_mapping_type::typeB, s_and_len(2, 10)},
                 {0, sch_mapping_type::typeB, s_and_len(4, 10)},
                 {0, sch_mapping_type::typeB, s_and_len(4, 8)},
                 {0, sch_mapping_type::typeB, s_and_len(4, 6)},
                 {1, sch_mapping_type::typeA, s_and_len(0, 14)},
                 {1, sch_mapping_type::typeA, s_and_len(0, 12)},
                 {1, sch_mapping_type::typeA, s_and_len(0, 10)},
                 {2, sch_mapping_type::typeA, s_and_len(0, 14)},
                 {2, sch_mapping_type::typeA, s_and_len(0, 12)},
                 {2, sch_mapping_type::typeA, s_and_len(0, 10)},
                 {0, sch_mapping_type::typeB, s_and_len(8, 6)},
                 {3, sch_mapping_type::typeA, s_and_len(0, 14)},
                 {3, sch_mapping_type::typeA, s_and_len(0, 10)}}};
    table[1] = table[0];
    table[2] = table[0];
    table[3] = table[0];
    for (unsigned u = 0; u != 4; ++u) {
      for (unsigned j = 0; j != 16; ++j) {
        // Add parameter j.
        table[u][j].k2 += pusch_default_time_allocation_default_A_get_j(to_subcarrier_spacing(u));
      }
    }
    return table;
  }();

  // TS38.214 Table 6.1.2.1.1-3. Default PUSCH time domain resource allocation A for extended CP.
  static constexpr std::array<std::array<pusch_time_domain_resource_allocation, 16>, 4> table_extended_cp = []() {
    std::array<std::array<pusch_time_domain_resource_allocation, 16>, 4> table = {};
    table[0] = {{{0, sch_mapping_type::typeA, s_and_len(0, 8)},
                 {0, sch_mapping_type::typeA, s_and_len(0, 12)},
                 {0, sch_mapping_type::typeA, s_and_len(0, 10)},
                 {0, sch_mapping_type::typeB, s_and_len(2, 10)},
                 {0, sch_mapping_type::typeB, s_and_len(4, 4)},
                 {0, sch_mapping_type::typeB, s_and_len(4, 8)},
                 {0, sch_mapping_type::typeB, s_and_len(4, 6)},
                 {1, sch_mapping_type::typeA, s_and_len(0, 8)},
                 {1, sch_mapping_type::typeA, s_and_len(0, 12)},
                 {1, sch_mapping_type::typeA, s_and_len(0, 10)},
                 {2, sch_mapping_type::typeA, s_and_len(0, 6)},
                 {2, sch_mapping_type::typeA, s_and_len(0, 12)},
                 {2, sch_mapping_type::typeA, s_and_len(0, 10)},
                 {0, sch_mapping_type::typeB, s_and_len(8, 4)},
                 {3, sch_mapping_type::typeA, s_and_len(0, 8)},
                 {3, sch_mapping_type::typeA, s_and_len(0, 10)}}};
    table[1] = table[0];
    table[2] = table[0];
    table[3] = table[0];
    for (unsigned u = 0; u != 4; ++u) {
      for (unsigned j = 0; j != 16; ++j) {
        // Add parameter j.
        table[u][j].k2 += pusch_default_time_allocation_default_A_get_j(to_subcarrier_spacing(u));
      }
    }
    return table;
  }();

  return (cp == cyclic_prefix::NORMAL ? table_normal_cp : table_extended_cp)[to_numerology_value(scs)];
}

pusch_time_domain_resource_allocation
srsran::pusch_default_time_allocation_default_A_get(cyclic_prefix cp, unsigned row_index, subcarrier_spacing scs)
{
  span<const pusch_time_domain_resource_allocation> table = pusch_default_time_allocations_default_A_table(cp, scs);
  if (row_index >= table.size()) {
    return PUSCH_DEFAULT_TIME_ALLOCATION_RESERVED;
  }
  return table[row_index];
}

span<const pusch_time_domain_resource_allocation>
srsran::get_c_rnti_pusch_time_domain_list(bool                        is_common_ss,
                                          coreset_id                  cs_id,
                                          const bwp_uplink_common&    active_bwp_ul_common,
                                          const bwp_uplink_dedicated* active_bwp_ul_ded)
{
  bool fallback     = is_common_ss and cs_id == to_coreset_id(0);
  active_bwp_ul_ded = fallback ? nullptr : active_bwp_ul_ded;

  // See TS 38.214, Table 6.1.2.1.1-1.
  if (active_bwp_ul_ded != nullptr and active_bwp_ul_ded->pusch_cfg.has_value() and
      not active_bwp_ul_ded->pusch_cfg.value().pusch_td_alloc_list.empty()) {
    // Cases:
    // - Any common search space not associated with CORESET 0, pusch-Config includes pusch-TimeDomainAllocationList
    // - UE specific search space, pusch-Config includes pusch-TimeDomainAllocationList
    return active_bwp_ul_ded->pusch_cfg.value().pusch_td_alloc_list;
  }

  if (active_bwp_ul_common.pusch_cfg_common.has_value() and
      (not active_bwp_ul_common.pusch_cfg_common.value().pusch_td_alloc_list.empty())) {
    // Case in which pusch-ConfigCommon includes puschTimeDomainAllocationList
    return active_bwp_ul_common.pusch_cfg_common.value().pusch_td_alloc_list;
  }

  // default A table case.
  return pusch_default_time_allocations_default_A_table(active_bwp_ul_common.generic_params.cp,
                                                        active_bwp_ul_common.generic_params.scs);
}

span<const pusch_time_domain_resource_allocation>
srsran::get_c_rnti_pusch_time_domain_list(const search_space_configuration& ss_cfg,
                                          const bwp_uplink_common&          active_bwp_ul_common,
                                          const bwp_uplink_dedicated*       active_bwp_ul_ded)
{
  return get_c_rnti_pusch_time_domain_list(
      ss_cfg.is_common_search_space(), ss_cfg.get_coreset_id(), active_bwp_ul_common, active_bwp_ul_ded);
}
