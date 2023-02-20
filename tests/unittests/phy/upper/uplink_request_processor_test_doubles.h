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

#include "srsgnb/phy/upper/uplink_request_processor.h"

namespace srsran {

/// Dummy implementation of an uplink request processor.
class uplink_request_processor_dummy : public uplink_request_processor
{
public:
  void process_prach_request(const prach_buffer_context& context) override {}
  void process_uplink_slot_request(const resource_grid_context& context, resource_grid& grid) override {}
};

} // namespace srsran
