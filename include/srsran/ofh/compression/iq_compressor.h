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
#include "srsran/ofh/compression/compressed_prb.h"
#include "srsran/ofh/compression/compression_params.h"

namespace srsran {
namespace ofh {

/// \brief Describes the IQ data compressor.
///
/// Compresses floating point IQ samples according to compression methods specified in WG4.CUS.0 document.
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
  /// \param[out] compressed_prbs A span containing compressed PRBs.
  /// \param[in]  iq_data IQ samples to be compressed.
  /// \param[in]  params  Compression parameters.
  virtual void
  compress(span<compressed_prb> compressed_prbs, span<const cbf16_t> iq_data, const ru_compression_params& params) = 0;
};

} // namespace ofh
} // namespace srsran
