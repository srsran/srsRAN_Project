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

#include "srsran/ofh/compression/compressed_prb.h"

namespace srsran {
namespace ofh {

/// Provides methods to pack compressed IQ samples into the underlying \c compressed_prb object.
class compressed_prb_packer
{
  /// Handle to the storage.
  compressed_prb& prb;

public:
  explicit compressed_prb_packer(compressed_prb& prb_) : prb(prb_) {}

  /// Packs the given IQ samples into the underlying object.
  void pack(span<const int16_t> compressed_iq, unsigned iq_width);
};

} // namespace ofh
} // namespace srsran
