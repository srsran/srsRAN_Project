/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
                              "Null compressor detected for compression type '{}'",
                              to_string(static_cast<compression_type>(i)));
  }
}

void iq_compressor_selector::compress(span<compressed_prb>         compressed_prbs,
                                      span<const cbf16_t>          iq_data,
                                      const ru_compression_params& params)
{
  auto& compressor = compressors[static_cast<unsigned>(params.type)];
  compressor->compress(compressed_prbs, iq_data, params);
}
