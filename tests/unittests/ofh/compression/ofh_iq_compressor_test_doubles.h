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

#include "srsran/ofh/compression/iq_compressor.h"

namespace srsran {
namespace ofh {
namespace testing {

/// Dummy IQ compressor implementation
class iq_compressor_dummy : public iq_compressor
{
public:
  // See interface for documentation.
  void compress(span<uint8_t> buffer, span<const cbf16_t> iq_data, const ru_compression_params& params) override {}
};

} // namespace testing
} // namespace ofh
} // namespace srsran
