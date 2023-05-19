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
/// \brief Channel precoder interface.

#pragma once

#include "srsran/phy/upper/re_buffer.h"
#include "srsran/ran/precoding/precoding_configuration.h"

namespace srsran {

/// Channel precoder interface.
class channel_precoder
{
public:
  /// Default destructor.
  virtual ~channel_precoder() = default;

  /// \brief Applies a set of precoding weights to the input resource elements.
  ///
  /// \param[out] output   Output symbols, indexed by RE and antenna port.
  /// \param[in] input     Input symbols, indexed by RE and transmit layer.
  /// \param[in] precoding Precoding coefficients arranged in matrix form, indexed by transmit layer and antenna port.
  virtual void apply_precoding(re_buffer_writer&              output,
                               const re_buffer_reader&        input,
                               const precoding_weight_matrix& precoding) = 0;
};

} // namespace srsran
