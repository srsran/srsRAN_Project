/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/ran/csi_rs/csi_rs_types.h"
#include "srsran/ran/csi_rs/frequency_allocation_type.h"

namespace srsran {

namespace csi_rs {

/// \brief Returns the row index corresponding to Table 7.4.1.5.3-1, TS 38.211 given the CSI-RS configuration.
/// \param[in] nof_ports Number of ports, as per Table 7.4.1.5.3-1, TS 38.211.
/// \param[in] density Density, as per Table 7.4.1.5.3-1, TS 38.211.
/// \param[in] cdm_type CDM-type, as per Table 7.4.1.5.3-1, TS 38.211.
/// \param[in] fd_alloc Freq. Domain Allocation, as per \c frequencyDomainAllocation in \c CSI-RS-ResourceMapping,
///            TS 38.331.
/// \return The row index of Table 7.4.1.5.3-1, TS 38.211 corresponding to the inputs. If the given input configuration
///         does not match any of the rows in the table, the function triggers an assert and returns an invalid value
///         (i.e., 0).
unsigned get_csi_rs_resource_mapping_row_number(uint8_t                          nof_ports,
                                                csi_rs_freq_density_type         density,
                                                csi_rs_cdm_type                  cdm_type,
                                                const freq_allocation_mask_type& fd_alloc);

/// \brief returns the number of CSI-RS ports corresponding to a CSI-RS mapping table entry.
///
/// The number of ports for each CSI-RS mapping format is defined in TS38.211 Table 7.4.1.5.3-1.
///
/// \param csi_rs_row_id CSI-RS mapping table row identifier, set as per the first column in TS38.211 Table 7.4.1.5.3-1.
/// \return The number of ports for the specified CSI-RS mapping table entry.
unsigned get_nof_csi_rs_ports(unsigned csi_rs_row_id);

} // namespace csi_rs

} // namespace srsran
