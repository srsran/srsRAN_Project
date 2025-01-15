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

#include "srsran/ofh/compression/iq_decompressor.h"

namespace srsran {
namespace ofh {

/// \brief IQ decompressor selector implementation.
///
/// The selector will select the IQ decompressor between the registered ones to decompress IQ samples, based on the
/// given compression parameters.
class iq_decompressor_selector : public iq_decompressor
{
public:
  explicit iq_decompressor_selector(
      std::array<std::unique_ptr<iq_decompressor>, NOF_COMPRESSION_TYPES_SUPPORTED> decompressors_);

  // See interface for documentation.
  void
  decompress(span<cbf16_t> iq_data, span<const uint8_t> compressed_data, const ru_compression_params& params) override;

private:
  std::array<std::unique_ptr<iq_decompressor>, NOF_COMPRESSION_TYPES_SUPPORTED> decompressors;
};

} // namespace ofh
} // namespace srsran
