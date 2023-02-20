/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/channel_coding/polar/polar_code.h"
#include <cstdint>

namespace srsran {

class polar_rate_matcher
{
public:
  virtual ~polar_rate_matcher() = default;

  /*!
   * Carries out the actual rate-matching.
   * \param[in] input        The codeword obtained from the polar encoder.
   * \param[out] output      The rate-matched codeword resulting from the rate-matching
   *                         operation.
   * \param[in] n            \f$log_2\f$ of the codeword length.
   * \param[in] E            Rate-matched codeword length.
   * \param[in] K            Message size (including CRC).
   * \return An integer: 0 if the function executes correctly, -1 otherwise.
   */
  virtual void rate_match(span<uint8_t> output, span<const uint8_t> input, const polar_code& code) = 0;
};

} // namespace srsran
