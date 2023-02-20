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

#include "srsran/phy/upper/channel_processors/ssb_processor.h"

namespace srsran {

/// Spy implementation of a SSB processor.
class ssb_processor_spy : public ssb_processor
{
  bool process_method_called = false;

public:
  void process(resource_grid_writer& grid, const pdu_t& pdu) override { process_method_called = true; }

  /// Returns true if the process method has been called, false otherwise.
  bool is_process_called() const { return process_method_called; }
};

} // namespace srsran
