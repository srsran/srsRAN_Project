/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_fapi_p5_sector_operation_request_adaptor.h"

using namespace srsran;
using namespace fapi_adaptor;

fapi::config_message_gateway& phy_fapi_p5_sector_operation_request_adaptor::get_config_message_gateway()
{
  return gateway;
}

void phy_fapi_p5_sector_operation_request_adaptor::set_config_message_notifier(
    fapi::config_message_notifier& config_notifier)
{
  gateway.set_config_message_notifier(config_notifier);
}

void phy_fapi_p5_sector_operation_request_adaptor::set_error_message_notifier(
    fapi::error_message_notifier& err_notifier)
{
  gateway.set_error_message_notifier(err_notifier);
}

void phy_fapi_p5_sector_operation_request_adaptor::set_cell_operation_request_notifier(
    fapi::cell_operation_request_notifier& cell_notifier)
{
  gateway.set_cell_operation_request_notifier(cell_notifier);
}
