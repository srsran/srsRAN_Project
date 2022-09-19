/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/uplink_processor.h"

namespace srsgnb {

class uplink_processor_spy : public uplink_processor
{
  bool has_proces_prach_method_called = false;

public:
  void process_prach(const prach_buffer& buffer, const prach_buffer_context& context) override
  {
    has_proces_prach_method_called = true;
  }

  void process_pusch(span<uint8_t>                      data,
                     rx_softbuffer&                     softbuffer,
                     const resource_grid_reader&        grid,
                     const uplink_processor::pusch_pdu& pdu) override
  {
  }

  bool has_process_prach_method_called() const { return has_proces_prach_method_called; }
};

} // namespace srsgnb
