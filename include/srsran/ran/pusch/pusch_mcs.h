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

#pragma once

#include "srsran/ran/sch/sch_mcs.h"

namespace srsran {

/// \brief Modulation and Code Scheme table identifiers for PUSCH.
///
/// The identifiers are inspired by TS38.331 Section as 6.3.2 (see field \e mcs-Table in various Information Elements).
/// For the actual tables and the selection procedure, see TS38.214 Section 6.1.4.1.
enum class pusch_mcs_table {
  /// Identifies MCS index table 1, TS38.214 Table 5.1.3.1-1.
  qam64 = 0,
  /// Identifies MCS index table 2, TS38.214 Table 5.1.3.1-2.
  qam256 = 1,
  /// Identifies MCS index table 3, TS38.214 Table 5.1.3.1-3.
  qam64LowSe = 2,
};

/// Convert PUSCH MCS table to string.
constexpr const char* pusch_mcs_table_to_string(pusch_mcs_table table)
{
  switch (table) {
    case pusch_mcs_table::qam64:
      return "qam64";
    case pusch_mcs_table::qam256:
      return "qam256";
    case pusch_mcs_table::qam64LowSe:
      return "qam64LowSe";
    default:
      break;
  }
  return "invalid";
}

/// \brief Gets the Modulation and Coding Scheme configuration for PUSCH.
///
/// Reserved target code rates and spectral efficiencies are indicated with zero.
///
/// \param[in] table                  MCS table.
/// \param[in] index                  MCS index.
/// \param[in] use_transform_precoder Set to true if transform precoding is enabled.
/// \param[in] tp_pi2bpsk_present     Set to true if higher layer parameter \e tp-pi2BPSK is enabled. See TS38.331
///                                   Section 6.3.2 Information Element \e PUSCH-Config for more information.
/// \return Modulation and Coding Scheme parameters.
sch_mcs_description
pusch_mcs_get_config(pusch_mcs_table table, sch_mcs_index index, bool use_transform_precoder, bool tp_pi2bpsk_present);

} // namespace srsran
