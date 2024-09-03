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

#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "plugin_bbdev_acc_factory.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"

using namespace srsran;
using namespace dpdk;

std::unique_ptr<bbdev_acc_factory> srsran::dpdk::create_bbdev_acc_factory(std::string impl_name)
{
  // Convert implementation name to lower case.
  for (char& c : impl_name) {
    c = std::tolower(c);
  }

  // Try creating a plugin bbdev accelerator factory.
  auto factory = create_plugin_bbdev_acc_factory(impl_name);
  if (factory) {
    return factory;
  }

  // No match.
  fmt::print("Factory for bbdev accelerator type {} not found. Make sure to select a valid type.\n", impl_name);

  return nullptr;
}
