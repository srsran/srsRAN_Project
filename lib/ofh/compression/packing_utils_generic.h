/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
