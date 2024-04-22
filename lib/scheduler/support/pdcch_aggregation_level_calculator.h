/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
/// \param[in] cqi CQI reported by the UE + OLLA adjustment.
/// \param[in] cqi_table CQI table to be used for the mapping.
/// \param[in] pdcch_candidates PDCCH candidates per aggregation level, where aggregation level for the array element
/// with index "x" is L=1U << x.
/// \param[in] nof_dci_bits PDCCH DCI size in nof. bits.
/// \return PDCCH aggregation level if a valid candidate is found. Else, returns highest aggregation level with PDCCH
/// candidate configured.
aggregation_level map_cqi_to_aggregation_level(float               cqi,
                                               cqi_table_t         cqi_table,
                                               span<const uint8_t> pdcch_candidates,
                                               unsigned            nof_dci_bits);

} // namespace srsran
