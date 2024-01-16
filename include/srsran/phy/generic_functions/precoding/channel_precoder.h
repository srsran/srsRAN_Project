/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
/// \brief Channel precoder interface.

#pragma once

#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/phy/support/re_buffer.h"

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
  /// \remark An assertion is triggered if the output buffer does not have the same number of RE per port as the number
  /// of RE per layer of the input buffer.
  /// \remark An assertion is triggered if the precoding matrix dimensions do not match the number of layers of the
  /// input buffer and the number of antenna ports of the output buffer.
  virtual void apply_precoding(re_buffer_writer&              output,
                               const re_buffer_reader&        input,
                               const precoding_weight_matrix& precoding) const = 0;

  /// \brief Maps the input symbols into layers and applies a set of precoding weights.
  /// \param[out] output   Output symbols, indexed by RE and antenna port.
  /// \param[in] input     Input symbols.
  /// \param[in] precoding Precoding coefficients arranged in matrix form, indexed by transmit layer and antenna port.
  /// \remark An assertion is triggered if the output buffer does not have the same number of RE per port as the number
  /// of RE per layer of the input buffer.
  /// \remark An assertion is triggered if the precoding matrix dimensions are not consistent with input buffer size and
  /// the number of antenna ports of the output buffer.
  virtual void apply_layer_map_and_precoding(re_buffer_writer&              output,
                                             span<const ci8_t>              input,
                                             const precoding_weight_matrix& precoding) const = 0;
};

} // namespace srsran
