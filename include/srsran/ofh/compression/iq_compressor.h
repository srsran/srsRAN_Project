/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
  compress(span<compressed_prb> compressed_prbs, span<const cf_t> iq_data, const ru_compression_params& params) = 0;
};

} // namespace ofh
} // namespace srsran
