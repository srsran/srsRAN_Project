/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
