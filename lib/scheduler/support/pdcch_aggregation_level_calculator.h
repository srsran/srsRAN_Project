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

#include "srsran/adt/optional.h"
#include "srsran/ran/csi_report/csi_report_data.h"
#include "srsran/ran/pdcch/aggregation_level.h"
#include "srsran/ran/pdcch/dci_packing.h"

namespace srsran {

enum class cqi_table_t;

/// \brief Computes PDCCH aggregation level to use based on the input parameters.
///
/// \param[in] cqi CQI reported by the UE.
/// \param[in] cqi_table CQI table to be used for the mapping.
/// \param[in] pdcch_candidates PDCCH candidates per aggregation level, where aggregation level for the array element
/// with index "x" is L=1U << x.
/// \param[in] nof_dci_bits PDCCH DCI size in nof. bits.
/// \return PDCCH aggregation level if a valid candidate is found. Else, returns highest aggregation level with PDCCH
/// candidate configured.
aggregation_level map_cqi_to_aggregation_level(cqi_value           cqi,
                                               cqi_table_t         cqi_table,
                                               span<const uint8_t> pdcch_candidates,
                                               unsigned            nof_dci_bits);

} // namespace srsran
