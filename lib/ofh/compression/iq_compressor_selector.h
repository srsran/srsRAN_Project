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

#include "srsran/ofh/compression/iq_compressor.h"

namespace srsran {
namespace ofh {

/// \brief IQ compressor selector implementation.
///
/// The selector will select the IQ compressor between the registered ones to compress IQ samples, based on the
/// given compression parameters.
class iq_compressor_selector : public iq_compressor
{
public:
  explicit iq_compressor_selector(
      std::array<std::unique_ptr<iq_compressor>, NOF_COMPRESSION_TYPES_SUPPORTED> compressors_);

  // See interface for documentation.
  void compress(span<uint8_t> buffer, span<const cbf16_t> iq_data, const ru_compression_params& params) override;

private:
  std::array<std::unique_ptr<iq_compressor>, NOF_COMPRESSION_TYPES_SUPPORTED> compressors;
};

} // namespace ofh
} // namespace srsran
