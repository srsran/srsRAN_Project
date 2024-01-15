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

namespace srsran {
namespace ether {

/// \brief Describes the VLAN Ethernet frame decoder.
///
/// Decodes a VLAN Ethernet frame following the IEEE 802.3 and IEEE 802.1Q specifications.
class vlan_frame_decoder
{
public:
  /// Default destructor.
  virtual ~vlan_frame_decoder() = default;

  /// \brief Decodes the contents in the given buffer into a VLAN Ethernet frame.
  ///
  /// Decodes the contents of the buffer, filling the fields in the \c eth_params structure. Returns a new span with the
  /// VLAN Ethernet header removed.
  ///
  /// \param[in]  frame Frame to be decoded.
  /// \param[out] eth_params Ethernet message parameters or undefined contents in case of error.
  /// \return A span containing the payload of the VLAN Ethernet frame or an empty span in case of error.
  virtual span<const uint8_t> decode(span<const uint8_t> frame, vlan_frame_params& eth_params) = 0;
};

} // namespace ether
} // namespace srsran
