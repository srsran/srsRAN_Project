/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
