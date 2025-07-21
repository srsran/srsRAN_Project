/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_flexible_o_du_low_session_manager.h"
#include "split6_flexible_o_du_low_session_factory.h"

using namespace srsran;

bool split6_flexible_o_du_low_session_manager::on_start_request(const fapi::fapi_cell_config& config)
{
  // Call the factory.
  flexible_odu_low = odu_low_session_factory->create_o_du_low_session(config);

  // Return true when the flexible O-DU low was successfully created, otherwise false.
  return flexible_odu_low != nullptr;
}
