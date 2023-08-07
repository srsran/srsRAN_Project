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
#include "srsran/adt/span.h"
#include "srsran/ran/uci/uci_part2_size_description.h"

namespace srsran {

/// \brief Calculates the UCI part 2 from UCI part 1.
/// \param[in] part1 UCI part 1 decoded data.
/// \param[in] descr UCI part 1 parameters correspondence to UCI part 2 size.
/// \return The size of UCI part 2 payload.
unsigned uci_part2_get_size(span<const uint8_t> part1, const uci_part2_size_description& descr);

} // namespace srsran
