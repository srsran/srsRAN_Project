/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/mac/mac_factory.h"
#include "mac_impl.h"
#include "srsran/du_high/du_high_executor_mapper.h"

using namespace srsran;

std::unique_ptr<mac_interface> srsran::create_mac(const mac_config& mac_cfg)
{
  std::unique_ptr<mac_interface> mac_obj = std::make_unique<mac_impl>(mac_cfg);
  return mac_obj;
}
