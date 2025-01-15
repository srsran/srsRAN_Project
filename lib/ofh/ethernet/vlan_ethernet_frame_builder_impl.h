/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/ethernet/ethernet_frame_builder.h"

namespace srsran {
namespace ether {

/// Implementation for the VLAN Ethernet frame builder.
class vlan_frame_builder_impl : public frame_builder
{
public:
  /// Constructor.
  ///
  /// \param[in] eth_params Ethernet message parameters.
  explicit vlan_frame_builder_impl(const vlan_frame_params& eth_params);

  // See interface for documentation.
  units::bytes get_header_size() const override;

  // See interface for documentation.
  void build_frame(span<uint8_t> buffer) override;

private:
  /// Ethernet message parameters.
  const vlan_frame_params eth_params;
};

} // namespace ether
} // namespace srsran
