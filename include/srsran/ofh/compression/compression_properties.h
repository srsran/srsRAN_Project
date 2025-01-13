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
