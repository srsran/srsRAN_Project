/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pusch_default_time_allocation.h"

using namespace srsran;

namespace {
unsigned pusch_default_time_allocation_default_A_get_j(subcarrier_spacing scs)
{
  // TS38.214 Table 6.1.2.1.1-4. Definition of value j.
  static std::array<unsigned, 4> values = {1, 1, 2, 3};

  unsigned numerology = to_numerology_value(scs);
  srsran_assert(numerology < values.size(), "Invalid numerology.");
  return values[numerology];
}

pusch_default_time_allocation_config pusch_default_time_allocation_default_A_get_normal(unsigned           row_index,
                                                                                        subcarrier_spacing scs)
{
  // TS38.214 Table 6.1.2.1.1-2. Default PUSCH time domain resource allocation A for normal CP.
  static const std::array<pusch_default_time_allocation_config, 16> TABLE = {{{sch_mapping_type::typeA, 0, 0, 14},
                                                                              {sch_mapping_type::typeA, 0, 0, 12},
                                                                              {sch_mapping_type::typeA, 0, 0, 10},
                                                                              {sch_mapping_type::typeB, 0, 2, 10},
                                                                              {sch_mapping_type::typeB, 0, 4, 10},
                                                                              {sch_mapping_type::typeB, 0, 4, 8},
                                                                              {sch_mapping_type::typeB, 0, 4, 6},
                                                                              {sch_mapping_type::typeA, 1, 0, 14},
                                                                              {sch_mapping_type::typeA, 1, 0, 12},
                                                                              {sch_mapping_type::typeA, 1, 0, 10},
                                                                              {sch_mapping_type::typeA, 2, 0, 14},
                                                                              {sch_mapping_type::typeA, 2, 0, 12},
                                                                              {sch_mapping_type::typeA, 2, 0, 10},
                                                                              {sch_mapping_type::typeB, 0, 8, 6},
                                                                              {sch_mapping_type::typeA, 3, 0, 14},
                                                                              {sch_mapping_type::typeA, 3, 0, 10}}};

  if (row_index >= TABLE.size()) {
    return PUSCH_DEFAULT_TIME_ALLOCATION_RESERVED;
  }

  // Select row.
  pusch_default_time_allocation_config result = TABLE[row_index];

  // Add parameter j.
  result.k2 += pusch_default_time_allocation_default_A_get_j(scs);

  return result;
}

pusch_default_time_allocation_config pusch_default_time_allocation_default_A_get_extended(unsigned           row_index,
                                                                                          subcarrier_spacing scs)
{
  // TS38.214 Table 6.1.2.1.1-3. Default PUSCH time domain resource allocation A for extended CP.
  static const std::array<pusch_default_time_allocation_config, 16> TABLE = {{{sch_mapping_type::typeA, 0, 0, 8},
                                                                              {sch_mapping_type::typeA, 0, 0, 12},
                                                                              {sch_mapping_type::typeA, 0, 0, 10},
                                                                              {sch_mapping_type::typeB, 0, 2, 10},
                                                                              {sch_mapping_type::typeB, 0, 4, 4},
                                                                              {sch_mapping_type::typeB, 0, 4, 8},
                                                                              {sch_mapping_type::typeB, 0, 4, 6},
                                                                              {sch_mapping_type::typeA, 1, 0, 8},
                                                                              {sch_mapping_type::typeA, 1, 0, 12},
                                                                              {sch_mapping_type::typeA, 1, 0, 10},
                                                                              {sch_mapping_type::typeA, 2, 0, 6},
                                                                              {sch_mapping_type::typeA, 2, 0, 12},
                                                                              {sch_mapping_type::typeA, 2, 0, 10},
                                                                              {sch_mapping_type::typeB, 0, 8, 4},
                                                                              {sch_mapping_type::typeA, 3, 0, 8},
                                                                              {sch_mapping_type::typeA, 3, 0, 10}}};

  if (row_index >= TABLE.size()) {
    return PUSCH_DEFAULT_TIME_ALLOCATION_RESERVED;
  }

  // Select row.
  pusch_default_time_allocation_config result = TABLE[row_index];

  // Add parameter j.
  result.k2 += pusch_default_time_allocation_default_A_get_j(scs);

  return result;
}

} // namespace

pusch_default_time_allocation_config
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
      pusch_default_time_allocation_config cfg = pusch_default_time_allocation_default_A_get(cp_, i, scs_);
      table[i].k2                              = cfg.k2;
      table[i].map_type                        = cfg.mapping_type;
      table[i].symbols                         = {cfg.start_symbol, cfg.duration};
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
