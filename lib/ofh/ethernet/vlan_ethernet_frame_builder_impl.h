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

#pragma once

#include "srsran/ofh/ethernet/vlan_ethernet_frame_builder.h"

namespace srsran {
namespace ether {

/// Implementation for the VLAN Ethernet frame builder.
class vlan_frame_builder_impl : public vlan_frame_builder
{
public:
  // See interface for documentation.
  units::bytes get_header_size() const override;

  // See interface for documentation.
  void build_vlan_frame(span<uint8_t> buffer, const vlan_frame_params& eth_params) override;
};

} // namespace ether
} // namespace srsran
