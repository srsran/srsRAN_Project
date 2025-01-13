/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
