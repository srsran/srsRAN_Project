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

#include "srsran/ofh/compression/compressed_prb.h"

namespace srsran {
namespace ofh {

/// Provides methods to unpack compressed IQ samples from the underlying \c compressed_prb object.
class compressed_prb_unpacker
{
  /// Handle to the storage.
  const compressed_prb& prb;

public:
  explicit compressed_prb_unpacker(const compressed_prb& prb_) : prb(prb_) {}

  /// Extract \c length bits from the underlying storage starting from offset \c offset.
  int16_t unpack(unsigned offset, unsigned length) const;
};

} // namespace ofh
} // namespace srsran
