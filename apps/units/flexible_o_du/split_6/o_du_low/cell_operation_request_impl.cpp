/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cell_operation_request_impl.h"
#include "split6_flexible_o_du_low_factory.h"

using namespace srsran;

bool cell_operation_request_handler_impl::on_start_request(const fapi::fapi_cell_config& config)
{
  // Call the factory.
  flexible_odu_low = create_split6_flexible_o_du_low_impl();

  // Return true when the flexible O-DU low was successfully created, otherwise false.
  return flexible_odu_low != nullptr;
}
