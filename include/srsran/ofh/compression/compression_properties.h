/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/compression/compression_params.h"
#include "srsran/ran/resource_block.h"
#include "srsran/support/units.h"

namespace srsran {
namespace ofh {

/// Returns true if the compression parameter is present based on the given compression type.
constexpr bool is_compression_param_present(compression_type type)
{
  return (type == compression_type::BFP) || (type == compression_type::mu_law) ||
         (type == compression_type::bfp_selective) || (type == compression_type::mod_selective);
}

/// Returns size of a PRB compressed according to the given compression parameters.
inline units::bytes get_compressed_prb_size(const ru_compression_params& params)
{
  static constexpr units::bytes compr_param_size{1};

  unsigned prb_size = NOF_SUBCARRIERS_PER_RB * 2 * params.data_width;
  if (is_compression_param_present(params.type)) {
    prb_size += compr_param_size.to_bits().value();
  }
  return units::bits(prb_size).round_up_to_bytes();
}

} // namespace ofh
} // namespace srsran
