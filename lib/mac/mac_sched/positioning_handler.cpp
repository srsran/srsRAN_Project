/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "positioning_handler.h"

#ifndef SRSRAN_HAS_ENTERPRISE

#include "positioning_handler.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;

namespace {

class disabled_positioning_handler final : public positioning_handler
{
public:
  async_task<mac_cell_positioning_measurement_response>
  handle_positioning_measurement_request(const mac_cell_positioning_measurement_request& req) override
  {
    return launch_no_op_task(mac_cell_positioning_measurement_response{});
  }

  void handle_srs(const mac_srs_indication_message& msg) override {}
};

} // namespace

std::unique_ptr<positioning_handler> srsran::create_positioning_handler()
{
  return std::make_unique<disabled_positioning_handler>();
}

#endif
