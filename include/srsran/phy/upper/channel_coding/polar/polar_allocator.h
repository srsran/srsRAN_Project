/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Polar allocator interface.

#pragma once

#include "srsran/phy/upper/channel_coding/polar/polar_code.h"

namespace srsran {

/// Polar allocator interface.
class polar_allocator
{
public:
  /// Default destructor.
  virtual ~polar_allocator() = default;

  /// \brief Allocates the message bits into the encoder input.
  ///
  /// \param[out] input_encoder   Sequence of allocated bits to be fed to the polar encoder.
  /// \param[in]  message         Information bits to be allocated.
  /// \param[in]  code            Polar code description.
  virtual void allocate(span<uint8_t> input_encoder, span<const uint8_t> message, const polar_code& code) = 0;
};

} // namespace srsran
