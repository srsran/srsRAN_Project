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
#include "srsran/ofh/ethernet/vlan_ethernet_frame_params.h"
#include "srsran/support/units.h"

namespace srsran {
namespace ether {

/// \brief Describes the VLAN Ethernet frame builder.
///
/// Builds a VLAN Ethernet frame following the IEEE 802.3 and IEEE 802.1Q specifications.
class vlan_frame_builder
{
public:
  /// Default destructor.
  virtual ~vlan_frame_builder() = default;

  /// Returns the VLAN Ethernet header size in bytes.
  virtual units::bytes get_header_size() const = 0;

  /// \brief Builds a VLAN Ethernet frame using the given parameters into \c buffer.
  ///
  /// This function expects that the buffer already contains the payload of the frame. The builder will write the
  /// header in the first \ref vlan_ethernet_frame_builder::get_header_size bytes of the buffer.
  ///
  /// \param[in] buffer Buffer where the frame will be built.
  /// \param[in] eth_params Ethernet message parameters.
  virtual void build_vlan_frame(span<uint8_t> buffer, const vlan_frame_params& eth_params) = 0;
};

} // namespace ether
} // namespace srsran
