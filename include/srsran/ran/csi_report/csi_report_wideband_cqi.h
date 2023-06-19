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