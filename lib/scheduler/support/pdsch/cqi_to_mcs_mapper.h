/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/pdsch/pdsch_mcs.h"
#include "srsgnb/ran/sch_mcs.h"

namespace srsgnb {

/// \brief Performs CQI to MCS mapping, as per Tables Table 5.2.2.1-2, Table 5.2.2.1-3, or Table 5.2.2.1-4, TS 38.214.
///
/// \param[in] cqi CQI reported by the UE.
/// \param[in] mcs_table MCS table to be used for the mapping.
/// \return The MCS corresponding to map.
optional<sch_mcs_index> map_cqi_to_mcs(unsigned cqi, pdsch_mcs_table mcs_table);

} // namespace srsgnb
