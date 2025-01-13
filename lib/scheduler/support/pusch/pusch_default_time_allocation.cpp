/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pusch_default_time_allocation.h"
#include "srsran/scheduler/config/serving_cell_config.h"

using namespace srsran;

/// Reserved default PUSCH time-domain allocation. It indicates the configuration is invalid.
static constexpr pusch_time_domain_resource_allocation PUSCH_DEFAULT_TIME_ALLOCATION_RESERVED = {};

namespace {

// Helper to construct ofdm symbol range
constexpr ofdm_symbol_range s_and_len(unsigned s, unsigned l)
{
  return ofdm_symbol_range::start_and_len(s, l);
}

unsigned pusch_default_time_allocation_default_A_get_j(subcarrier_spacing scs)
{
  // TS38.214 Table 6.1.2.1.1-4. Definition of value j.
  static std::array<unsigned, 4> values = {1, 1, 2, 3};

  unsigned numerology = to_numerology_value(scs);
  srsran_assert(numerology < values.size(), "Invalid numerology.");
  return values[numerology];
}

pusch_time_domain_resource_allocation pusch_default_time_allocation_default_A_get_normal(unsigned           row_index,
                                                                                         subcarrier_spacing scs)
{
  // TS38.214 Table 6.1.2.1.1-2. Default PUSCH time domain resource allocation A for normal CP.
  static const std::array<pusch_time_domain_resource_allocation, 16> TABLE = {
      {{0, sch_mapping_type::typeA, s_and_len(0, 14)},
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

  if (row_index >= TABLE.size()) {
    return PUSCH_DEFAULT_TIME_ALLOCATION_RESERVED;
  }

  // Select row.
  pusch_time_domain_resource_allocation result = TABLE[row_index];

  // Add parameter j.
  result.k2 += pusch_default_time_allocation_default_A_get_j(scs);

  return result;
}

pusch_time_domain_resource_allocation pusch_default_time_allocation_default_A_get_extended(unsigned           row_index,
                                                                                           subcarrier_spacing scs)
{
  // TS38.214 Table 6.1.2.1.1-3. Default PUSCH time domain resource allocation A for extended CP.
  static const std::array<pusch_time_domain_resource_allocation, 16> TABLE = {
      {{0, sch_mapping_type::typeA, s_and_len(0, 8)},
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

  if (row_index >= TABLE.size()) {
    return PUSCH_DEFAULT_TIME_ALLOCATION_RESERVED;
  }

  // Select row.
  pusch_time_domain_resource_allocation result = TABLE[row_index];

  // Add parameter j.
  result.k2 += pusch_default_time_allocation_default_A_get_j(scs);

  return result;
}

} // namespace

pusch_time_domain_resource_allocation
srsran::pusch_default_time_allocation_default_A_get(cyclic_prefix cp, unsigned row_index, subcarrier_spacing scs)
{
  switch (cp) {
    case cyclic_prefix::NORMAL:
      return pusch_default_time_allocation_default_A_get_normal(row_index, scs);
    case cyclic_prefix::EXTENDED:
    default:
      return pusch_default_time_allocation_default_A_get_extended(row_index, scs);
  }
}

span<const pusch_time_domain_resource_allocation>
srsran::pusch_default_time_allocations_default_A_table(cyclic_prefix cp, subcarrier_spacing scs)
{
  // TS38.214 Table 5.1.2.1.1-2.
  static constexpr size_t PUSCH_TD_RES_ALLOC_TABLE_SIZE = 16;

  // Build PUSCH-TimeDomain tables statically.
  auto table_builder = [](cyclic_prefix cp_, subcarrier_spacing scs_) {
    std::array<pusch_time_domain_resource_allocation, PUSCH_TD_RES_ALLOC_TABLE_SIZE> table;
    for (unsigned i = 0; i < PUSCH_TD_RES_ALLOC_TABLE_SIZE; ++i) {
      table[i] = pusch_default_time_allocation_default_A_get(cp_, i, scs_);
    }
    return table;
  };
  static const std::array<std::array<pusch_time_domain_resource_allocation, PUSCH_TD_RES_ALLOC_TABLE_SIZE>, 8> tables =
      {table_builder(cyclic_prefix::NORMAL, to_subcarrier_spacing(0)),
       table_builder(cyclic_prefix::NORMAL, to_subcarrier_spacing(1)),
       table_builder(cyclic_prefix::NORMAL, to_subcarrier_spacing(2)),
       table_builder(cyclic_prefix::NORMAL, to_subcarrier_spacing(3)),
       table_builder(cyclic_prefix::EXTENDED, to_subcarrier_spacing(0)),
       table_builder(cyclic_prefix::EXTENDED, to_subcarrier_spacing(1)),
       table_builder(cyclic_prefix::EXTENDED, to_subcarrier_spacing(2)),
       table_builder(cyclic_prefix::EXTENDED, to_subcarrier_spacing(3))};

  // Retrieve respective table.
  return tables[static_cast<unsigned>(cp.value) * 4 + to_numerology_value(scs)];
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
