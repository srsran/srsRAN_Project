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

#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_factories.h"
#include "plugin_bbdev_pdsch_enc_acc_factory.h"

using namespace srsran;
using namespace hal;

std::unique_ptr<hw_accelerator_pdsch_enc_factory>
srsran::hal::create_bbdev_pdsch_enc_acc_factory(const bbdev_hwacc_pdsch_enc_factory_configuration& accelerator_config,
                                                std::string                                        impl_name)
{
  // Convert implementation name to lower case.
  for (char& c : impl_name) {
    c = std::tolower(c);
  }

  // Try creating a plugin bbdev-based PDSCH encoder hardware-accelerator factory.
  auto factory = create_plugin_bbdev_pdsch_enc_acc_factory(accelerator_config, impl_name);
  if (factory) {
    return factory;
  }

  // No match.
  fmt::print("Factory for bbdev-based PDSCH encoder hardware-accelerator type {} not found. Make sure to select a "
             "valid type.\n",
             impl_name);

  return nullptr;
}
