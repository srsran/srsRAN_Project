/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_factories.h"
#include "plugin_bbdev_pusch_dec_acc_factory.h"

using namespace srsran;
using namespace hal;

std::unique_ptr<hw_accelerator_pusch_dec_factory>
srsran::hal::create_bbdev_pusch_dec_acc_factory(const bbdev_hwacc_pusch_dec_factory_configuration& accelerator_config,
                                                std::string                                        impl_name)
{
  // Convert implementation name to lower case.
  for (char& c : impl_name) {
    c = std::tolower(c);
  }

  // Try creating a plugin bbdev-based PUSCH decoder hardware-accelerator factory.
  auto factory = create_plugin_bbdev_pusch_dec_acc_factory(accelerator_config, impl_name);
  if (factory) {
    return factory;
  }

  // No match.
  fmt::print("Factory for bbdev-based PUSCH decoder hardware-accelerator type {} not found. Make sure to select a "
             "valid type.\n",
             impl_name);

  return nullptr;
}
