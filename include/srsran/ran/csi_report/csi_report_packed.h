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
#include "srsran/support/units.h"

namespace srsran {

/// \brief Maximum Channel State Information (CSI) report size in bits.
///
/// Maximum CSI-Part1 report payload size for report quantity \e cri-RI-LI-PMI-CQI, wideband, four port
/// TypeI-SinglePanel PMI codebook, and multiplexed PUCCH.
static constexpr units::bits csi_report_max_size(17U);

/// Packed Channel State Information (CSI) report data type.
using csi_report_packed = bounded_bitset<csi_report_max_size.value(), false>;

} // namespace srsran