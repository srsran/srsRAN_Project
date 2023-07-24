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

#include "iq_decompressor_selector.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace ofh;

iq_decompressor_selector::iq_decompressor_selector(
    std::array<std::unique_ptr<iq_decompressor>, NOF_COMPRESSION_TYPES_SUPPORTED> decompressors_) :
  decompressors(std::move(decompressors_))
{
  // Sanity check that all the positions in the array has a decompressor.
  for (unsigned i = 0, e = decompressors.size(); i != e; ++i) {
    report_fatal_error_if_not(decompressors[i],
                              "Null decompressor detected for compression type {}",
                              to_string(static_cast<compression_type>(i)));
  }
}

void iq_decompressor_selector::decompress(span<cf_t>                   iq_data,
                                          span<const compressed_prb>   compressed_prbs,
                                          const ru_compression_params& params)
{
  return decompressors[static_cast<unsigned>(params.type)]->decompress(iq_data, compressed_prbs, params);
}
