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
/// \brief Polar encoder interface.

#pragma once

#include "srsran/adt/span.h"
#include <cstdint>

namespace srsran {

/// Polar encoder interface.
class polar_encoder
{
public:
  /// Default destructor.
  virtual ~polar_encoder() = default;

  /// \brief Encodes the input vector into a codeword with the specified polar encoder.
  /// \param[out] output       The encoder output vector.
  /// \param[in]  input        The encoder input vector.
  /// \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the encoder input/output vector.
  ///     It cannot be larger than the maximum \c code_size_log specified in \c q.code_size_log of
  ///     the \c srsran_polar_encoder_t structure.
  virtual void encode(span<uint8_t> output, span<const uint8_t> input, unsigned code_size_log) = 0;
};

} // namespace srsran
