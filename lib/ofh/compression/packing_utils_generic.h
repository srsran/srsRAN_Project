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

#include "srsran/adt/bit_buffer.h"
#include "srsran/adt/span.h"
#include "srsran/support/units.h"

namespace srsran {
namespace ofh {

/// Packs the given IQ samples into the given bit buffer object.
void pack_bytes(bit_buffer& buffer, span<const int16_t> compressed_iq, unsigned iq_width);

/// Extract \c length bits from the provided bit buffer starting from offset \c offset.
int16_t unpack_bits(const bit_buffer_reader& buffer, unsigned offset, unsigned length);

} // namespace ofh
} // namespace srsran
