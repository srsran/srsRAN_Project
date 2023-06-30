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
