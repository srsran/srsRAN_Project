/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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

protected:
  /// \brief Applies precoding to the RE belonging to a single antenna port.
  ///
  /// \param[out] port_re   View over the RE of a single antenna port.
  /// \param[in] input     Input symbols, indexed by RE and transmit layer.
  /// \param[in] precoding Precoding coefficients, indexed by layer.
  virtual void
  apply_precoding_port(span<cf_t> port_re, const re_buffer_reader& input_re, span<const cf_t> port_weights) = 0;
};

} // namespace srsran
