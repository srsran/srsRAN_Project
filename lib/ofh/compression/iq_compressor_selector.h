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
  void compress(span<compressed_prb>         compressed_prbs,
                span<const cf_t>             iq_data,
                const ru_compression_params& params) override;

private:
  std::array<std::unique_ptr<iq_compressor>, NOF_COMPRESSION_TYPES_SUPPORTED> compressors;
};

} // namespace ofh
} // namespace srsran
