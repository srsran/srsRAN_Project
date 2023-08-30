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

#include "srsran/adt/optional.h"
#include "srsran/ran/pdcch/aggregation_level.h"
#include "srsran/ran/pdcch/dci_packing.h"

namespace srsran {

enum class cqi_table_t;

/// \brief Computes PDCCH aggregation level to use based on the input parameters.
///
/// \param[in] cqi CQI reported by the UE.
/// \param[in] cqi_table CQI table to be used for the mapping.
/// \param[in] min_aggr_lvl Minimum aggregation level applicable for PDCCH scheduling.
/// \param[in] max_aggr_lvl Maximum aggregation level applicable for PDCCH scheduling.
/// \param[in] nof_dci_bits PDCCH DCI size in nof. bits.
/// \return PDCCH aggregation level if a valid candidate is found. Else, returns nullopt.
optional<aggregation_level> map_cqi_to_aggregation_level(unsigned          cqi,
                                                         cqi_table_t       cqi_table,
                                                         aggregation_level min_aggr_lvl,
                                                         aggregation_level max_aggr_lvl,
                                                         unsigned          nof_dci_bits);

} // namespace srsran
