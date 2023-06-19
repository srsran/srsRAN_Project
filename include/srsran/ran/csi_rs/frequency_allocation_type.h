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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/csi_rs/csi_rs_constants.h"

namespace srsran {

namespace csi_rs {

/// \brief Frequency domain allocation within a physical resource block in accordance with TS 38.211,
/// clause 7.4.1.5.3, and \c frequencyDomainAllocation in \c CSI-RS-ResourceMapping in TS 38.331. Possible sizes:
/// {row1: 4, row2: 12, row4: 3, row_other: 6}
using freq_allocation_mask_type = bounded_bitset<12, false>;

/// \brief Frequency domain allocation indices \f${k_0, k_1, ..., k_n}\f$, as per Section 7.4.1.5.3 TS 38.211,
/// corresponding to \ref freq_allocation_mask_type. The mapping between \ref freq_allocation_mask_type and this is
/// defined in Section 7.4.1.5.3 TS 38.211.
using freq_allocation_index_type = static_vector<unsigned, CSI_RS_MAX_NOF_K_INDEXES>;

/// Converts a frequency domain bitmap to the corresponding indices \f${k_0, k_1, ..., k_n}\f$, as per Section 7.4.1.5.3
/// TS 38.211.
void convert_freq_domain(freq_allocation_index_type& dst, const freq_allocation_mask_type& src, unsigned row);
} // namespace csi_rs

} // namespace srsran
