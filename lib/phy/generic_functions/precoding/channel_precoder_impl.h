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
  void apply_precoding(re_buffer_writer<cbf16_t>&     output,
                       const re_buffer_reader<>&      input,
                       const precoding_weight_matrix& precoding) const override;

protected:
  /// \brief Applies precoding to the RE belonging to a single antenna port.
  ///
  /// \param[out] port_re        View over the RE of a single antenna port.
  /// \param[in]  input_re       Input symbols, indexed by RE and transmit layer.
  /// \param[in]  port_weights   Layer weights for the current port.
  virtual void apply_precoding_port(span<cbf16_t>             port_re,
                                    const re_buffer_reader<>& input_re,
                                    span<const cf_t>          port_weights) const = 0;
};

} // namespace srsran
