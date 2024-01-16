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
#include "srsran/ofh/serdes/ofh_cplane_message_properties.h"

namespace srsran {
namespace ofh {

/// \brief Open Fronthaul Control-Plane message builder interface.
///
/// Builds a Control-Plane message following the O-RAN Open Fronthaul specification.
class cplane_message_builder
{
public:
  /// Default destructor.
  virtual ~cplane_message_builder() = default;

  /// \brief Builds a DL/UL radio channel Control-Plane message into the given buffer using the given
  /// \c parameters.
  ///
  /// This Control-Plane message is used for most DL/UL radio channels.
  ///
  /// \param[out] buffer Buffer where the message will be built.
  /// \param[in] msg_params Control-Plane parameters.
  /// \return Number of bytes serialized in the buffer.
  virtual unsigned build_dl_ul_radio_channel_message(span<uint8_t>                          buffer,
                                                     const cplane_section_type1_parameters& msg_params) = 0;

  /// \brief Builds Control-Plane message aimed at idle or guard periods indication from O-DU to O-RU.
  ///
  /// \param[out] buffer    Buffer where the message will be built.
  /// \param[in] msg_params Control-Plane parameters.
  /// \return Number of bytes serialized in the buffer.
  virtual unsigned build_idle_guard_period_message(span<uint8_t>                          buffer,
                                                   const cplane_section_type0_parameters& msg_params) = 0;

  /// \brief Builds PRACH and mixed-numerology channel Control-Plane message into the given buffer using the given
  /// \c parameters.
  ///
  /// \param[out] buffer    Buffer where the message will be built.
  /// \param[in] msg_params Control-Plane parameters.
  /// \return Number of bytes serialized in the buffer.
  virtual unsigned build_prach_mixed_numerology_message(span<uint8_t>                          buffer,
                                                        const cplane_section_type3_parameters& msg_params) = 0;
};

} // namespace ofh
} // namespace srsran
