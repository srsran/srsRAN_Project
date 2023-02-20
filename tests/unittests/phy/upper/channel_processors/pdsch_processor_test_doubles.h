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

#include "srsgnb/phy/upper/channel_processors/pdsch_processor.h"

namespace srsran {

/// Spy implementation of a PDSCH processor.
class pdsch_processor_spy : public pdsch_processor
{
  bool process_method_called = false;

public:
  void process(resource_grid_writer&                                        grid,
               static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                 pdu) override
  {
    process_method_called = true;
  }

  /// Returns true if the process method has been called, false otherwise.
  bool is_process_called() const { return process_method_called; }
};

} // namespace srsran
