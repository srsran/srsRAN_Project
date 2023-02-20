/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/signal_processors/nzp_csi_rs_generator.h"

namespace srsgnb {

/// Spy implementation of a CSI-RS processor.
class csi_rs_processor_spy : public nzp_csi_rs_generator
{
  bool map_method_called = false;

public:
  void map(resource_grid_writer& grid, const config_t& config) override { map_method_called = true; }

  /// Returns true if the map method has been called, false otherwise.
  bool is_map_called() const { return map_method_called; }
};

} // namespace srsgnb
