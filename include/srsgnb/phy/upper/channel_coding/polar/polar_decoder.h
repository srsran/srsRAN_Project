/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Polar decoding.

#pragma once

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_encoder.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
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
