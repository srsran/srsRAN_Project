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

#include "srsgnb/adt/span.h"
#include <cstdint>

namespace srsgnb {

class polar_encoder
{
public:
  virtual ~polar_encoder() = default;

  /*!
   * Encodes the input vector into a codeword with the specified polar encoder.
   * \param[in] input The encoder input vector.
   * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the encoder input/output vector.
   *     It cannot be larger than the maximum code_size_log specified in q.code_size_log of
   *     the srsran_polar_encoder_t structure.
   * \param[out] output The encoder output vector.
   */
  virtual void encode(span<uint8_t> output, span<const uint8_t> input, unsigned code_size_log) = 0;
};

} // namespace srsgnb
