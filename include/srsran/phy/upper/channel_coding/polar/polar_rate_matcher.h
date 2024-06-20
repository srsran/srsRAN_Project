/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Polar rate matcher interface.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/channel_coding/polar/polar_code.h"
#include <cstdint>

namespace srsran {

/// Polar rate matcher interface.
class polar_rate_matcher
{
public:
  /// Default destructor.
  virtual ~polar_rate_matcher() = default;

  /// \brief Carries out the actual rate-matching.
  /// \param[out] output  The rate-matched codeword resulting from the rate-matching operation.
  /// \param[in]  input   The codeword obtained from the polar encoder.
  /// \param[in]  code    The polar code description.
  virtual void rate_match(span<uint8_t> output, span<const uint8_t> input, const polar_code& code) = 0;
};

} // namespace srsran
