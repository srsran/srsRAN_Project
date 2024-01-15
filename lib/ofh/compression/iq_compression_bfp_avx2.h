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

#include "iq_compression_bfp_impl.h"

namespace srsran {
namespace ofh {

/// Implementation of the Block Floating Point IQ data compression using AVX2 intrinsics.
class iq_compression_bfp_avx2 : public iq_compression_bfp_impl
{
public:
  // Constructor.
  explicit iq_compression_bfp_avx2(srslog::basic_logger& logger_, float iq_scaling_ = 1.0) :
    iq_compression_bfp_impl(logger_, iq_scaling_)
  {
  }

  // See interface for the documentation.
  void compress(span<compressed_prb> output, span<const cf_t> input, const ru_compression_params& params) override;

  // See interface for the documentation.
  void decompress(span<cf_t> output, span<const compressed_prb> input, const ru_compression_params& params) override;
};

} // namespace ofh
} // namespace srsran
