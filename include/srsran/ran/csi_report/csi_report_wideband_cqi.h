/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsran/adt/bounded_integer.h"
#include "srsran/ran/sch_mcs.h"

namespace srsran {

/// Reserved wideband Channel Quality Indicator (CQI) value.
static constexpr sch_mcs_description csi_report_reserved_wideband_cqi = {};

/// \brief Channel Quality Indicator (CQI) table.
///
/// Selects the CQI table to use for the wideband CQI value selection.
enum class cqi_table_type : uint8_t {
  /// Select TS38.214 Table 5.2.2.1-2.
  table_1 = 0,
  /// Select TS38.214 Table 5.2.2.1-3.
  table_2,
  /// Select TS38.214 Table 5.2.2.1-4.
  table_3,
};

/// Channel Quality Indicator type.
using csi_report_wideband_cqi_type = bounded_integer<uint8_t, 0, 15>;

/// \brief Converts a wideband Chanel Quality Indication (CQI) value to a MCS description.
///
/// \param table
/// \param wideband_cqi
/// \return
sch_mcs_description to_mcs_description(cqi_table_type table, csi_report_wideband_cqi_type wideband_cqi);

} // namespace srsran