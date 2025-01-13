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

/// \brief Describes the IQ data decompressor.
///
/// Deserializes compression parameters and decompresses received PRBs with compressed IQ data according to compression
/// methods specified in WG4.CUS.0 document.
class iq_decompressor
{
public:
  /// Default destructor.
  virtual ~iq_decompressor() = default;

  /// \brief Decompress received compressed PRBs.
  ///
  /// Decompresses compressed PRBs from the input buffer according to received compression parameters and puts the
  /// results into an array of brain floating point IQ samples.
  ///
  /// \param[out] iq_data  Resulting IQ samples after decompression.
  /// \param[in]  compressed_data A span containing received compressed IQ data and compression parameters.
  /// \param[in]  params  Compression parameters.
  virtual void
  decompress(span<cbf16_t> iq_data, span<const uint8_t> compressed_data, const ru_compression_params& params) = 0;
};

} // namespace ofh
} // namespace srsran
