/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
