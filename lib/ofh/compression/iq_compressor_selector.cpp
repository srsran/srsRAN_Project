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

#include "iq_compressor_selector.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace ofh;

iq_compressor_selector::iq_compressor_selector(
    std::array<std::unique_ptr<iq_compressor>, NOF_COMPRESSION_TYPES_SUPPORTED> compressors_) :
  compressors(std::move(compressors_))
{
  // Sanity check that all the positions in the array has a valid compressor.
  for (unsigned i = 0, e = compressors.size(); i != e; ++i) {
    report_fatal_error_if_not(compressors[i],
                              "Null compressor detected for compression type {}",
                              to_string(static_cast<compression_type>(i)));
  }
}

void iq_compressor_selector::compress(span<compressed_prb>         compressed_prbs,
                                      span<const cf_t>             iq_data,
                                      const ru_compression_params& params)
{
  auto& compressor = compressors[static_cast<unsigned>(params.type)];
  compressor->compress(compressed_prbs, iq_data, params);
}
