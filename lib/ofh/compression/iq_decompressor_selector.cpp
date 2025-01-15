/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
                              "Null decompressor detected for compression type '{}'",
                              to_string(static_cast<compression_type>(i)));
  }
}

void iq_decompressor_selector::decompress(span<cbf16_t>                iq_data,
                                          span<const uint8_t>          compressed_data,
                                          const ru_compression_params& params)
{
  return decompressors[static_cast<unsigned>(params.type)]->decompress(iq_data, compressed_data, params);
}
