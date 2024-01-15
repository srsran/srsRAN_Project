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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/ofh/serdes/ofh_uplane_message_properties.h"
#include "srsran/support/units.h"

namespace srsran {
namespace ofh {

/// \brief Open Fronthaul User-Plane message builder interface.
///
/// Builds a User Plane message following the O-RAN Open Fronthaul specification.
class uplane_message_builder
{
public:
  /// Default destructor.
  virtual ~uplane_message_builder() = default;

  /// Returns the Open Fronthaul User-Plane header size in bytes.
  virtual units::bytes get_header_size(const ru_compression_params& params) const = 0;

  /// \brief Builds a User Plane message given the \c config parameters, places result in \c buffer.
  ///
  /// \param[out] buffer Buffer where the message will be built.
  /// \param[in] iq_data IQ samples.
  /// \param[in] config  User plane message parameters.
  /// \return Number of bytes serialized in the buffer.
  virtual unsigned
  build_message(span<uint8_t> buffer, span<const cf_t> iq_data, const uplane_message_params& params) = 0;
};

} // namespace ofh
} // namespace srsran
