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

#include "../../phy_test_utils.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"

namespace srsran {

/// Spy implementation of a PDCCH processor.
class pdcch_processor_spy : public pdcch_processor
{
  bool process_method_called = false;

public:
  void process(resource_grid_writer& grid, const pdu_t& pdu) override { process_method_called = true; }

  /// Returns true if the process method has been called, false otherwise.
  bool is_process_called() const { return process_method_called; }
};

PHY_SPY_FACTORY_TEMPLATE(pdcch_processor);

} // namespace srsran
