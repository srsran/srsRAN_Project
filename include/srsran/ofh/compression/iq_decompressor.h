/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/ofh/compression/compressed_prb.h"
#include "srsran/ofh/compression/compression_params.h"

namespace srsran {
namespace ofh {

/// \brief Describes the IQ data decompressor.
///
/// Decompresses received PRBs with compressed IQ data according to compression methods specified in WG4.CUS.0 document.
class iq_decompressor
{
public:
  /// Default destructor.
  virtual ~iq_decompressor() = default;

  /// \brief Decompress received compressed PRBs.
  ///
  /// Decompresses compressed PRBs from the input buffer according to received compression parameters and puts the
  /// results into an array of floating point IQ samples.
  ///
  /// \param[out] iq_data  Resulting IQ samples after decompression.
  /// \param[in]  compressed_prbs A span containing received compressed PRBs.
  /// \param[in]  params  Compression parameters.
  virtual void
  decompress(span<cf_t> iq_data, span<const compressed_prb> compressed_prbs, const ru_compression_params& params) = 0;
};

} // namespace ofh
} // namespace srsran
