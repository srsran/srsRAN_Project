/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
namespace testing {

/// Spy VLAN frame builder implementation.
class vlan_frame_builder_spy : public frame_builder
{
  bool              build_vlan_frame_method_called = false;
  vlan_frame_params params;

public:
  explicit vlan_frame_builder_spy(const vlan_frame_params& eth_params) : params(eth_params) {}

  // See interface for documentation.
  units::bytes get_header_size() const override { return units::bytes(18); }

  // See interface for documentation.
  void build_frame(span<uint8_t> buffer) override { build_vlan_frame_method_called = true; }

  /// Returns true if the build VLAN frame has been called, otherwise false.
  bool has_build_vlan_frame_method_been_called() const { return build_vlan_frame_method_called; }

  /// Returns the ethernet parameters processed by this builder.
  const vlan_frame_params& get_vlan_frame_params() const { return params; }
};

} // namespace testing
} // namespace ether
} // namespace srsran
