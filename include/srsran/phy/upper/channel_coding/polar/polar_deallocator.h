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
/// \brief Polar deallocator - Interface.

#pragma once

#include "srsran/phy/upper/channel_coding/polar/polar_code.h"

namespace srsran {

/// Polar deallocator interface.
class polar_deallocator
{
public:
  /// Default destructor.
  virtual ~polar_deallocator() = default;

  /// \brief Extracts the information bits from the polar decoder output.
  ///
  /// \param[out] message         Information bits.
  /// \param[in]  output_decoder  Output bits of the polar decoder.
  /// \param[in]  code            Polar code description.
  virtual void deallocate(span<uint8_t> message, span<const uint8_t> output_decoder, const polar_code& code) = 0;
};

} // namespace srsran
