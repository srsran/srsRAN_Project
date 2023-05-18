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
/// \brief Channel precoder implementation for an arbitrary number of layers and transmit ports.

#pragma once

#include "srsran/phy/upper/precoding/channel_precoder.h"

namespace srsran {

/// Channel precoder implementation.
class channel_precoder_impl : public channel_precoder
{
public:
  /// Dimensions of the precoding matrix.
  using weight_dims = precoding_configuration::dims;

  /// Default constructor.
  explicit channel_precoder_impl() = default;

  // See interface for documentation.
  void apply_precoding(re_buffer_writer_view&                        output,
                       const re_buffer_reader_view&                  input,
                       const precoding_configuration::weight_matrix& precoding) override;
};

} // namespace srsran
