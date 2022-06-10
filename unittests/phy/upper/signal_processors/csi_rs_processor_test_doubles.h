/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_CSI_RS_PROCESSOR_TEST_DOUBLES_H
#define SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_CSI_RS_PROCESSOR_TEST_DOUBLES_H

#include "srsgnb/phy/upper/signal_processors/csi_rs_processor.h"

namespace srsgnb {

/// Spy implementation of a CSI-RS processor.
class csi_rs_processor_spy : public csi_rs_processor
{
  bool map_method_called = false;

public:
  void map(resource_grid_writer& grid, const config_t& config) override { map_method_called = true; }

  /// Returns true if the map method has been called, false otherwise.
  bool is_map_called() const { return map_method_called; }
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_CSI_RS_PROCESSOR_TEST_DOUBLES_H
