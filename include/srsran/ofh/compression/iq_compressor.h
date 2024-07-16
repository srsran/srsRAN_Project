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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/ofh/compression/compression_params.h"

namespace srsran {
namespace ofh {

/// \brief Describes the IQ data compressor.
///
/// Compresses and serializes floating point IQ samples together with compression parameters according to compression
/// methods specified in WG4.CUS.0 document.
class iq_compressor
{
public:
  /// Default destructor.
  virtual ~iq_compressor() = default;

  /// \brief Compress input IQ samples.
  ///
  /// Compresses IQ samples from the input buffer according to received compression parameters and puts the results into
  /// an array of compressed PRBs.
  ///
  /// \param[out] buffer   Buffer where the compressed IQ data and compression parameters will be stored.
  /// \param[in]  iq_data  IQ samples to be compressed.
  /// \param[in]  params   Compression parameters.
  virtual void compress(span<uint8_t> buffer, span<const cbf16_t> iq_data, const ru_compression_params& params) = 0;
};

} // namespace ofh
} // namespace srsran
