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

#pragma once

#include "srsran/adt/span.h"
#include "srsran/adt/variant.h"
#include "srsran/ofh/ecpri/ecpri_packet_properties.h"

namespace srsran {
namespace ecpri {

/// eCPRI packet parameter.
struct packet_parameters {
  /// Common header.
  common_header header;
  /// eCPRI type parameters.
  variant<realtime_control_parameters, iq_data_parameters> type_params;
};

/// eCPRI packet decoder interface.
class packet_decoder
{
public:
  /// Default destructor.
  virtual ~packet_decoder() = default;

  /// \brief Decodes the given packet, filling the eCPRI params and returning a span to the eCPRI payload.
  ///
  /// If the decoder detects an unsupported value, it will return an empty span and the params value will be undefined.
  ///
  /// \param[in] packet eCPRI packet to decode.
  /// \param[out] params eCPRI decoded parameters.
  /// \return A span to the eCPRI payload on success, otherwise an empty span in case of error.
  virtual span<const uint8_t> decode(span<const uint8_t> packet, packet_parameters& params) = 0;
};

} // namespace ecpri
} // namespace srsran
