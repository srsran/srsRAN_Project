/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
