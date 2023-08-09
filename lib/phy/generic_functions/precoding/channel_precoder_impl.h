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

#include "srsran/phy/generic_functions/precoding/channel_precoder.h"

namespace srsran {

/// Channel precoder implementation.
class channel_precoder_impl : public channel_precoder
{
public:
  /// Default constructor.
  explicit channel_precoder_impl() = default;

  // See interface for documentation.
  void apply_precoding(re_buffer_writer&              output,
                       const re_buffer_reader&        input,
                       const precoding_weight_matrix& precoding) override;
};

} // namespace srsran
