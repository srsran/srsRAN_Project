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

/// \file
/// \brief Polar decoding.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/channel_coding/polar/polar_code.h"
#include "srsran/phy/upper/channel_coding/polar/polar_encoder.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include <cstdint>

namespace srsran {

/// Polar decoder interface.
class polar_decoder
{
public:
  /// Default destructor.
  virtual ~polar_decoder() = default;

  /// \brief Decodes the input (LLR) codeword with the specified polar decoder.
  /// \param[out] data_decoded The decoder output vector.
  /// \param[in]  input_llr    The decoder LLR input vector.
  /// \param[in]  code         Polar code descriptor.
  virtual void
  decode(span<uint8_t> data_decoded, span<const log_likelihood_ratio> input_llr, const polar_code& code) = 0;
};

} // namespace srsran
